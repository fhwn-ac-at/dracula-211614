CC = clang
CFLAGS += -Wall -Wextra -Werror --std=c17

graph: main.c graph.c

clean:
	rm -f graph *.o

.PHONY: clean
