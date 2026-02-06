.PHONY : all run clean 

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

NAMEEXE := main
OBJFILES := main.o cat.o dog.o

all: $(NAMEEXE)

run: $(NAMEEXE)
	./$(NAMEEXE) 

$(NAMEEXE) : $(OBJFILES)
	$(CC) $(OBJFILES) -o $@ 

%.o : %.c
	$(CC) $< -c -o $@ $(CFLAGS)

clean: 
	rm $(NAMEEXE) *.o