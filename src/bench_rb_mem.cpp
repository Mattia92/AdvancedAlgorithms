#include <stdio.h> /* printf, fgets */
#include <set> /* std::set<int> */

#define M 10
#define N M * 1000 * 1000
#define BUF_SIZE 2

int main(void) {
	std::set<int> rb_tree;
	unsigned int i;
	char d[BUF_SIZE];

	printf("=== BENCHMARK RB MEM - START ===\n");

	printf("BENCHMARK RB MEM - Inserting %dM keys into red black tree\n", M);

	for (i = 0; i < N; i++) {
		rb_tree.insert(i);
	}

	printf("BENCHMARK RB MEM - Insertions done, measure memory usage then press any key to exit\n");

	// Read at most BUF_SIZE - 1 character, stopping after EOF or '\n' from stdin and store '\0' as BUF_SIZE char
	fgets(d, BUF_SIZE, stdin);

	rb_tree.clear();	

	printf("=== BENCHMARK RB MEM - END ===\n");

	return 0;
}
