all:
	gcc -O0 -ggdb -masm=intel -o test ./test.c
	objdump -M intel -h -S ./test > ./test.lss
