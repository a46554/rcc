CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

rcc: $(OBJS)
		$(CC) -o rcc $(SRCS) $(LDFLAGS)
		
$(OBJS):rcc.h
	
test: rcc
	./test.sh
	
clean:
	rm -rf rcc *.o *~ tmp*

.PHONY: test clean
