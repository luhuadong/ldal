TARGET = libldal.so

CC = gcc
CFLAGS = -std=gnu99 -Wall -I./inc -g -fPIC

LINKER = gcc
LFLAGS = -Wall -I./inc -lm -pthread -shared
TFLAGS = -Wall -I./inc -lm -pthread

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin
TESTDIR = test

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
INCLUDES := $(shell find $(INCDIR) -type f -name '*.h')
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@rm -rf $(BINDIR) $(OBJDIR)
	@echo "Cleanup complete!"

install:
	@install $(BINDIR)/$(TARGET) /usr/local/lib
	@echo "Install complete!"

testcase:
	$(CC) $(CFLAGS) $(TESTDIR)/test_memory.c $(TFLAGS) -lldal -o $(BINDIR)/test_memory
	$(CC) $(CFLAGS) $(TESTDIR)/test_file.c $(TFLAGS) -lldal -o $(BINDIR)/test_file
	$(CC) $(CFLAGS) $(TESTDIR)/test_serial.c $(TFLAGS) -lldal -o $(BINDIR)/test_serial
	$(CC) $(CFLAGS) $(TESTDIR)/test_rtc.c $(TFLAGS) -lldal -o $(BINDIR)/test_rtc
	$(CC) $(CFLAGS) $(TESTDIR)/test_gpio.c $(TFLAGS) -lldal -o $(BINDIR)/test_gpio
	$(CC) $(CFLAGS) $(TESTDIR)/pmsxx.c $(TFLAGS) -lldal -o $(BINDIR)/pmsxx
	@echo "Compiled test complete!"