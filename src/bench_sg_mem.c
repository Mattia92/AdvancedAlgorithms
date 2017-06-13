#include <stdio.h> /* printf, scanf_s */

#include "scapegoat.h"

#define M 10
#define N M * 1000 * 1000

int main(void) {
	struct sg_tree* sg_tree;
	unsigned int i;
	char d;

	printf("=== BENCHMARK SG MEM - START ===\n");

	printf("BENCHMARK SG MEM - Inserting %dM keys into scapegoat tree\n", M);
	
	sg_tree = sg_create_tree(0.55);

	for (i = 0; i < N; i++) {
		sg_insert(sg_tree, i);
	}

	printf("BENCHMARK SG MEM - Insertions done, measure memory usage then press any key to exit\n");

	scanf_s("%c", &d);

	printf("=== BENCHMARK SG MEM - END ===\n");

	return 0;
}
