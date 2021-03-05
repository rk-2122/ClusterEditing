CC = /usr/bin/g++ 
CFLAGS = -I${HOME}/local/include -std=c++11 -O3 -Wall


# COMPILE
all: main.o graph.o solver.o
	$(CC) -o clusteredit main.o graph.o solver.o -lm

main.o: main.cpp main.h graph.h solver.h
	$(CC) $(CFLAGS) -c main.cpp 

graph.o: graph.cpp main.h graph.h
	$(CC) $(CFLAGS) -c graph.cpp

solver.o: solver.cpp solver.h main.h graph.h
	$(CC) $(CFLAGS) -c solver.cpp

clean:
	$(RM) -f  *.o

test: all
	./clusteredit

