CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm -lgmp
TARGET = pi

all: $(TARGET)

$(TARGET): pi.o
	$(CC) -o $(TARGET) pi.o $(CFLAGS) $(LDFLAGS)

pi.o: pi.c
	$(CC) -c pi.c $(CFLAGS)

clean:
	rm -f pi.o

purge: clean
	rm -f $(TARGET)
