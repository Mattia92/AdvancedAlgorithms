clang -Wall -Wextra -Xclang -flto-visibility-public-std -O3 -o bench.exe bench.cpp scapegoat.c
pause
bench.exe
pause
