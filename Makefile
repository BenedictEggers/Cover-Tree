FLAGS=-Wall -O3 -ffast-math -funroll-loops

all: test stats

Cover_Tree_Point.o: Cover_Tree_Point.h Cover_Tree_Point.cc
	g++ -c $(FLAGS) Cover_Tree_Point.cc

test: test.cc Cover_Tree.h Cover_Tree_Point.o
	g++ $(FLAGS) -o test test.cc Cover_Tree_Point.o

stats: statistics.cc Cover_Tree.h Cover_Tree_Point.o
	g++ $(FLAGS) -o statistics statistics.cc Cover_Tree_Point.o

clean:
	rm -f *.o test
