#include <stdio.h> /* printf, scanf_s */
#include <set> /* std::set<int> */

#define M 10
#define N M * 1000 * 1000

int main(void) {
	std::set<int> rb_tree;
	unsigned int i;
	char d;

	printf("=== BENCHMARK RB MEM - START ===\n");

	printf("BENCHMARK RB MEM - Inserting %dM keys into red black tree\n", M);

	for (i = 0; i < N; i++) {
		rb_tree.insert(i);
	}

	printf("BENCHMARK RB MEM - Insertions done, measure memory usage then press any key to exit\n");

	scanf_s("%c", &d);

	rb_tree.clear();

	printf("=== BENCHMARK RB MEM - END ===\n");

	return 0;
}
