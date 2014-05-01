FLAGS=-Wall -O3 -ffast-math -funroll-loops

all:	test

Cover_Tree_Point.o: Cover_Tree_Point.h Cover_Tree_Point.cc
	g++ -c $(FLAGS) Cover_Tree_Point.cc

Cover_Tree.o: Cover_Tree.h Cover_Tree.cc
	g++ -c $(FLAGS) Cover_Tree.cc

test.o: test.cc Cover_Tree.h Cover_Tree_Point.h
	g++ -c $(FLAGS) test.cc

test: test.o Cover_Tree_Point.o Cover_Tree.o test.o
	g++ $(FLAGS) -o test Cover_Tree_Point.o Cover_Tree.o test.o

clean:
	rm -f *.o test