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
		printf("Invalid alpha = %.2f, please choose an alpha in [0.5, 1), using default 0.5\n", alpha);
	}
	printf("Creating scapegoat tree with alpha = %f\n", alpha);
	sg_tree = malloc(sizeof(t_sg_tree));
	sg_tree->alpha = alpha;
	sg_tree->root = NULL;
	sg_tree->size = 0;
	sg_tree->max_size = 0;
	sg_tree->h_alpha = 0;
	return sg_tree;
}

void sg_delete_tree(t_sg_tree* sg_tree) {
	// TODO
}

// 0 if not present, 1 if present
t_sg_node* sg_search(t_sg_tree* tree, int key) {
	t_sg_node *temp = tree->root;
	printf("Searching for key = %d\n", key);
	while (temp != NULL) {
		if (temp->key == key) {
			// Found key
			printf("Key = %d found!\n", key);
			return temp;
		}

		if (key > temp->key) {
			printf("Key = %d > %d, moving to right child\n", key, temp->key);
			temp = temp->right;
		} else {
			printf("Key = %d < %d, moving to left child\n", key, temp->key);
			temp = temp->left;
		}
	}
	return NULL;
}

// Update h_alpha
void sg_update_h_alpha(t_sg_tree* sg_tree) {
	sg_tree->h_alpha = sg_tree->size == 0 ? 0 : (int)floor(log(sg_tree->size) / log(1 / sg_tree->alpha));
	printf("h_alpha updated to %d, tree size %d\n", sg_tree->h_alpha, sg_tree->size);
}

// Update sg_tree after node deletion and rebuild if needed
void sg_on_delete(t_sg_tree* sg_tree) {
	printf("Updating tree after node deletion\n");
	// Decrease tree size
    --sg_tree->size;
    printf("Size of the tree decreased to %d\n", sg_tree->size);
    // Update h_alpha
    sg_update_h_alpha(sg_tree);
    // Check if rebalance is needed		
	if (sg_tree->size < sg_tree->alpha * sg_tree->max_size) {
		printf("Rebalancing: tree size %d < %.2f\n", sg_tree->size,
			sg_tree->alpha * sg_tree->max_size);
		sg_tree->root = sg_rebuild(sg_tree->size, sg_tree->root);
		printf("New root is %d\n", sg_tree->root->key);
		sg_tree->max_size = sg_tree->size;
		printf("Tree max size = %d\n", sg_tree->max_size);
	}
}

char sg_delete(t_sg_tree* sg_tree, int key) {
	t_sg_node *parent = NULL, *to_remove = sg_tree->root, *replacement, *temp;
	char is_right_child;
	printf("Removing key = %d\n", key);

	if (to_remove == NULL) {
		printf("Tree is empty, cannot remove anything\n");
		return FALSE;
	}

	while(to_remove != NULL) {
		if (to_remove->key == key) {
			// to_remove is the node to be deleted
			printf("key = %d found\n", key);

			// Three cases
			// 1) to_remove is a leaf
			// 2) to_remove has one child
			// 3) to_remove has two children

			// Case 1: to_remove is a leaf
			if (to_remove->left == NULL && to_remove->right == NULL) {
				printf("Case 1: node to remove is a leaf\n");

				// Case 1.1: to_remove root
				if (parent == NULL) {
					assert(to_remove == sg_tree->root);
					printf("Case 1.1: node to remove is a leaf and the root\n");
					sg_tree->root = NULL;
				// Case 1.2: to_remove non root
				} else {
					printf("Case 1.2: node to remove is a leaf and not the root, its parent has key = %d\n", parent->key);
					if (is_right_child) {
						assert(parent->right->key == key);
                		parent->right = NULL;
					}
             		else {
             			assert(parent->left->key == key);
             			parent->left = NULL;
             		}
				}
             	free(to_remove);
            // Case 2: to_remove has only one child
			} else if (to_remove->left == NULL || to_remove->right == NULL) {
				// temp is the only child of to_remove
				temp = to_remove->left != NULL ? to_remove->left : to_remove->right;
				printf("Case 2: node to remove has only one child with key = %d\n", temp->key);

				assert(temp != NULL);
				// Case 2.1: to_remove is the root
				if (parent == NULL) {
					assert(to_remove == sg_tree->root);
					printf("Case 2.1: node to remove is the root, changing root to %d\n", temp->key);
					sg_tree->root = temp;
				// Case 2.2: to_remove is not the root
				} else {
					printf("Case 2.2: node to remove is not the root, changing parent %d child with %d",
						parent->key, temp->key);
					if (is_right_child) {
						assert(parent->right->key == key);
						parent->right = temp;
					} else {
						assert(parent->left->key == key);
						parent->left = temp;
					}
				}
				free(to_remove);
			// Case 3: to_remove has two children
			} else {
				printf("Case 3: node to remove has two children left %d and right %d\n"
					"Searching replacement node as the highest node less than the one to remove\n",
					to_remove->left->key,
					to_remove->right->key);

				// Note: to_remove has it's key always overwritten with the one
				// in the replacement node. to_remove is not freed in this case.

				// Find replacement i.e. the highest node less than to_remove.
				// It is the largest member in the left subtree.
				replacement = to_remove->left;
				
				// Case 3.1
				if (replacement->right == NULL) {

					printf("Case 3.1: Found replacement node %d (with no right child),\n"
						"Rep. node is the left child of to remove node\n"
						"Now replacing node to remove key and right child with the ones of the replacement node\n"
						"Left child of node to remove untouched\n"
						"Deleting replacement node\n",
						replacement->key);

					to_remove->key = replacement->key;
					to_remove->left = replacement->left;
					free(replacement);
				// Case 3.2
				} else {
					while (replacement->right->right != NULL) {
						replacement = replacement->right;
					}

					printf("Case 3.1: Found replacement node %d (with no right child) which parent is %d\n"
						"Replacing node to remove key with replacement node key\n"
						"Replacing replacement parent's right child with left child of replacement node\n"
						"Deleting replacement node\n",
						replacement->right->key, replacement->key);

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
			printf("key = %d > %d, moving to right child\n", key, to_remove->key);
			to_remove = to_remove->right;
			is_right_child = TRUE;
		} else {
			printf("key = %d > %d, moving to right child\n", key, to_remove->key);
			to_remove = to_remove->left;
			is_right_child = FALSE;
		}
	}

	printf("key = %d not found, delete failed\n", key);
	return FALSE;
}

void sg_on_insert(t_sg_tree* sg_tree, t_sg_node** stack, unsigned int stack_top) {
	t_sg_node *scapegoat;
	unsigned int sg_scape_left_size, sg_scape_right_size, sg_scape_size = 0;
	printf("Updating tree after node insertion\n");
	// Adjust tree vars
	++sg_tree->size;
	sg_tree->max_size = fmax(sg_tree->size, sg_tree->max_size);
	sg_update_h_alpha(sg_tree);
	printf("Tree: size = %d, max_size = %d, h_alpha = %d\n", sg_tree->size, sg_tree->max_size, sg_tree->h_alpha);
	// Check if the newly inserted node is deep
	if (stack_top > sg_tree->h_alpha) {
		printf("New inserted node is deep, need to find a scapegoat to rebalance the tree\n");	
		while (TRUE) {
			assert(stack_top > 0);

			// Search scapegoat
			scapegoat = stack[--stack_top];
			assert(scapegoat == stack[stack_top]);
			assert(scapegoat != NULL);

			printf("Searching at depth %d if %d is the scapegoat\n", stack_top, scapegoat->key);	

			// From now on stack[stack_top - 1] is the parent of scapegoat if stack_top >= 1

			// First iteration of the loop
			if (sg_scape_size == 0) {
				sg_scape_right_size = sg_calc_size(scapegoat->right);
				sg_scape_left_size = sg_calc_size(scapegoat->left);
				sg_scape_size = 1 + sg_scape_right_size + sg_scape_left_size;
			// Further iterations, previous sizes must be reused 
			} else if (scapegoat->right == stack[stack_top + 1]) {
				// Previous node tested is the right child of current one
				sg_scape_right_size = sg_scape_size;
				sg_scape_left_size = sg_calc_size(scapegoat->left);
			} else {
				// Previous node tested is the left child of current one
				sg_scape_right_size = sg_calc_size(scapegoat->right);
				sg_scape_left_size = sg_scape_size;
			}

			sg_scape_size = 1 + sg_scape_right_size + sg_scape_left_size;

			if (sg_scape_left_size <= sg_tree->alpha * sg_scape_size && sg_scape_right_size <= sg_tree->alpha * sg_scape_size) {
				// Node is alpha-weight-balanced
				printf("Node %d is alpha-weight-balanced\n", scapegoat->key);
			} else {
				// Scapegoat found
				printf("Scapegoat found! Node %d is not alpha-weight-balanced, rebalancing around it\n", scapegoat->key);

				// Scapegoat is root
				if (stack_top == 0) {
					sg_tree->root = sg_rebuild(sg_scape_size, scapegoat);
				// Scapegoat is not root
				} else if (scapegoat == stack[stack_top - 1]->right) {
					// Scapegoat is the right child of its parent
					stack[stack_top - 1]->right = sg_rebuild(sg_scape_size, scapegoat);
				} else {
					// Scapegoat is the left child of its parent
					stack[stack_top - 1]->left = sg_rebuild(sg_scape_size, scapegoat);
				}
			
				free(stack);
				return;
			}
		}
	}
}

char sg_insert(t_sg_tree* sg_tree, int key) {
	// Stack to save the nodes traversed to reach insertion point
	// insertion point max depth = h_alpha + 1
	t_sg_node **stack, *parent = sg_tree->root;
	unsigned int stack_top;

	printf("Inserting key = %d\n", key);

	if (parent == NULL) {
		printf("Tree empty, adding %d as root\n", key);

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
			printf("Key %d already present, insert failed\n", key);

			free(stack);
			return FALSE;
		}

		if (key > parent->key) {
			printf("Key %d > %d\n", key, parent->key);
			if (parent->right == NULL) {
				// Insert right
				printf("Adding %d as right child of it's parent\n", key);

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
			printf("Key %d < %d\n", key, parent->key);
			if (parent->left == NULL) {
				// Insert left
				printf("Adding %d as left child of it's parent\n", key);

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
