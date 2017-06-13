clang -Wall -Wextra -O3 -o bench_sg_mem.exe bench_sg_mem.c scapegoat.c
pause
bench_sg_mem.exe
pause
clang -Wall -Wextra -Xclang -flto-visibility-public-std -O3 -o bench_rb_mem.exe bench_rb_mem.cpp
pause
bench_rb_mem.exe
pause
