#include <stdio.h>	/* printf */
#include <stdlib.h> /* rand */
#include <assert.h> /* assert */
#include "test_utils.h" /* scapegoat, test_utils */

#define ALPHA 0.57

#define TRUE 1
#define FALSE 0

int main(void) {
	t_sg_tree* tree;

	printf("=== TEST 1 - START ===\n");

	tree = sg_create_tree(ALPHA);

	assert(tree->size == 0);

	check_insert(tree, 2, TRUE);
	check_insert(tree, 1, TRUE);
	check_insert(tree, 6, TRUE);
	check_insert(tree, 5, TRUE);
	check_insert(tree, 15, TRUE);
	check_search(tree, 54, FALSE);
	check_insert(tree, 4, TRUE);
	check_insert(tree, 12, TRUE);
	check_insert(tree, 16, TRUE);
	check_insert(tree, 3, TRUE);
	check_delete(tree, 34, FALSE);
	check_insert(tree, 9, TRUE);
	check_search(tree, 12, TRUE);
	check_delete(tree, 2, TRUE);
	check_insert(tree, 13, TRUE);
	check_insert(tree, 17, TRUE);
	check_insert(tree, 7, TRUE);
	check_insert(tree, 11, TRUE);
	check_search(tree, 15, TRUE);
	check_insert(tree, 14, TRUE);
	check_insert(tree, 18, TRUE);
	check_insert(tree, 10, TRUE);
	check_insert(tree, 27, TRUE);
	check_insert(tree, 14, FALSE);
	check_insert(tree, 33, TRUE);
	check_insert(tree, 30, TRUE);
	check_delete(tree, 15, TRUE);
	check_search(tree, 15, FALSE);
	check_delete(tree, 17, TRUE);
	check_delete(tree, 4, TRUE);
	check_delete(tree, 33, TRUE);
	check_delete(tree, 30, TRUE);
	check_delete(tree, 27, TRUE);

	sg_delete_tree(tree);

	printf("=== TEST 1 - END ===\n");
	
	return 0;
}