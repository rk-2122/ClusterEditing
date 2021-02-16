CC = /usr/local/bin/g++ 
CFLAGS = -I${HOME}/local/include -std=c++11 -O3 


# COMPILE
all: main.o graph.o
	$(CC) -o clusteredit main.o graph.o solver.o -lm

main.o: main.cpp main.h graph.h solver.h
	$(CC) -c main.cpp

graph.o: graph.cpp main.h graph.h
	$(CC) -c graph.cpp

solver.o: solver.cpp solver.h main.h graph.h
	$(CC) -c solver.cpp

clean:
	$(RM) -f  *.o

test: all
	./clusteredit

