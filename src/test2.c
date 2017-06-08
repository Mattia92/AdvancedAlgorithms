#include <stdio.h>	/* printf */
#include <stdlib.h> /* rand */
#include <assert.h> /* assert */
#include "scapegoat.h"

#define ALPHA 0.57

#define TRUE 1
#define FALSE 0

void insert (t_sg_tree* tree, int val, char res) {
	unsigned int size_ins;
	char res_ins;

	size_ins = sg_calc_size(tree->root);
	res_ins = sg_insert(tree, val);
	assert(size_ins + res_ins == sg_calc_size(tree->root));
	assert(res == res_ins);
}

void delete (t_sg_tree* tree, int val, char res) {
	unsigned int size_del;
	char res_del;

	size_del = sg_calc_size(tree->root);
	res_del = sg_delete(tree, val);
	assert(size_del - res_del == sg_calc_size(tree->root));
	assert(res == res_del);
}

void search (t_sg_tree* tree, int val, char res) {
	unsigned int size_src;
	t_sg_node* res_src;

	size_src = sg_calc_size(tree->root);
	res_src = sg_search(tree, val);
	assert(size_src == sg_calc_size(tree->root));
	assert(res == (res_src != NULL));
}

void check_struct (t_sg_node* node, int keyc, char left, int left_key, char right, int right_key) {
	assert(node != NULL);
	assert(node->key == keyc);
	assert(left == (node->left != NULL));
	assert(right == (node->right != NULL));
	if(left){
		assert(left_key == node->left->key);
	}
	if(right){
		assert(right_key == node->right->key);
	}
}

int main(void) {

	t_sg_tree* tree;
	unsigned int size;

	printf("Staring Test\n");

	tree = sg_create_tree(ALPHA);

	assert(tree->size == 0);

	// check insert and delete of root
	insert(tree, 1, TRUE);
	check_struct(tree->root, 1, FALSE, 0, FALSE, 0);
	delete(tree, 1, TRUE);
	assert(tree->root == NULL && tree->size == 0);

	// check insert of root and two children
	// delete of root (left child become new root)
	// check new root with one right child
	// delete of root (right child become new root) and delete of new root
	insert(tree, 2, TRUE);
	check_struct(tree->root, 2, FALSE, 0, FALSE, 0);
	insert(tree, 1, TRUE);
	insert(tree, 3, TRUE);
	check_struct(tree->root, 2, TRUE, 1, TRUE, 3);
	check_struct(tree->root->left, 1, FALSE, 0, FALSE, 0);
	check_struct(tree->root->right, 3, FALSE, 0, FALSE, 0);
	delete(tree, 2, TRUE);
	check_struct(tree->root, 1, FALSE, 0, TRUE, 3);
	delete(tree, 1, TRUE);
	check_struct(tree->root, 3, FALSE, 0, FALSE, 0);
	delete(tree, 3, TRUE);
	assert(tree->root == NULL && tree->size == 0);

	// // check rebalance on insert (scapegoat is root)
	insert(tree, 1, TRUE);
	insert(tree, 2, TRUE);
	check_struct(tree->root, 1, FALSE, 0, TRUE, 2);
	check_struct(tree->root->right, 2, FALSE, 0, FALSE, 0);
	insert(tree, 3, TRUE);
	check_struct(tree->root, 2, TRUE, 1, TRUE, 3);
	check_struct(tree->root->left, 1, FALSE, 0, FALSE, 0);
	check_struct(tree->root->right, 3, FALSE, 0, FALSE, 0);
	delete(tree, 3, TRUE);
	delete(tree, 1, TRUE);
	delete(tree, 2, TRUE);
	assert(tree->root == NULL && tree->size == 0);

	// check structure after a sequence of inserts
	insert(tree, 100, TRUE);
	insert(tree, 57, TRUE);
	insert(tree, 238, TRUE);
	insert(tree, 34, TRUE);
	insert(tree, 147, TRUE);
	insert(tree, 82, TRUE);
	insert(tree, 526, TRUE);
	// check insertion of an already existing node
	insert(tree, 147, FALSE);
	check_struct(tree->root, 100, TRUE, 57, TRUE, 238);
	check_struct(tree->root->left, 57, TRUE, 34, TRUE, 82);
	check_struct(tree->root->left->left, 34, FALSE, 0, FALSE, 0);
	check_struct(tree->root->left->right, 82, FALSE, 0, FALSE, 0);
	check_struct(tree->root->right, 238, TRUE, 147, TRUE, 526);
	check_struct(tree->root->right->left, 147, FALSE, 0, FALSE, 0);
	check_struct(tree->root->right->right, 526, FALSE, 0, FALSE, 0);

	// check an existing and a not existing node
	// check search doesn't change tree structure
	search(tree, 82, TRUE);
	search(tree, 1, FALSE);
	check_struct(tree->root->left->right, 82, FALSE, 0, FALSE, 0);

	// check delete of a not existing node and a non-root leaf
	delete(tree, 24, FALSE);
	delete(tree, 526, TRUE);
	check_struct(tree->root->right, 238, TRUE, 147, FALSE, 0);

	insert(tree, 345, TRUE);
	check_struct(tree->root->right, 238, TRUE, 147, TRUE, 345);
	check_struct(tree->root->right->right, 345, FALSE, 0, FALSE, 0);

	// check delete of non-root node with exactly one child
	insert(tree, 63, TRUE);
	check_struct(tree->root->left->right->left, 63, FALSE, 0, FALSE, 0);
	delete(tree, 82, TRUE);
	check_struct(tree->root->left, 57, TRUE, 34, TRUE, 63);
	check_struct(tree->root->left->right, 63, FALSE, 0, FALSE, 0);

	// check rebalance on insert (scapegoat is left child)
	insert(tree, 123, TRUE);
	insert(tree, 119, TRUE);
	check_struct(tree->root->right, 238, TRUE, 123, TRUE, 345);
	check_struct(tree->root->right->left, 123, TRUE, 119, TRUE, 147);
	check_struct(tree->root->right->left->left, 119, FALSE, 0, FALSE, 0);
	check_struct(tree->root->right->left->right, 147, FALSE, 0, FALSE, 0);

	sg_delete_tree(tree);

	printf("Ending Test\n");

	return 0;
}