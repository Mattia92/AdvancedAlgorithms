#include <stdio.h> /* printf, fgets */

#include "scapegoat.h"

#define M 10
#define N M * 1000 * 1000
#define BUF_SIZE 2

int main(void) {
	struct sg_tree* sg_tree;
	unsigned int i;
	char d[BUF_SIZE];

	printf("=== BENCHMARK SG MEM - START ===\n");

	printf("BENCHMARK SG MEM - Inserting %dM keys into scapegoat tree\n", M);
	
	sg_tree = sg_create_tree(0.55);

	for (i = 0; i < N; i++) {
		sg_insert(sg_tree, i);
	}

	printf("BENCHMARK SG MEM - Insertions done, measure memory usage then press any key to exit\n");

	// Read at most BUF_SIZE - 1 character, stopping after EOF or '\n' from stdin and store '\0' as BUF_SIZE char
	fgets(d, BUF_SIZE, stdin);

	printf("=== BENCHMARK SG MEM - END ===\n");

	return 0;
}
