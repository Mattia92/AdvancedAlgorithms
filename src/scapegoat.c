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
		printf("CREATE - Invalid alpha = %.2f, please choose an alpha in [0.5, 1), using default 0.5\n", alpha);
	}
	printf("CREATE - Creating scapegoat tree with alpha = %f\n", alpha);
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

// NULL if not present, pointer to node if present
t_sg_node* sg_search(t_sg_tree* tree, int key) {
	t_sg_node *temp = tree->root;

	#ifdef DEBUG
	printf("SEARCH - key = %d\n", key);
	#endif

	while (temp != NULL) {
		if (temp->key == key) {
			// Found key

			#ifdef DEBUG
			printf("SEARCH - Key = %d found!\n", key);
			#endif

			return temp;
		}

		if (key > temp->key) {

			#ifdef DEBUG
			printf("SEARCH - Key = %d > %d, moving to right child\n", key, temp->key);
			#endif

			temp = temp->right;
		} else {

			#ifdef DEBUG
			printf("SEARCH - Key = %d < %d, moving to left child\n", key, temp->key);
			#endif

			temp = temp->left;
		}
	}

	#ifdef DEBUG
	printf("SEARCH - Key = %d not found, search failed\n", key);
	#endif

	return NULL;
}

// Update h_alpha
void sg_update_h_alpha(t_sg_tree* sg_tree) {
	sg_tree->h_alpha = sg_tree->size == 0 ? 0 : (int)floor(log(sg_tree->size) / log(1 / sg_tree->alpha));
}

// Update sg_tree after node deletion and rebuild if needed
void sg_on_delete(t_sg_tree* sg_tree) {
	#ifdef DEBUG
	printf("ON DELETE - Updating tree after node deletion\n");
	#endif
	// Decrease tree size
    --sg_tree->size;
    
    // Update h_alpha
    sg_update_h_alpha(sg_tree);

    #ifdef DEBUG
    printf("ON DELETE - size = %d, h_alpha = %d\n", sg_tree->size, sg_tree->h_alpha);
    #endif

    if (sg_tree->size < 3) {
    	return;
    }

    // Check if rebalance is needed		
	if (sg_tree->size < sg_tree->alpha * sg_tree->max_size) {
		sg_tree->root = sg_rebuild(sg_tree->size, sg_tree->root);
		sg_tree->max_size = sg_tree->size;
		// printf("%d %d %d\n", sg_tree->root, sg_tree->root->left, sg_tree->root->right);
		printf("%d \n", sg_tree->root->left);
		#ifdef DEBUG
		printf("ON DELETE - Tree rebalanced: new root is %d\n", sg_tree->root->key);
		#endif
	}
}

char sg_delete(t_sg_tree* sg_tree, int key) {
	t_sg_node *parent = NULL, *to_remove = sg_tree->root, *replacement, *temp;
	char is_right_child;
	#ifdef DEBUG
	printf("DELETE - Removing key = %d\n", key);
	#endif

	if (to_remove == NULL) {
		#ifdef DEBUG
		printf("DELETE - Tree is empty, cannot remove anything\n");
		#endif
		return FALSE;
	}

	while(to_remove != NULL) {
		if (to_remove->key == key) {
			// to_remove is the node to be deleted
			#ifdef DEBUG
			printf("DELETE - key = %d found\n", key);
			#endif

			// Three cases
			// 1) to_remove is a leaf
			// 2) to_remove has one child
			// 3) to_remove has two children

			// Case 1: to_remove is a leaf
			if (to_remove->left == NULL && to_remove->right == NULL) {
				#ifdef DEBUG
				printf("DELETE - Case 1: node to remove is a leaf\n");
				#endif

				// Case 1.1: to_remove root
				if (parent == NULL) {
					assert(to_remove == sg_tree->root);
					#ifdef DEBUG
					printf("DELETE - Case 1.1: node to remove is a leaf and the root\n");
					#endif
					sg_tree->root = NULL;
				// Case 1.2: to_remove non root
				} else {
					assert(to_remove != sg_tree->root);
					#ifdef DEBUG
					printf("DELETE - Case 1.2: node to remove is a leaf and not the root, its parent has key = %d\n", parent->key);
					#endif
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
				#ifdef DEBUG
				printf("DELETE - Case 2: node to remove has only one child with key = %d\n", temp->key);
				#endif

				assert(temp != NULL);
				// Case 2.1: to_remove is the root
				if (parent == NULL) {
					assert(to_remove == sg_tree->root);
					#ifdef DEBUG
					printf("DELETE - Case 2.1: node to remove is the root, changing root to %d\n", temp->key);
					#endif
					sg_tree->root = temp;
				// Case 2.2: to_remove is not the root
				} else {
					assert(to_remove != sg_tree->root);
					#ifdef DEBUG
					printf("DELETE - Case 2.2: node to remove is not the root, changing parent %d child with %d\n",
						parent->key, temp->key);
					#endif
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
				#ifdef DEBUG
				printf("DELETE - Case 3: node to remove has two children left %d and right %d\n"
					"\tSearching replacement node as the highest node less than the one to remove\n",
					to_remove->left->key,
					to_remove->right->key);
				#endif

				// Note: to_remove has it's key always overwritten with the one
				// in the replacement node. to_remove is not freed in this case.

				// Find replacement i.e. the highest node less than to_remove.
				// It is the largest member in the left subtree.
				replacement = to_remove->left;
				
				// Case 3.1
				if (replacement->right == NULL) {
					#ifdef DEBUG
					printf("DELETE - Case 3.1: Found replacement node %d (with no right child),\n"
						"\tRep. node is the left child of to remove node\n"
						"\tNow replacing node to remove key and right child with the ones of the replacement node\n"
						"\tLeft child of node to remove untouched\n"
						"\tDeleting replacement node\n",
						replacement->key);
					#endif

					to_remove->key = replacement->key;
					to_remove->left = replacement->left;
					free(replacement);
				// Case 3.2
				} else {
					while (replacement->right->right != NULL) {
						replacement = replacement->right;
					}

					#ifdef DEBUG
					printf("DELETE - Case 3.1: Found replacement node %d (with no right child) which parent is %d\n"
						"\tReplacing node to remove key with replacement node key\n"
						"\tReplacing replacement parent's right child with left child of replacement node\n"
						"\tDeleting replacement node\n",
						replacement->right->key, replacement->key);
					#endif

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
			#ifdef DEBUG
			printf("DELETE - key = %d > %d, moving to right child\n", key, to_remove->key);
			#endif
			to_remove = to_remove->right;
			is_right_child = TRUE;
		} else {
			#ifdef DEBUG
			printf("DELETE - key = %d < %d, moving to left child\n", key, to_remove->key);
			#endif
			to_remove = to_remove->left;
			is_right_child = FALSE;
		}
	}

	#ifdef DEBUG
	printf("DELETE - key = %d not found, delete failed\n", key);
	#endif
	return FALSE;
}

void sg_on_insert(t_sg_tree* sg_tree, t_sg_node** stack, unsigned int stack_top) {
	// stack_top equals the depth of the newly inserted node
	t_sg_node *scapegoat;
	unsigned int sg_scape_left_size, sg_scape_right_size, sg_scape_size = 0;
	assert((stack_top == 0 && stack == NULL) || (stack_top > 0 && stack != NULL));
	#ifdef DEBUG
	printf("ON INSERT - Updating tree after node insertion at depth %d\n", stack_top);
	#endif
	// Adjust tree vars
	++sg_tree->size;
	sg_tree->max_size = fmax(sg_tree->size, sg_tree->max_size);
	sg_update_h_alpha(sg_tree);
	#ifdef DEBUG
	printf("ON INSERT - Tree: size = %d, max_size = %d, h_alpha = %d\n", sg_tree->size, sg_tree->max_size, sg_tree->h_alpha);
	#endif
	if (stack_top == 0) {
		return;
	}
	// Check if the newly inserted node is deep
	if (stack_top > sg_tree->h_alpha) {
		#ifdef DEBUG
		printf("ON INSERT - New inserted node is deep, need to find a scapegoat to rebalance the tree\n");	
		#endif
		while (TRUE) {
			assert(stack_top > 0);

			// Search scapegoat
			scapegoat = stack[--stack_top];
			assert(scapegoat == stack[stack_top]);
			assert(scapegoat != NULL);

			#ifdef DEBUG
			printf("ON INSERT - Searching at depth %d if %d is the scapegoat\n", stack_top, scapegoat->key);	
			#endif

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
				#ifdef DEBUG
				printf("ON INSERT - Node %d is alpha-weight-balanced\n", scapegoat->key);
				#endif
			} else {
				// Scapegoat found
				#ifdef DEBUG
				printf("ON INSERT - Scapegoat found! Node %d is not alpha-weight-balanced, rebalancing around it\n", scapegoat->key);
				#endif

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

	#ifdef DEBUG
	printf("INSERT - key = %d\n", key);
	#endif

	if (parent == NULL) {
		#ifdef DEBUG
		printf("INSERT - Tree empty, adding %d as root\n", key);
		#endif

		parent = malloc(sizeof(t_sg_node));
		parent->key = key;
		parent->left = NULL;
		parent->right = NULL;

		sg_tree->root = parent;

		// Insert successfull
		// Root is never DEEP -> No rebalance required

		sg_on_insert(sg_tree, NULL, 0);

		return TRUE;
	}

	stack = malloc(sizeof(t_sg_node*) * (sg_tree->h_alpha + 1));
	// When inserting a node: stack_top == depth
	stack_top = 0;
	stack[stack_top++] = parent;

	while(TRUE) {
		if (key == parent->key) {
			// Found key, don't overwrite
			#ifdef DEBUG
			printf("INSERT - Key %d already present, insert failed\n", key);
			#endif

			free(stack);
			return FALSE;
		}

		if (key > parent->key) {
			#ifdef DEBUG
			printf("INSERT - Key %d > %d\n", key, parent->key);
			#endif
			if (parent->right == NULL) {
				// Insert right
				#ifdef DEBUG
				printf("INSERT - Adding %d as right child of it's parent\n", key);
				#endif

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
			#ifdef DEBUG
			printf("INSERT - Key %d < %d\n", key, parent->key);
			#endif
			if (parent->left == NULL) {
				// Insert left
				#ifdef DEBUG
				printf("INSERT - Adding %d as left child of it's parent\n", key);
				#endif

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
	if (sg_node == NULL) {
		return 0;
	}
	return sg_calc_size(sg_node->left) + 1 + sg_calc_size(sg_node->right);
}

t_sg_node* sg_rebuild(unsigned int sg_scape_size, t_sg_node* sg_scape_node) {
	// sg_dummy_node is a concrete node allocated on the stack
	t_sg_node sg_dummy_node, *sg_flatten_node, *temp;
	sg_dummy_node.key = 0;
	sg_dummy_node.left = NULL;
	sg_dummy_node.right = NULL;
	sg_flatten_node = sg_flatten(sg_scape_node, &sg_dummy_node);
	temp = sg_flatten_node;
	while (temp != NULL) {
		printf("%d ", temp->key);
		temp->left = NULL;
		temp = temp->right;
	}
	printf("\n");
	assert(sg_flatten_node != NULL);
	printf("%d\n", sg_scape_size);
	sg_build_tree(sg_scape_size, sg_flatten_node);
	printf("%08x %08x %08x\n", &sg_dummy_node, sg_dummy_node.left, sg_dummy_node.right);
	assert(sg_dummy_node.left != NULL);
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
	assert(sg_node_x != NULL);
	if (sg_size == 0) {
		sg_node_x->left = NULL;
		return sg_node_x;
	}
	sg_node_r = sg_build_tree(ceil((sg_size - 1) / 2), sg_node_x);
	assert(sg_node_r != NULL);
	sg_node_s = sg_build_tree(floor((sg_size - 1) / 2), sg_node_r->right);
	assert(sg_node_s != NULL);
	sg_node_r->right = sg_node_s->left;
	sg_node_s->left = sg_node_r;

	return sg_node_s;
}
