SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=%.o)
BIN=$(OBJ:%.o=%)

#CFLAGS=-Wall -g -c
#CC ?= gcc
#CFLAGS = -std=gnu99 -Wall -I../inc -g -fPIC
#TFLAGS = -Wall -I../inc -lm -pthread -lev -L../bin -lldal

CC ?= gcc
CFLAGS = -std=gnu99 -Wall -I../inc -g -c -fPIC
TFLAGS = -lm -pthread -lev -L../bin -lldal

all:$(BIN)

$(BIN):%:%.o
	$(CC) $^ $(TFLAGS) -o $@
	@rm *.o

$(OBJ):%.o:%.c
	$(CC) $(CFLAGS) $^ $(TFLAGS) -o $@

PHONY: clean
clean:
	@rm $(BIN)
	@echo "Cleanup complete!"
