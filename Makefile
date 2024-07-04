# Source files and target executable
TARGET = hasher
SRC = main.c hashmap.c fnv.c utils.c

# Compiler and compiler flags
CC = clang
CFLAGS = -Wall -Wextra -std=c99 -Os 

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Clean rule: remove the executable and any generated files
clean:
	rm -f $(TARGET)

# Phony targets (targets that are not associated with files)
.PHONY: all clean

