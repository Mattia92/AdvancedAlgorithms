function pause() {
	read -n1 -r -p "Press any key to continue..." key
}

clang -Wall -Wextra -O3 -o bench_sg_mem bench_sg_mem.c scapegoat.c
pause
./bench_sg_mem
pause
clang++ -Wall -Wextra -O3 bench_rb_mem.cpp -o bench_rb_mem
pause
./bench_rb_mem
pause
