CC = clang
CFLAGS += -Wall -Wextra -Werror --std=c17

snakesandladders:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f snakesandladders *.o

.PHONY: clean
