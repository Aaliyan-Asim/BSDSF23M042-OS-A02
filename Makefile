# ===========================================================
# Makefile for ls Project (v1.4.0 - Alphabetical Sort)
# ===========================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L

# Version (update this for each new feature)
VERSION = 1.4.0

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Files
SRC = $(SRC_DIR)/lsv$(VERSION).c
OBJ = $(OBJ_DIR)/lsv$(VERSION).o
TARGET = $(BIN_DIR)/lsv$(VERSION)

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

# Create directories if they don't exist
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Clean up build files
clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
	@echo "🧹 Cleaned up build files."

# Phony targets
.PHONY: all clean

