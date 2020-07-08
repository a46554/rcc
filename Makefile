CFLAGS=-std=c11 -g -static

rcc: rcc.c
test: rcc
	./test.sh
clean:
	rm -rf rcc *.o *~ tmp*

.PHONY: test clean
