TARGET = libldal.so

CC ?= gcc
CFLAGS = -std=gnu99 -Wall -I./inc -g -fPIC

LINKER = $(CC)
LFLAGS = -Wall -I./inc -lm -pthread -lev -shared
TFLAGS = -Wall -I./inc -lm -pthread -lev -L./bin -lldal

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin
TESTDIR = test
TOOLDIR = tools

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
	$(CC) $(CFLAGS) $(TESTDIR)/test_memory.c $(TFLAGS) -o $(BINDIR)/test_memory
	$(CC) $(CFLAGS) $(TESTDIR)/test_file.c $(TFLAGS) -o $(BINDIR)/test_file
	$(CC) $(CFLAGS) $(TESTDIR)/test_serial.c $(TFLAGS) -o $(BINDIR)/test_serial
	$(CC) $(CFLAGS) $(TESTDIR)/pmsxx.c $(TFLAGS) -o $(BINDIR)/pmsxx
	$(CC) $(CFLAGS) $(TESTDIR)/test_rtc.c $(TFLAGS) -o $(BINDIR)/test_rtc
	$(CC) $(CFLAGS) $(TESTDIR)/test_gpio.c $(TFLAGS) -o $(BINDIR)/test_gpio
	$(CC) $(CFLAGS) $(TESTDIR)/test_di.c $(TFLAGS) -o $(BINDIR)/test_di
	$(CC) $(CFLAGS) $(TESTDIR)/test_do.c $(TFLAGS) -o $(BINDIR)/test_do
	$(CC) $(CFLAGS) $(TESTDIR)/test_ai.c $(TFLAGS) -o $(BINDIR)/test_ai
	$(CC) $(CFLAGS) $(TESTDIR)/create_aicfgs.c $(TFLAGS) -o $(BINDIR)/create_aicfgs
	$(CC) $(CFLAGS) $(TESTDIR)/test_battery.c $(TFLAGS) -o $(BINDIR)/test_battery
	$(CC) $(CFLAGS) $(TESTDIR)/test_ibutton.c $(TFLAGS) -o $(BINDIR)/test_ibutton
	$(CC) $(CFLAGS) $(TESTDIR)/udp_client.c $(TFLAGS) -o $(BINDIR)/udp_client
	$(CC) $(CFLAGS) $(TESTDIR)/udp_server.c $(TFLAGS) -o $(BINDIR)/udp_server
	$(CC) $(CFLAGS) $(TESTDIR)/tcp_client.c $(TFLAGS) -o $(BINDIR)/tcp_client
	$(CC) $(CFLAGS) $(TESTDIR)/test_ec25.c $(TFLAGS) -o $(BINDIR)/test_ec25
	$(CC) $(CFLAGS) $(TESTDIR)/k37x_sample.c $(TFLAGS) -o $(BINDIR)/k37x_sample
	@echo "Compiled test complete!"

tool:
	$(CC) $(TOOLDIR)/udp_echo_server.c -o $(BINDIR)/udp_echo_server
	$(CC) $(TOOLDIR)/tcp_echo_server.c -o $(BINDIR)/tcp_echo_server
	$(CC) $(TOOLDIR)/list_device.c -o $(BINDIR)/list_device
	@echo "Compiled tools complete!"