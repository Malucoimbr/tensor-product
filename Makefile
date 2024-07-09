CC = gcc
CFLAGS = -Wall -pthread

tensor: tensor.c
	$(CC) $(CFLAGS) -o tensor tensor.c

clean:
	rm -f tensor
