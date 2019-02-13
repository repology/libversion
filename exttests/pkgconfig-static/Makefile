all: test

test: test.c
	${CC} -o test test.c --static `pkg-config --static --cflags --libs libversion`
