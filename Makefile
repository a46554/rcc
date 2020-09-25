CFLAGS=-std=c11 -ggdb -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LDFLAGS=-g

rcc: $(OBJS)
		$(CC) -o rcc $(SRCS) $(LDFLAGS)
		
$(OBJS):rcc.h
	
test: rcc
	./test.sh
	
clean:
	rm -rf rcc *.o *~ tmp*

.PHONY: test clean
