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

int main(void) {

	t_sg_tree* tree;
	unsigned int size;

	printf("Staring Test\n");

	tree = sg_create_tree(ALPHA);

	assert(tree->size == 0);

	insert(tree, 2, TRUE);
	insert(tree, 1, TRUE);
	insert(tree, 6, TRUE);
	insert(tree, 5, TRUE);
	insert(tree, 15, TRUE);
	search(tree, 54, FALSE);
	insert(tree, 4, TRUE);
	insert(tree, 12, TRUE);
	insert(tree, 16, TRUE);
	insert(tree, 3, TRUE);
	delete(tree, 34, FALSE);
	insert(tree, 9, TRUE);
	search(tree, 12, TRUE);
	delete(tree, 2, TRUE);
	insert(tree, 13, TRUE);
	insert(tree, 17, TRUE);
	insert(tree, 7, TRUE);
	insert(tree, 11, TRUE);
	search(tree, 15, TRUE);
	insert(tree, 14, TRUE);
	insert(tree, 18, TRUE);
	insert(tree, 10, TRUE);
	insert(tree, 27, TRUE);
	insert(tree, 14, FALSE);
	insert(tree, 33, TRUE);
	insert(tree, 30, TRUE);
	delete(tree, 15, TRUE);
	search(tree, 15, FALSE);
	delete(tree, 17, TRUE);
	delete(tree, 4, TRUE);
	delete(tree, 33, TRUE);
	delete(tree, 30, TRUE);
	delete(tree, 27, TRUE);


	sg_delete_tree(tree);

	printf("Ending Test\n");

	return 0;
}