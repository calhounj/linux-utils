CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -g

SRC_DIR = src
BIN_DIR = bin

SRCS := $(wildcard $(SRC_DIR)/*.c)
BINS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRCS))

all: $(BINS)

$(BIN_DIR)/%: $(SRC_DIR)/%.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN_DIR)
