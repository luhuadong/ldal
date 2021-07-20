PHONY: all  clean
SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=%.o)
BIN=$(OBJ:%.o=%)

#CFLAGS=-Wall -g -c
CC ?= gcc
CFLAGS = -std=gnu99 -Wall -I../inc -g -fPIC
TFLAGS = -Wall -I../inc -lm -pthread -lev -L../bin -lldal

#$(CC) $^ -o $@

all:$(BIN)

$(BIN):%:%.o
	$(CC) $(CFLAGS) $^ $(TFLAGS) -o $@

$(OBJ):%.o:%.c
	$(CC) $(CFLAGS) $^ $(TFLAGS) -o $@
 
clean:
	@rm $(OBJ) $(BIN)
	@echo "Cleanup complete!"
