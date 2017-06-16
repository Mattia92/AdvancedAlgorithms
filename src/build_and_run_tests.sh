function pause() {
	read -n1 -r -p "Press any key to continue..." key
}

clang -Wall -Wextra -DDEBUG -O3 -o test_random test_random.c test_utils.c scapegoat.c -lm
pause
./test_random
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_1 test_1.c test_utils.c scapegoat.c -lm
pause
./test_1
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_2 test_2.c test_utils.c scapegoat.c -lm
pause
./test_2
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_3 test_3.c test_utils.c scapegoat.c -lm
pause
./test_3
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_4 test_4.c test_utils.c scapegoat.c -lm
pause
./test_4
pause
