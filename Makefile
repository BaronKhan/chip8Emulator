SOURCES = $(wildcard src/*.c)
test:
	gcc $(SOURCES) -o tst/testFunc
	./tst/testFunc

