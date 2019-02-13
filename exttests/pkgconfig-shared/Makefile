all: test

test: test.c
	${CC} -o test test.c `pkg-config --cflags --libs libversion`
