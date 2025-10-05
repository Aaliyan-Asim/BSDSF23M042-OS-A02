# ===========================================================
# Makefile for lsv1.1.0 Project (Feature 2 - Long Listing)
# ===========================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Files
SRC = $(SRC_DIR)/lsv1.1.0.c
OBJ = $(OBJ_DIR)/lsv1.1.0.o
TARGET = $(BIN_DIR)/ls

# ===========================================================
# Default target
# ===========================================================
all: $(TARGET)

# Build the final executable
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "✅ Build successful! Executable created at $(TARGET)"

# Compile .c to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if not exist
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Clean up build files
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
	@echo "🧹 Cleaned up build files."

# Phony targets
.PHONY: all clean
