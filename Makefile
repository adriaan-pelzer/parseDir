CFLAGS=-g -Wall -fPIC -I/usr/local/include
LIBS=-L/usr/local/lib

all: libParseDir

test: testParseDir
	rm -rf in
	mkdir in
	echo "1" > in/1
	sleep 1
	echo "2" > in/2
	sleep 1
	echo "3" > in/3
	sleep 1
	echo "4" > in/4
	sleep 1
	echo "5" > in/5
	./testParseDir in

testParseDir: Makefile parseDir.h testParseDir.c
	gcc ${CFLAGS} -o testParseDir testParseDir.c ${LIBS} -lParseDir

libParseDir: Makefile parseDir.o parseDir.h
	gcc -shared -o libParseDir.so.1.0 parseDir.o ${LIBS}

parseDir.o: Makefile parseDir.h parseDir.c
	gcc ${CFLAGS} -c parseDir.c -o parseDir.o

install:
	cp libParseDir.so.1.0 /usr/local/lib
	ln -sf /usr/local/lib/libParseDir.so.1.0 /usr/local/lib/libParseDir.so.1
	ln -sf /usr/local/lib/libParseDir.so.1.0 /usr/local/lib/libParseDir.so
	ldconfig /usr/local/lib
	cp parseDir.h /usr/local/include/parseDir.h

clean:
	rm *.o; rm *.so*; rm core*; rm testParseDir
