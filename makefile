CC = gcc
CFLAGS = -I. -Isrc
TARGET = bin/ckester

.PHONY: bin/ckester

all: $(TARGET)

$(TARGET): main.c
	mkdir -p bin
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -rf bin build
