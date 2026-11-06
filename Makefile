CC = gcc
CFLAGS = -O3 -Wall -Wextra -mpopcnt
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# All source files in src/ (excluding main.c)
SRCS_LIB = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
OBJS_LIB = $(SRCS_LIB:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/chess_engine

# Test targets
TEST_OBJS = $(OBJS_LIB) $(OBJ_DIR)/test_movegen.o
TEST_TARGET = $(BIN_DIR)/test_movegen

# Default target
all: $(TARGET)

$(TARGET): $(OBJ_DIR)/main.o $(OBJS_LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_DIR)/main.o $(OBJS_LIB) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for test objects
$(OBJ_DIR)/test_movegen.o: tests/test_movegen.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I./src -c $< -o $@

$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Sanitize build for testing
sanitize: CFLAGS += -fsanitize=address,undefined
sanitize: $(TARGET)

.PHONY: all clean sanitize test
