CC = /usr/bin/g++ 
CFLAGS = -I${HOME}/local/include -std=c++11 -g3 -Wall 
CPLEX = -I/Applications/CPLEX_Studio1210/cplex/include/ -I/Applications/CPLEX_Studio1210/concert/include/ -DIL_STD
CPLEXLIB = -L/Applications/CPLEX_Studio1210/cplex/lib/x86-64_osx/static_pic -L/Applications/CPLEX_Studio1210/concert/lib/x86-64_osx/static_pic 


# COMPILE
all: main.o graph.o solver.o reduction.o
	$(CC) $(CPLEXLIB) -o clusteredit main.o graph.o solver.o reduction.o -lm -lilocplex -lcplex -lconcert

main.o: main.cpp main.h graph.h solver.h
	$(CC) $(CFLAGS) -c main.cpp 

graph.o: graph.cpp main.h graph.h
	$(CC) $(CFLAGS) -c graph.cpp

solver.o: solver.cpp solver.h main.h graph.h
	$(CC) $(CFLAGS) $(CPLEX) -c solver.cpp 

reduction.o: reduction.cpp reduction.h main.h graph.h
	$(CC) $(CFLAGS) -c reduction.cpp

clean:
	$(RM) -f  *.o

test: all
	./clusteredit

