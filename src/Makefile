CC = gcc
CFLAGS = -O3 -Wall -Wextra
LDFLAGS = -lm

SRC = main.c bitboard.c position.c movegen.c evaluation.c search.c tournament.c
OBJ = $(SRC:.c=.o)
TARGET = chess_engine

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

distclean:
	$(MAKE) clean
	rm -rf weights/*.bin

.PHONY: all clean distclean
