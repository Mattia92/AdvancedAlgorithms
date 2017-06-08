#include <stdio.h>	/* printf */
#include <stdlib.h> /* rand */
#include <assert.h> /* assert */
#include "scapegoat.h"

#define ALPHA 0.67

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

	// check structure after a sequence of inserts
	insert(tree, 100, TRUE);
	insert(tree, 20, TRUE);
	insert(tree, 160, TRUE);
	insert(tree, 50, TRUE);
	insert(tree, 180, TRUE);
	insert(tree, 30, TRUE);
	insert(tree, 25, TRUE);
	insert(tree, 40, TRUE);
	insert(tree, 22, TRUE);
	insert(tree, 35, TRUE);
	check_struct(tree->root, 100, TRUE, 20, TRUE, 160);
	check_struct(tree->root->left, 20, FALSE, 0, TRUE, 50);
	check_struct(tree->root->left->right, 50, TRUE, 30, FALSE, 0);
	check_struct(tree->root->left->right->left, 30, TRUE, 25, TRUE, 40);
	check_struct(tree->root->left->right->left->left, 25, TRUE, 22, FALSE, 0);
	check_struct(tree->root->left->right->left->left->left, 22, FALSE, 0, FALSE, 0);
	check_struct(tree->root->left->right->left->right, 40, TRUE, 35, FALSE, 0);
	check_struct(tree->root->left->right->left->right->left, 35, FALSE, 0, FALSE, 0);
	check_struct(tree->root->right, 160, FALSE, 0, TRUE, 180);
	check_struct(tree->root->right->right, 180, FALSE, 0, FALSE, 0);

	// check rebalance on insert (scapegoat is right child)
	insert(tree, 37, TRUE);
	check_struct(tree->root->left, 20, FALSE, 0, TRUE, 35);
	check_struct(tree->root->left->right, 35, TRUE, 25, TRUE, 40);
	check_struct(tree->root->left->right->left, 25, TRUE, 22, TRUE, 30);
	check_struct(tree->root->left->right->left->left, 22, FALSE, 0, FALSE, 0);
	check_struct(tree->root->left->right->left->right, 30, FALSE, 0, FALSE, 0);
	check_struct(tree->root->left->right->right, 40, TRUE, 37, TRUE, 50);
	check_struct(tree->root->left->right->right->left, 37, FALSE, 0, FALSE, 0);
	check_struct(tree->root->left->right->right->right, 50, FALSE, 0, FALSE, 0);

	sg_delete_tree(tree);

	printf("Ending Test\n");

	return 0;
}