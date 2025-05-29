all:
	gcc -Wno-deprecated-declarations test.c lib.c -o target/test