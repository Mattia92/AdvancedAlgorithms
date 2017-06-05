#include <stdio.h>	/* printf */
#include <stdlib.h> /* rand */
#include "scapegoat.h"

int main() {
	t_sg_tree* tree;
	unsigned int i, k = 1000;

	printf("Ciao mondo\n");

	tree = sg_create_tree(0.66);

	sg_insert(tree, 1);
	sg_insert(tree, 2);
	sg_insert(tree, 3);

	sg_insert(tree, 1);

	sg_search(tree, 1);

	sg_delete(tree, 1);

	// sg_delete(tree, 1);

	sg_search(tree, 1);

	for (i = 0; i < k; i++) {
		sg_insert(tree, rand() % 10);
	}

	for (i = 0; i < k; i++) {
		sg_delete(tree, rand() % 10);
	}

	printf("Addio mondo\n");
	return 0;
}
