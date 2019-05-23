INTERM=Task.cpp Task.h main.cpp
PROGRAM=./main
CFLAGS=-g -Wall -O3
CC = g++
RM = rm -f


all: $(PROGRAM)

$(PROGRAM): $(INTERM)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(INTERM)

clean:
	$(RM) $(PROGRAM)
