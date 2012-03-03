CC=gcc
CFLAGS=-W -Wall -pedantic -g
LDFLAGS=-g -lz
EXEC=mca-explorer
SRC= $(wildcard src/*.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

mca-explorer: $(OBJ)
	$(CC) -o mca-explorer $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm $(OBJ)
    
mrproper: clean
	rm $(EXEC)
