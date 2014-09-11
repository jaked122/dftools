CCOPT=-std=c++11 -g
CC=g++
all: circle sierpinsky htree csv.h concentricity


circle: circle.cpp circle.h csv.h 
	$(CC) $(CCOPT) circle.cpp -o circle

sierpinsky: sierpinsky.cpp csv.h
	$(CC) $(CCOPT) sierpinsky.cpp -o sierpinsky

htree: htree.cpp csv.h
	$(CC) $(CCOPT) htree.cpp -o htree

concentricity:concentricity.cpp circle.h csv.h
	$(CC) $(CCOPT) concentricity.cpp -o concentricity
