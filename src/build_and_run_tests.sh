function pause(){
read -n1 -r -p "Press any key to continue..." key
}

clang -Wall -Wextra -DDEBUG -O3 -o test_random test_random.c scapegoat.c test_utils.c
pause
./test_random
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_1 test_1.c scapegoat.c test_utils.c
pause
./test_1
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_2 test_2.c scapegoat.c test_utils.c
pause
./test_2
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_3 test_3.c scapegoat.c test_utils.c
pause
./test_3
pause
clang -Wall -Wextra -DDEBUG -O3 -o test_4 test_4.c scapegoat.c test_utils.c
pause
./test_4
pause
