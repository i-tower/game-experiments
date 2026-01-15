CFLAGS = -Wall -Wextra -O0 -g -fsanitize=address
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -L/usr/local/lib
IFLAGS = -Iusr/local/include
CC = gcc

all:
	$(CC) -o game src/main.c $(CFLAGS) $(LDFLAGS) $(IFLAGS)