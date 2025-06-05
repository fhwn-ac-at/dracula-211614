CC = clang
CFLAGS += -Wall -Wextra -Werror --std=c17

random: random.c
graph: graph_main.c graph.c

clean:
	rm -f random *.o

.PHONY: clean
