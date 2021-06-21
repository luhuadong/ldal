TARGET = libldal.so

CC ?= gcc
CFLAGS = -std=gnu99 -Wall -I./inc -g -fPIC

LINKER = $(CC)
LFLAGS = -Wall -I./inc -lm -pthread -shared
TFLAGS = -Wall -I./inc -lm -pthread -L./bin

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
	$(CC) $(CFLAGS) $(TESTDIR)/pmsxx.c $(TFLAGS) -lldal -o $(BINDIR)/pmsxx
	$(CC) $(CFLAGS) $(TESTDIR)/test_rtc.c $(TFLAGS) -lldal -o $(BINDIR)/test_rtc
	$(CC) $(CFLAGS) $(TESTDIR)/test_gpio.c $(TFLAGS) -lldal -o $(BINDIR)/test_gpio
	$(CC) $(CFLAGS) $(TESTDIR)/test_di.c $(TFLAGS) -lldal -o $(BINDIR)/test_di
	$(CC) $(CFLAGS) $(TESTDIR)/test_do.c $(TFLAGS) -lldal -o $(BINDIR)/test_do
	$(CC) $(CFLAGS) $(TESTDIR)/test_ai.c $(TFLAGS) -lldal -o $(BINDIR)/test_ai
	$(CC) $(CFLAGS) $(TESTDIR)/create_aicfgs.c $(TFLAGS) -lldal -o $(BINDIR)/create_aicfgs
	$(CC) $(CFLAGS) $(TESTDIR)/test_battery.c $(TFLAGS) -lldal -o $(BINDIR)/test_battery
	$(CC) $(CFLAGS) $(TESTDIR)/test_ibutton.c $(TFLAGS) -lldal -o $(BINDIR)/test_ibutton
	$(CC) $(CFLAGS) $(TESTDIR)/udp_client.c $(TFLAGS) -lldal -o $(BINDIR)/udp_client
	$(CC) $(CFLAGS) $(TESTDIR)/udp_server.c $(TFLAGS) -lldal -o $(BINDIR)/udp_server
	@echo "Compiled test complete!"
