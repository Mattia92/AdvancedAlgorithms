#include <stdio.h>	/* printf */
#include <stdlib.h> /* rand */
#include <assert.h> /* assert */
#include "test_utils.h" /* scapegoat, test_utils */

#define ALPHA 0.57

#define TRUE 1
#define FALSE 0

int main(void) {
	t_sg_tree* tree;

	printf("=== TEST 2 - START ===\n");

	tree = sg_create_tree(ALPHA);

	assert(tree->size == 0);

	// check insert and delete of root
	check_insert(tree, 1, TRUE);
	check_node(tree->root, 1, FALSE, 0, FALSE, 0);
	check_delete(tree, 1, TRUE);
	assert(tree->root == NULL && tree->size == 0);

	// check insert of root and two children
	// delete of root (left child become new root)
	// check new root with one right child
	// delete of root (right child become new root) and delete of new root
	check_insert(tree, 2, TRUE);
	check_node(tree->root, 2, FALSE, 0, FALSE, 0);
	check_insert(tree, 1, TRUE);
	check_insert(tree, 3, TRUE);
	check_node(tree->root, 2, TRUE, 1, TRUE, 3);
	check_node(tree->root->left, 1, FALSE, 0, FALSE, 0);
	check_node(tree->root->right, 3, FALSE, 0, FALSE, 0);
	check_delete(tree, 2, TRUE);
	check_node(tree->root, 1, FALSE, 0, TRUE, 3);
	check_delete(tree, 1, TRUE);
	check_node(tree->root, 3, FALSE, 0, FALSE, 0);
	check_delete(tree, 3, TRUE);
	assert(tree->root == NULL && tree->size == 0);

	// // check rebalance on insert (scapegoat is root)
	check_insert(tree, 1, TRUE);
	check_insert(tree, 2, TRUE);
	check_node(tree->root, 1, FALSE, 0, TRUE, 2);
	check_node(tree->root->right, 2, FALSE, 0, FALSE, 0);
	check_insert(tree, 3, TRUE);
	check_node(tree->root, 2, TRUE, 1, TRUE, 3);
	check_node(tree->root->left, 1, FALSE, 0, FALSE, 0);
	check_node(tree->root->right, 3, FALSE, 0, FALSE, 0);
	check_delete(tree, 3, TRUE);
	check_delete(tree, 1, TRUE);
	check_delete(tree, 2, TRUE);
	assert(tree->root == NULL && tree->size == 0);

	// check structure after a sequence of inserts
	check_insert(tree, 100, TRUE);
	check_insert(tree, 57, TRUE);
	check_insert(tree, 238, TRUE);
	check_insert(tree, 34, TRUE);
	check_insert(tree, 147, TRUE);
	check_insert(tree, 82, TRUE);
	check_insert(tree, 526, TRUE);

	// check insertion of an already existing node
	check_insert(tree, 147, FALSE);
	check_node(tree->root, 100, TRUE, 57, TRUE, 238);
	check_node(tree->root->left, 57, TRUE, 34, TRUE, 82);
	check_node(tree->root->left->left, 34, FALSE, 0, FALSE, 0);
	check_node(tree->root->left->right, 82, FALSE, 0, FALSE, 0);
	check_node(tree->root->right, 238, TRUE, 147, TRUE, 526);
	check_node(tree->root->right->left, 147, FALSE, 0, FALSE, 0);
	check_node(tree->root->right->right, 526, FALSE, 0, FALSE, 0);

	// check an existing and a not existing node
	// check search doesn't change tree structure
	check_search(tree, 82, TRUE);
	check_search(tree, 1, FALSE);
	check_node(tree->root->left->right, 82, FALSE, 0, FALSE, 0);

	// check delete of a not existing node and a non-root leaf
	check_delete(tree, 24, FALSE);
	check_delete(tree, 526, TRUE);
	check_node(tree->root->right, 238, TRUE, 147, FALSE, 0);
	check_insert(tree, 345, TRUE);
	check_node(tree->root->right, 238, TRUE, 147, TRUE, 345);
	check_node(tree->root->right->right, 345, FALSE, 0, FALSE, 0);

	// check delete of non-root node with exactly one child
	check_insert(tree, 63, TRUE);
	check_node(tree->root->left->right->left, 63, FALSE, 0, FALSE, 0);
	check_delete(tree, 82, TRUE);
	check_node(tree->root->left, 57, TRUE, 34, TRUE, 63);
	check_node(tree->root->left->right, 63, FALSE, 0, FALSE, 0);

	// check rebalance on insert (scapegoat is left child)
	check_insert(tree, 123, TRUE);
	check_insert(tree, 119, TRUE);
	check_node(tree->root->right, 238, TRUE, 123, TRUE, 345);
	check_node(tree->root->right->left, 123, TRUE, 119, TRUE, 147);
	check_node(tree->root->right->left->left, 119, FALSE, 0, FALSE, 0);
	check_node(tree->root->right->left->right, 147, FALSE, 0, FALSE, 0);

	sg_delete_tree(tree);

	printf("=== TEST 2 - END ===\n");

	return 0;
}
