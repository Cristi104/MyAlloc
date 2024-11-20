RunTest: Test
	./test

Test: libMyAlloc
	gcc -L. -Wl,-rpath=. -Wall -o test Test.c -lMyAlloc

libMyAlloc: MyAlloc.o
	gcc -shared -o libMyAlloc.so MyAlloc.o

MyAlloc.o:
	gcc -c -Wall -Werror -fpic MyAlloc.c

clean:
	rm MyAlloc.o
	rm test