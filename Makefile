# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude -I/usr/include
LDFLAGS = -L/usr/include -l ssl -l crypto

# Source files
SRCS = $(wildcard src/*/*.c) src/main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = fwd

# Default target
all: $(EXEC)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) ${LDFLAGS}

# Clean build files
clean:
	rm -f $(OBJS) $(EXEC)