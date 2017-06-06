#include <stdio.h>	/* printf */
#include <stdlib.h> /* rand */
#include <assert.h> /* assert */
#include "scapegoat.h"

int main(void) {
	t_sg_tree* tree;
	unsigned int i, k = 10000, rand_max = 10000, size;
	int val;
	char res;

	printf("Ciao mondo\n");

	tree = sg_create_tree(0.55);

	assert(tree->size == 0);

	for (i = 0; i < k; i++) {
		size = sg_calc_size(tree->root);
		val = rand() % rand_max;
		res = sg_insert(tree, val);
		assert(size + res == sg_calc_size(tree->root));
	}

	size = sg_calc_size(tree->root);

	for (i = 0; i < k; i++) {
		val = rand() % rand_max;
		sg_search(tree, val);
		assert(size == sg_calc_size(tree->root));
	}

	for (i = 0; i < k; i++) {
		size = sg_calc_size(tree->root);
		val = rand() % rand_max;
		res = sg_delete(tree, val);
		assert(size - res == sg_calc_size(tree->root));
	}

	sg_delete_tree(tree);

	printf("Addio mondo\n");
	return 0;
}
