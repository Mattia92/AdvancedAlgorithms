function pause() {
	read -n1 -r -p "Press any key to continue..." key
}

clang -Wall -Wextra -O3 -c scapegoat.c -o scapegoat.lib -lm
clang++ -Wall -Wextra -O3 bench.cpp scapegoat.lib -o bench
pause
./bench
pause
