PROJECT = hello
CFLAGS = -g -Wall -std=c99
LDFLAGS = -lm

$(PROJECT):	hello.o
		$(CC) hello.o $(LDFLAGS) -o $@ 	

clean: 
	rm -f *.o $(PROJECT)

hello.o: hello.c

