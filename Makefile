#simpleserver make file
CC = clang
CFLAGS = -g -Wall -Werror -Wno-unused-variable

TARGET = server

OBJS = server.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o $(TARGET)