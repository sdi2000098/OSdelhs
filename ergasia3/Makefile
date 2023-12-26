CC = g++
CFLAGS = -Wall -Wextra -pedantic

all: main reader writer

main: Init.cpp
	$(CC) $(CFLAGS) -o main Init.cpp

reader: reader.cpp
	$(CC) $(CFLAGS) -o reader reader.cpp

writer: writer.cpp
	$(CC) $(CFLAGS) -o writer writer.cpp

clean:
	rm -f main reader writer
