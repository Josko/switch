CC = gcc
CFLAGS = -std=c89 -O3 -Wall -Wextra
LDFLAGS = -lxcb

all: switch

switch: switch.o

clean:
	rm -r switch.o switch

