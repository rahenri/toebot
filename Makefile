all: toebot
CXXFLAGS=-Wall -O3 --std=c++1y

test.o: util.h test.cpp

main.o: util.h main.cpp board.h ai.h

board.o: board.cpp board.h

util.o: util.cpp util.h

ai.o: ai.cpp ai.h

toebot: main.o test.o board.o util.o ai.o
	g++ main.o test.o board.o util.o ai.o -O3 -o toebot

clean:
	rm *.o toebot
