#include "scapegoat.h"

void check_insert(t_sg_tree* tree, int key, unsigned char expected_result);
void check_delete(t_sg_tree* tree, int key, unsigned char expected_result);
void check_search(t_sg_tree* tree, int key, unsigned char expected_result);
void check_node(t_sg_node* node, int key, unsigned char has_left, int left_key, unsigned char has_right, int right_key);
