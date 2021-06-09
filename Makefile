TARGET = ldal

CC = gcc
CFLAGS = -std=c99 -Wall -I. -g

LINKER = gcc
LFLAGS = -Wall -I. -lm

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
INCLUDES := $(shell find $(SRCDIR) -type f -name '*.h')
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@rm -rf $(BINDIR) $(OBJDIR)
	@echo "Cleanup complete!"
