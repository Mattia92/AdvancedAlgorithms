#include <stdio.h>	/* printf */
#include <stdlib.h> /* rand */
#include <assert.h> /* assert */
#include "test_utils.h" /* scapegoat, test_utils */

#define ALPHA 0.67

#define TRUE 1
#define FALSE 0

int main(void) {
	t_sg_tree* tree;

	printf("=== TEST 4 - START ===\n");

	tree = sg_create_tree(ALPHA);

	assert(tree->size == 0);

	// check structure after a sequence of inserts
	check_insert(tree, 100, TRUE);
	check_insert(tree, 20, TRUE);
	check_insert(tree, 160, TRUE);
	check_insert(tree, 50, TRUE);
	check_insert(tree, 180, TRUE);
	check_insert(tree, 30, TRUE);
	check_insert(tree, 25, TRUE);
	check_insert(tree, 40, TRUE);
	check_insert(tree, 22, TRUE);
	check_insert(tree, 35, TRUE);
	check_insert(tree, 15, TRUE);
	check_insert(tree, 37, TRUE);
	check_node(tree->root, 100, TRUE, 20, TRUE, 160);
	check_node(tree->root->left, 20, TRUE, 15, TRUE, 50);
	check_node(tree->root->left->left, 15, FALSE, 0, FALSE, 0);
	check_node(tree->root->left->right, 50, TRUE, 30, FALSE, 0);
	check_node(tree->root->left->right->left, 30, TRUE, 25, TRUE, 40);
	check_node(tree->root->left->right->left->left, 25, TRUE, 22, FALSE, 0);
	check_node(tree->root->left->right->left->left->left, 22, FALSE, 0, FALSE, 0);
	check_node(tree->root->left->right->left->right, 40, TRUE, 35, FALSE, 0);
	check_node(tree->root->left->right->left->right->left, 35, FALSE, 0, TRUE, 37);
	check_node(tree->root->left->right->left->right->left->right, 37, FALSE, 0, FALSE, 0);
	check_node(tree->root->right, 160, FALSE, 0, TRUE, 180);
	check_node(tree->root->right->right, 180, FALSE, 0, FALSE, 0);

	// check structure after delete
	check_delete(tree, 30, TRUE);
	check_delete(tree, 15, TRUE);
	check_delete(tree, 20, TRUE);
	check_node(tree->root, 100, TRUE, 50, TRUE, 160);
	check_node(tree->root->left, 50, TRUE, 25, FALSE, 0);
	check_node(tree->root->left->left, 25, TRUE, 22, TRUE, 40);
	check_node(tree->root->left->left->left, 22, FALSE, 0, FALSE, 0);
	check_node(tree->root->left->left->right, 40, TRUE, 35, FALSE, 0);
	check_node(tree->root->left->left->right->left, 35, FALSE, 0, TRUE, 37);
	check_node(tree->root->left->left->right->left->right, 37, FALSE, 0, FALSE, 0);
	check_node(tree->root->right, 160, FALSE, 0, TRUE, 180);
	check_node(tree->root->right->right, 180, FALSE, 0, FALSE, 0);

	// check rebalance on delete
	check_delete(tree, 180, TRUE);
	check_node(tree->root, 40, TRUE, 35, TRUE, 100);
	check_node(tree->root->left, 35, TRUE, 25, TRUE, 37);
	check_node(tree->root->left->left, 25, TRUE, 22, FALSE, 0);
	check_node(tree->root->left->left->left, 22, FALSE, 0, FALSE, 0);
	check_node(tree->root->left->right, 37, FALSE, 0, FALSE, 0);
	check_node(tree->root->right, 100, TRUE, 50, TRUE, 160);
	check_node(tree->root->right->left, 50, FALSE, 0, FALSE, 0);
	check_node(tree->root->right->right, 160, FALSE, 0, FALSE, 0);

	sg_delete_tree(tree);

	printf("=== TEST 4 - END ===\n");

	return 0;
}
