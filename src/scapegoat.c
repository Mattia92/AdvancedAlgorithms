#include <stdio.h>	/* printf */
#include <math.h>	/* log, floor, ceil, fmax */
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include "scapegoat.h"

#define TRUE 1
#define FALSE 0

t_sg_tree* sg_create_tree(double alpha) {
	t_sg_tree* sg_tree;
	if (alpha < 0.5 || alpha >= 1) {
		printf("Invalid alpha\n");
	}
	sg_tree = malloc(sizeof(t_sg_tree));
	sg_tree->alpha = alpha;
	return sg_tree;
}

void sg_delete_tree(t_sg_tree* sg_tree) {
	// TODO
}

// 0 if not present, 1 if present
char sg_search(t_sg_tree* tree, int key) {
	t_sg_node *temp = tree->root;
	while (temp != NULL) {
		if (temp->key == key) {
			// Found key
			return TRUE;
		}

		if (key > temp->key) {
			temp = temp->right;
		} else {
			temp = temp->left;
		}
	}
	return FALSE;
}

// Update h_alpha
void sg_update_h_alpha(t_sg_tree* sg_tree) {
	sg_tree->h_alpha = floor(log(sg_tree->size) / log(1 / sg_tree->alpha));
}

// Update sg_tree after node deletion and rebuild if needed
void sg_on_delete(t_sg_tree* sg_tree) {
	// Decrease tree size
    --sg_tree->size;
    // Update h_alpha
    sg_update_h_alpha(sg_tree);
    // Check if rebalance is needed		
	if (sg_tree->size < sg_tree->alpha * sg_tree->max_size) {
		sg_tree->root = sg_rebuild(sg_tree->size, sg_tree->root);
		sg_tree->max_size = sg_tree->size;
	}
}

char sg_delete(t_sg_tree* sg_tree, int key) {
	t_sg_node *parent = NULL, *to_remove = sg_tree->root, *replacement, *temp;
	char is_right_child;

	if (to_remove == NULL) {
		return FALSE;
	}

	while(to_remove != NULL) {
		if (to_remove->key == key) {
			// to_remove is the node to be deleted

			// Three cases
			// 1) to_remove is a leaf
			// 2) to_remove has one child
			// 3) to_remove has two children

			// Case 1: to_remove is a leaf
			if (to_remove->left == NULL && to_remove->right == NULL) {
				// Case 1.1: to_remove root
				if (parent == NULL) {
					assert(to_remove == sg_tree->root);
					sg_tree->root = NULL;
				// Case 1.2: to_remove non root
				} else {
					if (is_right_child) {
                		parent->right = NULL;
					}
             		else {
             			parent->left = NULL;
             		}
				}
             	free(to_remove);
            // Case 2: to_remove has only one child
			} else if (to_remove->left == NULL || to_remove->right == NULL) {
				// temp is the only child of to_remove
				temp = to_remove->left != NULL ? to_remove->left : to_remove->right;
				assert(temp != NULL);
				// Case 2.1: to_remove is the root
				if (parent == NULL) {
					assert(to_remove == sg_tree->root);
					sg_tree->root = temp;
				// Case 2.2: to_remove is not the root
				} else {
					if (is_right_child) {
						parent->right = temp;
					} else {
						parent->left = temp;
					}
				}
				free(to_remove);
			// Case 3: to_remove has two children
			} else {
				// Note: to_remove has it's key always overwritten with the one
				// in the replacement node. to_remove is not freed in this case.

				// Find replacement i.e. the highests node less than to_remove.
				// It is the largest member in the left subtree.
				replacement = to_remove->left;

				// Case 3.1
				if (replacement->right == NULL) {
					to_remove->key = replacement->key;
					to_remove->left = replacement->left;
					free(replacement);
				// Case 3.2
				} else {
					while (replacement->right->right != NULL) {
						replacement = replacement->right;
					}
					to_remove->key = replacement->right->key;
					temp = replacement->right;
					replacement->right = replacement->right->left;
					free(temp);
				}				
			}

			// Scapegoat logic
			sg_on_delete(sg_tree);
			return TRUE;
		}

		parent = to_remove;
		if (key > to_remove->key) {
			to_remove = to_remove->right;
			is_right_child = TRUE;
		} else {
			to_remove = to_remove->left;
			is_right_child = FALSE;
		}
	}

	return FALSE;
}

void sg_on_insert(t_sg_tree* sg_tree, t_sg_node** stack, unsigned int stack_top) {
	t_sg_node *scapegoat;
	unsigned int sg_scape_left_size, sg_scape_right_size, sg_scape_size = 0;
	// Adjust tree vars
	++sg_tree->size;
	sg_tree->max_size = fmax(sg_tree->size, sg_tree->max_size);
	sg_update_h_alpha(sg_tree);
	// Check if the newly inserted node is deep
	if (stack_top > sg_tree->h_alpha) {
		while (TRUE) {
			// Search scapegoat
			scapegoat = stack[--stack_top];

			assert(scapegoat == stack[stack_top]);

			// From now on stack[stack_top - 1] is the parent of scapegoat

			// First iteration of the loop
			if (sg_scape_size == 0) {
				sg_scape_right_size = sg_calc_size(scapegoat->right);
				sg_scape_left_size = sg_calc_size(scapegoat->left);
				sg_scape_size = 1 + sg_scape_right_size + sg_scape_left_size;
			// Further iterations, previous sizes must be reused 
			} else if (scapegoat->right == stack[stack_top + 1]) {
				sg_scape_right_size = sg_scape_size;
				sg_scape_left_size = sg_calc_size(scapegoat->left);
			} else {
				sg_scape_right_size = sg_calc_size(scapegoat->right);
				sg_scape_left_size = sg_scape_size;
			}

			sg_scape_size = 1 + sg_scape_right_size + sg_scape_left_size;

			if (sg_scape_left_size <= sg_tree->alpha * sg_scape_size && sg_scape_right_size <= sg_tree->alpha * sg_scape_size) {
				// Node is alpha-weight-balanced
			} else {
				// Scapegoat found

				// Scapegoat is root
				if (stack_top == 0) {
					sg_tree->root = sg_rebuild(sg_scape_size, scapegoat);
				// Scapegoat is not root
				} else if (scapegoat == stack[stack_top - 1]->right) {
					stack[stack_top - 1]->right = sg_rebuild(sg_scape_size, scapegoat);
				} else {
					stack[stack_top - 1]->left = sg_rebuild(sg_scape_size, scapegoat);
				}
			
				free(stack);
				return;
			}

			assert(stack_top >= 0);
		}
	}
}

char sg_insert(t_sg_tree* sg_tree, int key) {
	// Stack to save the nodes traversed to reach insertion point
	// insertion point max depth = h_alpha + 1
	t_sg_node **stack;
	t_sg_node *parent = sg_tree->root;
	unsigned int stack_top;

	if (parent == NULL) {
		parent = malloc(sizeof(t_sg_node));
		parent->key = key;
		parent->left = NULL;
		parent->right = NULL;
		// Insert successfull
		// Root is never DEEP -> No rebalance required
		return TRUE;
	}

	stack = malloc(sizeof(t_sg_node*) * (sg_tree->h_alpha + 1));
	// When inserting a node: stack_top == depth
	stack_top = 0;
	stack[stack_top++] = parent;

	while(TRUE) {
		if (key == parent->key) {
			// Found key, don't overwrite
			free(stack);
			return FALSE;
		}

		if (key > parent->key) {
			if (parent->right == NULL) {
				// Insert right
				parent->right = malloc(sizeof(t_sg_node));
				parent->right->key = key;
				parent->right->right = NULL;
				parent->right->left = NULL;
				sg_on_insert(sg_tree, stack, stack_top);
				free(stack);
				return TRUE;
			}
			parent = parent->right;
		} else {
			if (parent->left == NULL) {
				// Insert left
				parent->left = malloc(sizeof(t_sg_node));
				parent->left->key = key;
				parent->left->right = NULL;
				parent->left->left = NULL;
				sg_on_insert(sg_tree, stack, stack_top);
				free(stack);
				return TRUE;
			}
			parent = parent->left;
		}

		// stack size = sg_tree->h_alpha + 1,
		// last valid stack_top = sg_tree->h_alpha
		// This assert avoids infinite loops while debugging
		assert(stack_top < sg_tree->h_alpha + 1);
		stack[stack_top++] = parent;
	}
}

// TODO Implement iterative version (using dynamic stack with realloc)
unsigned int sg_calc_size(t_sg_node* sg_node) {
	if (sg_node->left == NULL && sg_node->right == NULL) {
		return 1;
	}
	return sg_calc_size(sg_node->left) + 1 + sg_calc_size(sg_node->right);
}

t_sg_node* sg_rebuild(unsigned int sg_scape_size, t_sg_node* sg_scape_node) {
	// sg_dummy_node is a concrete node allocated on the stack
	t_sg_node sg_dummy_node;
	sg_build_tree(sg_scape_size, sg_flatten(sg_scape_node, &sg_dummy_node));
	return sg_dummy_node.left;
}

t_sg_node* sg_flatten(t_sg_node* sg_node_x, t_sg_node* sg_node_y) {
	if (sg_node_x == NULL) {
		return sg_node_y;
	}
	sg_node_x->right = sg_flatten(sg_node_x->right, sg_node_y);
	return sg_flatten(sg_node_x->left, sg_node_x);
}

t_sg_node* sg_build_tree(unsigned int sg_size, t_sg_node* sg_node_x) {
	t_sg_node *sg_node_r, *sg_node_s;
	if (sg_size == 0) {
		sg_node_x->left = NULL;
		return sg_node_x;
	}

	sg_node_r = sg_build_tree(ceil((sg_size - 1) / 2), sg_node_x);
	sg_node_s = sg_build_tree(floor((sg_size - 1) / 2), sg_node_r->right);

	sg_node_r->right = sg_node_s->left;
	sg_node_s->left = sg_node_r;

	return sg_node_s;
}
