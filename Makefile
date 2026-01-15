CFLAGS = -Wall -Wextra -Og -ggdb
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -L/usr/local/lib
IFLAGS = -Iusr/local/include

all:
	gcc -o game src/main.c $(CFLAGS) $(LDFLAGS) $(IFLAGS)
