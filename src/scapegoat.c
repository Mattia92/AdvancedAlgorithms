#include <stdio.h>	/* printf */
#include <math.h>	/* log, floor, ceil, fmax */
#include <stdlib.h> /* malloc, free */
#include "scapegoat.h"

// Compile with -DDEBUG to enable debugging

#ifndef DEBUG
	#define NDEBUG
#endif
#include <assert.h> /* assert */

// Compile with -DSECURE_REBUILD to improve debugging of sg_rebuild

#define TRUE 1
#define FALSE 0

// Creates a tree with a given alpha
t_sg_tree* sg_create_tree(double alpha) {
	t_sg_tree* tree;
	printf("=== CREATE(%.2f) - START ===\n", alpha);
	// 0.999 max alpha allowed for numerical stability
	if (alpha < 0.5 || alpha > 0.999) {
		printf("=== CREATE(%.2f) - INVALID ALPHA - Please choose an alpha in [0.5, 0.999], using default 0.5 ===\n", alpha);
		alpha = 0.5;
	}
	tree = malloc(sizeof(t_sg_tree));
	tree->alpha = alpha;
	tree->root = NULL;
	tree->size = 0;
	tree->max_size = 0;
	tree->h_alpha = 1;
	printf("=== CREATE(%.2f) - SUCCESS ===\n", alpha);
	return tree;
}

// Deletes all the nodes in the tree and free the tree data structure
void sg_delete_tree(t_sg_tree* tree) {
	sg_delete_node(tree->root);
	free(tree);
}

// Recursively delete left and right child,
// then free node itself
void sg_delete_node(t_sg_node* node) {
	if (node->left != NULL) {
		sg_delete_node(node->left);
	}
	if (node->right != NULL) {
		sg_delete_node(node->right);
	}
	free(node);
}

// NULL if not present, pointer to node if present
t_sg_node* sg_search(t_sg_tree* tree, int key) {
	t_sg_node *temp = tree->root;

	#ifdef DEBUG
	printf("=== SEARCH(%d) - START ===\n", key);
	#endif

	while (temp != NULL) {
		if (temp->key == key) {
			// Found key

			#ifdef DEBUG
			printf("=== SEARCH(%d) - SUCCESS ===\n", key);
			#endif

			return temp;
		}

		if (key > temp->key) {

			#ifdef DEBUG
			printf("SEARCH(%d) - Key > %d, moving to right child\n", key, temp->key);
			#endif

			temp = temp->right;
		} else {

			#ifdef DEBUG
			printf("SEARCH(%d) - Key < %d, moving to left child\n", key, temp->key);
			#endif

			temp = temp->left;
		}
	}

	#ifdef DEBUG
	printf("=== SEARCH(%d) - FAILURE ===\n", key);
	#endif

	return NULL;
}

unsigned int h_alpha(unsigned int size, double alpha) {
	return (unsigned int)floor(log((double)size) / log(1 / alpha));
}

// Update h_alpha
void sg_update_h_alpha(t_sg_tree* tree) {
	tree->h_alpha = tree->size < 2 ? 1 : h_alpha(tree->size, tree->alpha);
}

// Update tree after node deletion and rebuild tree from root if needed
void sg_on_delete(t_sg_tree* tree) {
	#ifdef DEBUG
	printf("ON DELETE - START\n");
	#endif

	// Decrease tree size
    --tree->size;
    // Update h_alpha
    sg_update_h_alpha(tree);

    #ifdef DEBUG
    printf("ON DELETE - tree->size = %d, tree->h_alpha = %d\n", tree->size, tree->h_alpha);
    #endif

    // Check if rebalance is needed		
	if (tree->size >= 3 && tree->size < tree->alpha * tree->max_size) {
		#ifdef DEBUG
		printf("ON DELETE - Rebalancing tree\n");
		#endif

		tree->root = sg_rebuild(tree->size, tree->root);
		tree->max_size = tree->size;

		#ifdef DEBUG
		printf("ON DELETE - END - Tree rebalanced: tree->root->key = %d\n", tree->root->key);
		#endif

		return;
	}

	#ifdef DEBUG
	printf("ON DELETE - END - No rebalance required\n");
	#endif
}

// 1 if key found and node deleted, 0 otherwise
char sg_delete(t_sg_tree* tree, int key) {
	t_sg_node *parent = NULL, *to_remove = tree->root, *replacement, *temp;
	char is_right_child;

	#ifdef DEBUG
	printf("=== DELETE(%d) - START ===\n", key);
	#endif

	while(to_remove != NULL) {
		if (to_remove->key == key) {
			// to_remove is the node to be deleted
			#ifdef DEBUG
			printf("DELETE(%d) - KEY FOUND, to_remove->key = %d\n", key, to_remove->key);
			#endif

			// Three disjoint cases
			// 1) to_remove is a leaf
			// 2) to_remove has only one child
			// 3) to_remove has two children

			// 1) to_remove is a leaf
			if (to_remove->left == NULL && to_remove->right == NULL) {
				#ifdef DEBUG
				printf("DELETE(%d) - 1: to_remove is a leaf\n", key);
				#endif

				// 1.1) to_remove is root
				if (parent == NULL) {
					assert(to_remove == tree->root);

					#ifdef DEBUG
					printf("DELETE(%d) - 1.1: to_remove root\n", key);
					#endif

					tree->root = NULL;
				// 1.2) to_remove is not root
				} else {
					assert(to_remove != tree->root);

					#ifdef DEBUG
					printf("DELETE(%d) - 1.2: to_remove is not root, parent->key = %d\n", key, parent->key);
					#endif

					if (is_right_child) {
						assert(parent->right == to_remove);
                		parent->right = NULL;
					}
             		else {
             			assert(parent->left == to_remove);
             			parent->left = NULL;
             		}
				}
             	free(to_remove);
            // 2) to_remove has only one child
			} else if (to_remove->left == NULL || to_remove->right == NULL) {
				// temp is the only child of to_remove
				temp = to_remove->left != NULL ? to_remove->left : to_remove->right;

				#ifdef DEBUG
				printf("DELETE(%d) - 2: to_remove has only one child with key = %d\n", to_remove->key, temp->key);
				#endif

				assert(temp != NULL);
				// 2.1) to_remove is the root
				if (parent == NULL) {
					assert(to_remove == tree->root);
					#ifdef DEBUG
					printf("DELETE(%d) - 2.1: to_remove is root, changing root to %d\n", key, temp->key);
					#endif
					tree->root = temp;
				// 2.2) to_remove is not the root
				} else {
					assert(to_remove != tree->root);
					#ifdef DEBUG
					printf("DELETE(%d) - 2.2: to_remove not root, changing ptr to to_remove in parent %d with ptr to child %d\n",
						key, parent->key, temp->key);
					#endif
					if (is_right_child) {
						assert(parent->right == to_remove);
						parent->right = temp;
					} else {
						assert(parent->left == to_remove);
						parent->left = temp;
					}
				}
				free(to_remove);
			// 3) to_remove has two children
			} else {
				#ifdef DEBUG
				printf("DELETE(%d) - 3) to_remove has two children L=%d, R=%d\n"
					"   replacement node is the highest node less than to_remove (it has no right child)\n",
					key,
					to_remove->left->key,
					to_remove->right->key);
				#endif

				// Note: to_remove has it's key always overwritten with the one
				// in the replacement node. to_remove is not freed in this case.

				// Find replacement i.e. the highest node less than to_remove.
				// It is the largest member in the left subtree.
				replacement = to_remove->left;
				
				// 3.1)
				if (replacement->right == NULL) {
					#ifdef DEBUG
					printf("DELETE(%d) - 3.1) replacement node %d, left child of to_remove\n"
						"   to_remove->key = replacement->key\n"
						"   to_remove->right = replacement->right\n"
						"   to_remove->left untouched, Freeing replacement\n",
						key,
						replacement->key);
					#endif

					to_remove->key = replacement->key;
					to_remove->left = replacement->left;
					free(replacement);
				// 3.2)
				} else {
					// replacement here points to replacement parent,
					// thus replacement node is right child of replacement
					while (replacement->right->right != NULL) {
						replacement = replacement->right;
					}

					#ifdef DEBUG
					printf("DELETE(%d) - 3.2) replacement node %d, replacement parent %d\n"
						"   to_remove->key = replacement->key\n"
						"   replacement_parent->right = replacement->left\n"
						"   Freeing replacement node\n",
						key,
						replacement->right->key,
						replacement->key);
					#endif

					to_remove->key = replacement->right->key;
					temp = replacement->right;
					replacement->right = replacement->right->left;
					free(temp);
				}				
			}

			// Scapegoat logic
			sg_on_delete(tree);

			#ifdef DEBUG
			printf("=== DELETE(%d) - SUCCESS ===\n", key);
			#endif

			return TRUE;
		}

		parent = to_remove;
		if (key > to_remove->key) {
			#ifdef DEBUG
			printf("DELETE(%d) - Key > %d, moving to right child\n", key, to_remove->key);
			#endif

			to_remove = to_remove->right;
			is_right_child = TRUE;
		} else {
			#ifdef DEBUG
			printf("DELETE(%d) - Key < %d, moving to left child\n", key, to_remove->key);
			#endif

			to_remove = to_remove->left;
			is_right_child = FALSE;
		}
	}

	#ifdef DEBUG
	printf("=== DELETE(%d) - FAILURE ===\n", key);
	#endif

	return FALSE;
}

// Update tree on insert, check if inserted node is deep and if so rebalance around a scapegoat
void sg_on_insert(t_sg_tree* tree, t_sg_node** stack, unsigned int stack_top) {
	// stack_top equals the depth of the newly inserted node
	t_sg_node *scapegoat;
	unsigned int scapegoat_left_size, scapegoat_right_size, scapegoat_size = 0;

	assert((stack_top == 0 && stack == NULL) || (stack_top > 0 && stack != NULL));

	#ifdef DEBUG
	printf("ON INSERT - START - Node inserted at depth %d\n", stack_top);
	#endif

	// Adjust tree vars
	++tree->size;
	tree->max_size = (unsigned int)fmax(tree->size, tree->max_size);
	sg_update_h_alpha(tree);

	#ifdef DEBUG
	printf("ON INSERT - tree->size = %d, tree->max_size = %d, tree->h_alpha = %d\n",
		tree->size,
		tree->max_size,
		tree->h_alpha);
	#endif

	// Check if the newly inserted node is deep
	if (stack_top > tree->h_alpha) {
		#ifdef DEBUG
		printf("ON INSERT - New node is deep, searching a scapegoat to rebalance the tree\n");	
		#endif

		while (TRUE) {
			assert(stack_top > 0);

			// Search scapegoat
			scapegoat = stack[--stack_top];

			assert(scapegoat != NULL && scapegoat == stack[stack_top]);

			#ifdef DEBUG
			printf("ON INSERT - depth = %d, checking if %d is the scapegoat\n", stack_top, scapegoat->key);	
			#endif

			// From now on stack[stack_top - 1] is the parent of scapegoat if stack_top >= 1

			// First iteration of the loop
			if (scapegoat_size == 0) {
				scapegoat_right_size = sg_calc_size(scapegoat->right);
				scapegoat_left_size = sg_calc_size(scapegoat->left);
			// Further iterations, previous sizes must be reused 
			} else if (scapegoat->right == stack[stack_top + 1]) {
				// Previous node tested is the right child of current one
				scapegoat_right_size = scapegoat_size;
				scapegoat_left_size = sg_calc_size(scapegoat->left);
			} else {
				// Previous node tested is the left child of current one
				scapegoat_right_size = sg_calc_size(scapegoat->right);
				scapegoat_left_size = scapegoat_size;
			}

			scapegoat_size = 1 + scapegoat_right_size + scapegoat_left_size;

			if (scapegoat_left_size <= tree->alpha * scapegoat_size &&
				scapegoat_right_size <= tree->alpha * scapegoat_size) {
				// Node is alpha-weight-balanced
				#ifdef DEBUG
				printf("ON INSERT - depth = %d, node %d is alpha-weight-balanced\n",
					stack_top,
					scapegoat->key);
				#endif
			} else {
				// Scapegoat found
				#ifdef DEBUG
				printf("ON INSERT - depth = %d, scapegoat found! node %d is not alpha-weight-balanced\n",
					stack_top,
					scapegoat->key);
				#endif

				// Scapegoat is root
				if (stack_top == 0) {
					tree->root = sg_rebuild(scapegoat_size, scapegoat);
				// Scapegoat is not root
				} else if (scapegoat == stack[stack_top - 1]->right) {
					// Scapegoat is the right child of its parent
					stack[stack_top - 1]->right = sg_rebuild(scapegoat_size, scapegoat);
				} else {
					// Scapegoat is the left child of its parent
					stack[stack_top - 1]->left = sg_rebuild(scapegoat_size, scapegoat);
				}
			
				#ifdef DEBUG
				printf("ON INSERT - END - Tree rebalanced\n");
				#endif
				return;
			}
		}
	}
}

// 1 if key was not present so new node added, 0 otherwise
char sg_insert(t_sg_tree* tree, int key) {
	// Stack to save the nodes traversed to reach insertion point
	// insertion point max depth = h_alpha + 1
	t_sg_node **stack, *parent = tree->root;
	unsigned int stack_top;

	#ifdef DEBUG
	printf("=== INSERT(%d) - START ===\n", key);
	#endif

	if (parent == NULL) {
		// Tree is empty
		parent = malloc(sizeof(t_sg_node));
		parent->key = key;
		parent->left = NULL;
		parent->right = NULL;

		tree->root = parent;

		// Insert successfull
		// Root is never DEEP -> No rebalance required
		sg_on_insert(tree, NULL, 0);

		#ifdef DEBUG
		printf("=== INSERT(%d) - SUCCESS - Node added as new root ===\n", key);
		#endif
		return TRUE;
	}

	stack = malloc(sizeof(t_sg_node*) * (tree->h_alpha + 1));
	// When inserting a node: stack_top == depth
	stack_top = 0;
	stack[stack_top++] = parent;

	while(TRUE) {
		if (key == parent->key) {
			// Found key, don't overwrite
			#ifdef DEBUG
			printf("=== INSERT(%d) - FAILURE - Key already present ===\n", key);
			#endif

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

				#ifdef DEBUG
				printf("INSERT(%d) - Node inserted right\n", key);
				#endif

				sg_on_insert(tree, stack, stack_top);
				free(stack);

				#ifdef DEBUG
				printf("=== INSERT(%d) - SUCCESS ===\n", key);
				#endif

				return TRUE;
			}

			#ifdef DEBUG
			printf("INSERT(%d) - Key > %d, moving to right child\n", key, parent->key);
			#endif
			parent = parent->right;
		} else {
			if (parent->left == NULL) {
				// Insert left

				parent->left = malloc(sizeof(t_sg_node));
				parent->left->key = key;
				parent->left->right = NULL;
				parent->left->left = NULL;

				#ifdef DEBUG
				printf("INSERT(%d) - Node inserted left\n", key);
				#endif

				sg_on_insert(tree, stack, stack_top);
				free(stack);

				#ifdef DEBUG
				printf("=== INSERT(%d) - SUCCESS ===\n", key);
				#endif

				return TRUE;
			}

			#ifdef DEBUG
			printf("INSERT(%d) - Key < %d, moving to left child\n", key, parent->key);
			#endif
			parent = parent->left;
		}

		// stack size = sg_tree->h_alpha + 1,
		// last valid stack_top = sg_tree->h_alpha
		// This assert avoids infinite loops while debugging
		assert(stack_top >= 0 && stack_top < tree->h_alpha + 1);
		stack[stack_top++] = parent;
	}
}

unsigned int sg_calc_size(t_sg_node* node) {
	if (node == NULL) {
		return 0;
	}
	if (node->left == NULL && node->right == NULL) {
		return 1;
	}
	return sg_calc_size(node->left) + 1 + sg_calc_size(node->right);
}

// Rebuilds the subtree rooted at scapegoat, which size is n
// Returns the pointer to the new root of the rebuilt subtree
t_sg_node* sg_rebuild(unsigned int n, t_sg_node* scapegoat) {
	t_sg_node w, *z;

	#ifdef SECURE_REBUILD
	t_sg_node *temp;
	unsigned int size = sg_calc_size(scapegoat);
	assert(n == size);
	#endif

	w.key = 0;
	w.left = NULL;
	w.right = NULL;

	z = sg_flatten(scapegoat, &w);

	assert(w.key == 0 && w.left == NULL && w.right == NULL);

	#ifdef SECURE_REBUILD
	temp = z;
	while (temp != NULL) {
		temp->left = NULL;
		temp = temp->right;
	}
	size = sg_calc_size(z);
	assert(n + 1 == size);
	#endif

	// z is the head of the list of flattened nodes
	// z->right is the ptr to first node
	// the last node in the list is w

	assert(z != NULL);
	
	sg_build_tree(n, z);

	assert(w.left != NULL);

	#ifdef SECURE_REBUILD
	size = sg_calc_size(w.left);
	assert(n == size);
	#endif

	return w.left;
}

// Flatten the subtree rooted at x and append to the last element
// of the flattened list the list which head is y.
// Returns the pointer to the first node of the flattened list
t_sg_node* sg_flatten(t_sg_node* x, t_sg_node* y) {
	if (x == NULL) {
		return y;
	}
	x->right = sg_flatten(x->right, y);
	return sg_flatten(x->left, x);
}

// Builds a 1/2-weight-balanced tree of size n.
// x is the list of nodes of the tree plus a dummy node at the end,
// so the size of the list which head is x is n + 1.
// Returns the pointer to the node which left child is the root of
// the new balanced tree.
t_sg_node* sg_build_tree(unsigned int n, t_sg_node* x) {
	t_sg_node *r, *s;

	assert(x != NULL);

	if (n == 0) {
		x->left = NULL;
		return x;
	}

	r = sg_build_tree((unsigned int)ceil((double)(n - 1) / 2), x);

	assert(r != NULL);

	s = sg_build_tree((unsigned int)floor((double)(n - 1) / 2), r->right);

	assert(s != NULL);

	r->right = s->left;
	s->left = r;

	return s;
}
