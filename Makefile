CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g

TARGET = xiv2gen2
SRC = main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.o
	$(CC) main.o -o $(TARGET) -lcurl -loath

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f main.o $(TARGET)