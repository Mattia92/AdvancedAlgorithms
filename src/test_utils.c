#include <assert.h>
#include "test_utils.h"

void check_insert(t_sg_tree* tree, int key, unsigned char expected_result) {
	unsigned int size_pre_insert;
	unsigned char result;

	size_pre_insert = sg_calc_size(tree->root);
	result = sg_insert(tree, key);
	assert(size_pre_insert + result == sg_calc_size(tree->root));
	assert(expected_result == result);
}

void check_delete(t_sg_tree* tree, int key, unsigned char expected_result) {
	unsigned int size_pre_delete;
	unsigned char result;

	size_pre_delete = sg_calc_size(tree->root);
	result = sg_delete(tree, key);
	assert(size_pre_delete - result == sg_calc_size(tree->root));
	assert(expected_result == result);
}

void check_search(t_sg_tree* tree, int key, unsigned char expected_result) {
	unsigned int size_pre_search;
	t_sg_node* result;

	size_pre_search = sg_calc_size(tree->root);
	result = sg_search(tree, key);
	assert(size_pre_search == sg_calc_size(tree->root));
	assert(expected_result == (result != NULL));
}

void check_node(t_sg_node* node, int key, unsigned char has_left, int left_key, unsigned char has_right, int right_key) {
	assert(node != NULL);
	assert(node->key == key);
	assert(has_left == (node->left != NULL));
	assert(has_right == (node->right != NULL));
	if (has_left) {
		assert(left_key == node->left->key);
	}
	if (has_right) {
		assert(right_key == node->right->key);
	}
}
