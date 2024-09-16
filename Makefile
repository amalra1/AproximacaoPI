CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm
TARGET = pi

all: $(TARGET)

$(TARGET): pi.o
	$(CC) -o $(TARGET) pi.o $(CFLAGS)

pi.o: pi.c
	$(CC) -c pi.c $(CFLAGS)

clean:
	rm -f pi.o

purge: clean
	rm -f $(TARGET)
