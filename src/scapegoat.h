#ifdef __cplusplus
extern "C" {
#endif

// left == NULL means no left child
// right == NULL means no right child
// If leaf left == NULL and RIGHT == NULL
typedef struct sg_node {
	int key;
	// void* value;
	struct sg_node* left;
	struct sg_node* right;
} t_sg_node;

typedef struct sg_tree {
	t_sg_node* root; // If empty root == NULL and size == 0
	unsigned int size;
	unsigned int max_size;
	double alpha; // 0.5 <= alpha < 1
	unsigned int h_alpha;
} t_sg_tree;

t_sg_tree* sg_create_tree(double alpha);
void sg_delete_tree(t_sg_tree* tree);
void sg_delete_node(t_sg_node* node);
t_sg_node* sg_search(t_sg_tree* tree, int key);
void sg_update_h_alpha(t_sg_tree* tree);
void sg_on_delete(t_sg_tree* tree);
char sg_delete(t_sg_tree* tree, int key);
void sg_on_insert(t_sg_tree* tree, t_sg_node** stack, unsigned int stack_top);
char sg_insert(t_sg_tree* tree, int key);
unsigned int sg_calc_size(t_sg_node* node);
t_sg_node* sg_rebuild(unsigned int n, t_sg_node* scapegoat);
t_sg_node* sg_flatten(t_sg_node* x, t_sg_node* y);
t_sg_node* sg_build_tree(unsigned int n, t_sg_node* x);

#ifdef __cplusplus
}
#endif