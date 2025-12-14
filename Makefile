# Compiler and flags
CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Werror -O2 -Iinclude
LDFLAGS = -lcurl -ljansson -lpthread -lm
DEBUG_FLAGS = -g -O0 -DDEBUG

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests

# Target
TARGET = $(BIN_DIR)/bdix-monitor
TEST_TARGET = $(BIN_DIR)/test-suite

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(OBJ_DIR)/test_%.o)

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Build main executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "✓ Build complete: $(TARGET)"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Build tests
tests: directories $(TEST_TARGET)
	@echo "✓ Tests built: $(TEST_TARGET)"

$(TEST_TARGET): $(filter-out $(OBJ_DIR)/main.o, $(OBJS)) $(TEST_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run tests
check: tests
	@./$(TEST_TARGET)

# Install
install: all
	@echo "Installing to /usr/local/bin..."
	@sudo cp $(TARGET) /usr/local/bin/bdix-monitor
	@sudo chmod +x /usr/local/bin/bdix-monitor
	@echo "✓ Installation complete"

# Uninstall
uninstall:
	@sudo rm -f /usr/local/bin/bdix-monitor
	@echo "✓ Uninstalled"

# Clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "✓ Cleaned"

# Format code
format:
	@find $(SRC_DIR) $(INC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "✓ Code formatted"

# Static analysis
analyze:
	@cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_DIR)

.PHONY: all directories debug tests check install uninstall clean format analyze
