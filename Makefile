CC = clang
CFLAGS += -Wall -Wextra -Werror --std=c17 -D_XOPEN_SOURCE=500
INCLUDES += -Iinclude
VERSION += -DVERSION_MAJOR=1 -DVERSION_MINOR=0 -DVERSION_PATCH=0
DEBUG = -UDEBUG
SRC = src

sals:
	$(CC) $(CFLAGS) $(INCLUDES) $(VERSION) $(DEBUG) $(SRC)/*.c -o sals

clean:
	rm -f sals *.o

.PHONY: clean
