.PHONY: all lib debug

all: lib

lib:
	gcc -g -c -fPIC *.c -g3 
	ar rcs libnm.a *.o

debug:
	gcc -g -o Sod main.c -L. -lnm -lm -g3 -pthread
