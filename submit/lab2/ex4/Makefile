PROJECT = key-value
SRC_FILES = main.c Makefile key-value.c key-value.h process-file.c process-file.h README

CFLAGS = -g -Wall -std=c11
LDFLAGS = -lm

$(PROJECT):	key-value.o process-file.o main.o
		$(CC) key-value.o process-file.o main.o $(LDFLAGS) -o $@

clean:
		rm -f *.o $(PROJECT) $(PROJECT).tar.gz

depend:
		$(CC) -MM $(CPPFLAGS) *.c

dist:		$(PROJECT).tar.gz

$(PROJECT).tar.gz:	$(SRC_FILES)
			tar -cf $(PROJECT).tar $(SRC_FILES)
			gzip -f $(PROJECT).tar



#automatically generated dependencies produced by make depend
key-value.o: key-value.c key-value.h
main.o: main.c process-file.h
process-file.o: process-file.c key-value.h process-file.h






