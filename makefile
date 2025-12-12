CC = gcc
CFLAGS = -I. -Isrc
TARGET = bin/ckester

all: $(TARGET)

$(TARGET): src/main.c
	mkdir -p bin
	$(CC) $(CFLAGS) src/main.c -o $(TARGET)

clean:
	rm -rf bin build
