#include <stdio.h> /* printf */
#include <stdlib.h> /* rand */
#include <time.h> /* clock */

#include <set> /* std::set<int> */
#include "scapegoat.h"

#define INSERT 0
#define SEARCH 1
#define DELETE 2
#define MAX_OP 3

struct bench_node {
	int key;
	unsigned char op : 2;
};

inline double time_elapsed(time_t begin, time_t end) {
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

inline void report(unsigned int n, time_t begin) {
	double t = time_elapsed(begin, clock());
	printf("%.3f s, %.3f Mop/s\n", t, n / (t * 1000 * 1000));
}

void bench_sg(const char* type, t_sg_tree* sg_tree, unsigned int n, struct bench_node** bench_nodes) {
	unsigned int i;
	t_sg_node *sg_node;
	time_t begin;
	begin = clock();
	for (i = 0; i < n; ++i) {
		if (bench_nodes[i]->op == INSERT) {
			sg_insert(sg_tree, bench_nodes[i]->key);
		}
		else if (bench_nodes[i]->op == SEARCH) {
			sg_node = sg_search(sg_tree, bench_nodes[i]->key);
		}
		else {
			sg_delete(sg_tree, bench_nodes[i]->key);
		}
	}
	printf("SG-%.2f %s: ", sg_tree->alpha, type);
	report(n, begin);
}

void bench_rb(const char* type, std::set<int>* rb_tree, unsigned int n, struct bench_node** bench_nodes) {
	unsigned int i;
	std::set<int>::iterator it;
	time_t begin;
	begin = clock();
	for (i = 0; i < n; ++i) {
		if (bench_nodes[i]->op == INSERT) {
			rb_tree->insert(bench_nodes[i]->key);
		}
		else if (bench_nodes[i]->op == SEARCH) {
			it = rb_tree->find(bench_nodes[i]->key);
		}
		else {
			rb_tree->erase(bench_nodes[i]->key);
		}
	}
	printf("RB %s: ", type);
	report(n, begin);
}

int main(void) {
	double alphas[] = {0.55, 0.6, 0.65, 0.7, 0.75};
	unsigned int i, j, k, r, M = 10, n = M * 1000 * 1000, n_alphas = sizeof(alphas) / sizeof(alphas[0]), rand_max = 10 * 1000;
	struct bench_node **bench_nodes, *temp;
	struct sg_tree* sg_tree[sizeof(alphas) / sizeof(alphas[0])];
	std::set<int> rb_tree;

	printf("=== BENCHMARK - START ===\n");

	bench_nodes = (bench_node**)malloc(sizeof(struct bench_node*) * n);
	for (i = 0; i < n; ++i) {
		bench_nodes[i] = (bench_node*)malloc(sizeof(struct bench_node));
	}

	for (j = 0; j < n_alphas; j++) {
		sg_tree[j] = sg_create_tree(alphas[j]);
	}
	
	for (k = 0; k < 2; k++) {
		if (k == 0) {
			printf("BENCHMARK - Sequential test %dM INSERT -> %dM SEARCH -> %dM DELETE\n", M, M, M);
			for (i = 0; i < n; ++i) {
				bench_nodes[i]->key = i;
			}
		}
		else {
			printf("BENCHMARK - Shuffled test %dM INSERT -> %dM SEARCH -> %dM DELETE\n", M, M, M);
			for (i = 0; i < n; ++i) {
				// swap key_nodes[i] <-> key_nodes[r]
				r = rand() % n;
				temp = bench_nodes[r];
				bench_nodes[r] = bench_nodes[i];
				bench_nodes[i] = temp;
			}
		}

		// RB
		rb_tree.clear();

		printf("BENCHMARK - RB\n");
		for (i = 0; i < n; ++i) {
			bench_nodes[i]->op = INSERT;
		}
		bench_rb("INSERT", &rb_tree, n, bench_nodes);

		for (i = 0; i < n; ++i) {
			bench_nodes[i]->op = SEARCH;
		}
		bench_rb("SEARCH", &rb_tree, n, bench_nodes);

		for (i = 0; i < n; ++i) {
			bench_nodes[i]->op = DELETE;
		}
		bench_rb("DELETE", &rb_tree, n, bench_nodes);

		// SG
		for (j = 0; j < n_alphas; j++) {
			sg_clear_tree(sg_tree[j]);

			printf("BENCHMARK - SG-%.2f\n", alphas[j]);
			for (i = 0; i < n; ++i) {
				bench_nodes[i]->op = INSERT;
			}
			bench_sg("INSERT", sg_tree[j], n, bench_nodes);

			for (i = 0; i < n; ++i) {
				bench_nodes[i]->op = SEARCH;
			}
			bench_sg("SEARCH", sg_tree[j], n, bench_nodes);

			for (i = 0; i < n; ++i) {
				bench_nodes[i]->op = DELETE;
			}
			bench_sg("DELETE", sg_tree[j], n, bench_nodes);
		}
	}

	// Random 
	printf("BENCHMARK - Random test %dMops, rand_max key = %d\n", M, rand_max);
	for (i = 0; i < n; ++i) {
		bench_nodes[i]->op = rand() % MAX_OP;
		bench_nodes[i]->key = rand() % rand_max;
	}

	// RB
	rb_tree.clear();
	printf("BENCHMARK - RB\n");
	bench_rb("RANDOM", &rb_tree, n, bench_nodes);
	rb_tree.clear();

	// SG
	for (j = 0; j < n_alphas; j++) {
		sg_clear_tree(sg_tree[j]);
		printf("BENCHMARK - SG-%.2f\n", alphas[j]);
		bench_sg("RANDOM", sg_tree[j], n, bench_nodes);
		sg_clear_tree(sg_tree[j]);
	}

	for (j = 0; j < n_alphas; j++) {
		sg_delete_tree(sg_tree[j]);
	}

	for (i = 0; i < n; ++i) {
		free(bench_nodes[i]);
	}
	free(bench_nodes);

	printf("=== BENCHMARK - END ===\n");

	return 0;
}
