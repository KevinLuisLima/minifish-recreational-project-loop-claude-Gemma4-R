CC = gcc
CFLAGS = -O3 -Wall -Wextra
LDFLAGS = -lm

SRC = main.c bitboard.c evaluation.c position.c search.c
OBJ = $(SRC:.c=.o)
TARGET = chess_engine

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *.bin

.PHONY: all clean
