all:
	make clean
	gcc -I include -c streamjs.c

deps:
	git clone https://github.com/adarqui/libret-c
	cd libret-c && make test && make install

test:
	gcc -I include/ -ggdb -g3 -Wall test.c streamjs.c -lcunit -o run-test
	./run-test

clean:
	rm -f run-test run-demo *.o *.so
