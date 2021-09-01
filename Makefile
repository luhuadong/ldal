TARGET = libldal.so

CC ?= gcc
CFLAGS = -std=gnu99 -g -Wall -Wunused-function -I./inc -I./src -fPIC

LINKER = $(CC)
LFLAGS = -Wall -I./inc -I./src -lm -pthread -lev -shared
TFLAGS = -Wall -I./inc -I./src -lm -pthread -lev -L./bin -lldal

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin
TESTDIR = tests
TOOLDIR = tools
INSTALL_DIR = /usr/lib

export CC OBJDIR BINDIR

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
	@install $(BINDIR)/$(TARGET) $(INSTALL_DIR)
	@echo "Install complete!"

testcase:
	make -C $(TESTDIR)

tool:
	$(CC) $(TOOLDIR)/udp_echo_server.c -o $(BINDIR)/udp_echo_server
	$(CC) $(TOOLDIR)/tcp_echo_server.c -o $(BINDIR)/tcp_echo_server
	$(CC) $(TOOLDIR)/list_device.c -o $(BINDIR)/list_device
	@echo "Compiled tools complete!"
