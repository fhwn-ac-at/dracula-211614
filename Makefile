CC = clang
CFLAGS += -Wall -Wextra -Werror --std=c17 -D_XOPEN_SOURCE=500
INCLUDES += -Iinclude
VERSION += -DVERSION_MAJOR=0 -DVERSION_MINOR=2 -DVERSION_PATCH=2
DEBUG = -UDEBUG
SRC = src

sals:
	$(CC) $(CFLAGS) $(INCLUDES) $(VERSION) $(DEBUG) $(SRC)/*.c -o sals

clean:
	rm -f sals *.o

.PHONY: clean
