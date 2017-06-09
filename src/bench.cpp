/*
Copyright (C) 2014-present Jung-Sang Ahn <jungsang.ahn@gmail.com>
All rights reserved.

Last modification: Jan 22, 2017

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free, rand */
#include <time.h> /* clock */
#include <set> /* std::set */

#include "avltree.h"
#include "scapegoat.h"

#define INSERT 0
#define SEARCH 1
#define DELETE 2
#define MAX_OP 3

struct ako_node {
    struct avl_node avl;
    int key;
    unsigned char op : 2;
};

int cmp_func(struct avl_node *a, struct avl_node *b, void *aux) {
    struct ako_node *aa, *bb;
    aa = _get_entry(a, struct ako_node, avl);
    bb = _get_entry(b, struct ako_node, avl);

    if (aa->key < bb->key) return -1;
    else if (aa->key > bb->key) return 1;
    else return 0;
}

double time_elapsed(time_t begin, time_t end) {
	return (double)(end - begin) / CLOCKS_PER_SEC;
}

void report(char* s, unsigned int n, time_t begin) {
	double t = time_elapsed(begin, clock());
	printf("%s: %.3f s, %.3f Mop/s\n", s, t, n / (t * 1000 * 1000));
}

void test_sg(t_sg_tree *sg_tree, unsigned int n, struct ako_node** ako_nodes) {
	unsigned int i;
	t_sg_node *sg_node;

	for (i = 0; i < n; ++i) {
		if (ako_nodes[i]->op == INSERT) {
		    sg_insert(sg_tree, ako_nodes[i]->key);
	    } else if (ako_nodes[i]->op == SEARCH) {
	    	sg_node = sg_search(sg_tree, ako_nodes[i]->key);
	    } else {
		    sg_delete(sg_tree, ako_nodes[i]->key);
		}
	}
}

void test_avl(struct avl_tree* avl_tree, unsigned int n, struct ako_node** ako_nodes) {
	unsigned int i;
	struct ako_node query;
	struct avl_node *avl_node;

    for (i = 0; i < n; ++i) {
	 	if (ako_nodes[i]->op == INSERT) {   
			avl_insert(avl_tree, &ako_nodes[i]->avl, cmp_func);
	    } else if (ako_nodes[i]->op == SEARCH) {
		    query.key = ako_nodes[i]->key;
		    avl_node = avl_search(avl_tree, &query.avl, cmp_func);
	    } else {
		    avl_remove(avl_tree, &ako_nodes[i]->avl);
		}
	}
}

void test_rb(std::set<int>* rb_tree, unsigned int n, struct ako_node** ako_nodes) {
	unsigned int i;
	std::set<int>::iterator it;

	for (i = 0; i < n; ++i) {
		if (ako_nodes[i]->op == INSERT) { 
			rb_tree->insert(ako_nodes[i]->key);
	    } else if (ako_nodes[i]->op == SEARCH) {
		    it = rb_tree->find(ako_nodes[i]->key);
	    } else {
		    rb_tree->erase(ako_nodes[i]->key);
		}
	}
}

int main(void) {
	double alphas[] = {0.55, 0.6, 0.65, 0.7, 0.75};
    unsigned int i, j, k, r, M = 1, n = M * 1000 * 1000, n_alphas = sizeof(alphas) / sizeof(alphas[0]), rand_max = 10 * 1000;
    time_t begin;
    struct ako_node **ako_nodes, *temp;
    struct avl_tree avl_tree;
    struct sg_tree* sg_tree;
    std::set<int> rb_tree;

    printf("=== BENCHMARK - START ===\n");

    ako_nodes = (ako_node**)malloc(sizeof(struct key_nodes_op*) * n);
    for (i = 0; i < n; ++i) {
		ako_nodes[i] = (ako_node*)malloc(sizeof(struct ako_node));
	}

	for (k = 0; k < 3; k++) {
		if (k == 0) {
		    printf("BENCHMARK - Sequential test %dM INSERT -> %dM SEARCH -> %dM DELETE\n", M, M, M);
		    for (i = 0; i < n; ++i) {
		    	ako_nodes[i]->key = i;
		    }
		} else if (k == 1) {
			printf("BENCHMARK - Shuffled test %dM INSERT -> %dM SEARCH -> %dM DELETE\n", M, M, M);
			for (i = 0; i < n; ++i) {
		        // swap key_nodes[i] <-> key_nodes[r]
		        r = rand() % n;
		        temp = ako_nodes[r];
				ako_nodes[r] = ako_nodes[i];
				ako_nodes[i] = temp;
		    }
		} else if (k == 2) {
			// TODO Not working for AVL, null pointer
			// Random test

			printf("BENCHMARK - Random test %dMops, rand_max key = %d\n", M, rand_max);
		    for (i = 0; i < n; ++i) {
				ako_nodes[i]->op = rand() % MAX_OP;
				ako_nodes[i]->key = rand() % rand_max;
		    }
		}

		if (k != 2) {
		    // AVL
		    avl_init(&avl_tree, NULL);
			for (i = 0; i < n; ++i) {
				ako_nodes[i]->op = INSERT;
			}
		    begin = clock();
		    test_avl(&avl_tree, n, ako_nodes);
		    report("AVL INSERT", n, begin);
			for (i = 0; i < n; ++i) {
				ako_nodes[i]->op = SEARCH;
			}
		    begin = clock();
		    test_avl(&avl_tree, n, ako_nodes);
		    report("AVL SEARCH", n, begin);
			for (i = 0; i < n; ++i) {
				ako_nodes[i]->op = DELETE;
			}
		    begin = clock();
		    test_avl(&avl_tree, n, ako_nodes);
		    report("AVL DELETE", n, begin);
		}

	    // RB
	    rb_tree.clear();
	    if (k != 2) {
			for (i = 0; i < n; ++i) {
				ako_nodes[i]->op = INSERT;
		    }
		}
	    begin = clock();
	    test_rb(&rb_tree, n, ako_nodes);
	    report("RB INSERT", n, begin);
		if (k != 2) {
		    for (i = 0; i < n; ++i) {
				ako_nodes[i]->op = SEARCH;
		    }
		}
	    begin = clock();
	    test_rb(&rb_tree, n, ako_nodes);
	    report("RB SEARCH", n, begin);
	    if (k != 2) {
		    for (i = 0; i < n; ++i) {
				ako_nodes[i]->op = DELETE;
		    }
		}
	    begin = clock();
	    test_rb(&rb_tree, n, ako_nodes);
	    report("RB DELETE", n, begin);	    

	    // SG
	    for (j = 0; j < n_alphas; j++) {
	    	sg_tree = sg_create_tree(alphas[j]);
	    	printf("BENCHMARK - SG %.2f\n", alphas[j]);
	    	if (k != 2) {
		    	for (i = 0; i < n; ++i) {
					ako_nodes[i]->op = INSERT;
			    }
			}
		    begin = clock();
		    test_sg(sg_tree, n, ako_nodes);
		    report("SG INSERT", n, begin);
		    if (k != 2) {
			    for (i = 0; i < n; ++i) {
					ako_nodes[i]->op = SEARCH;
			    }
			}
		    begin = clock();
		    test_sg(sg_tree, n, ako_nodes);
		    report("SG SEARCH", n, begin);
		    if (k != 2) {
			    for (i = 0; i < n; ++i) {
					ako_nodes[i]->op = DELETE;
			    }
			}
		    begin = clock();
		    test_sg(sg_tree, n, ako_nodes);
		    report("SG DELETE", n, begin);

	    	sg_delete_tree(sg_tree);
	    }
	}

	for (i = 0; i < n; ++i) {
		free(ako_nodes[i]);
	}
	free(ako_nodes);

	printf("=== BENCHMARK - END ===\n");

    return 0;
}
