libMyAlloc: MyAlloc.o
	gcc -shared -o libMyAlloc.so MyAlloc.o

MyAlloc.o:
	gcc -c -Wall -Werror -fpic MyAlloc.c

clean:
	rm MyAlloc.o
	rm test

uninstall:
	rm MyAlloc.o
	rm test
	rm libMyAlloc.so

RunTest: Test
	./test

Test: libMyAlloc
	gcc -L. -Wl,-rpath=. -Wall -o test Test.c -lMyAlloc
