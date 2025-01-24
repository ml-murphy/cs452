prog=deq

include ../GNUmakefile

test:      $(prog) ; ./test
# valgrind: $(prog) ; $@ --track-origins=yes --leak-check=full --show-leak-kinds=all ./$< $(args)

try: main.o libdeq.so
	gcc -o $@ $< -L. -ldeq -Wl,-rpath=.
