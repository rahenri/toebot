all: toebot
CXXFLAGS=-Wall -O3 --std=c++1y -Werror

OBJECTS = main.o test.o board.o util.o ai.o random.o hash.o

test.o: util.h test.cpp

main.o: util.h main.cpp board.h ai.h random.h

board.o: board.cpp board.h

util.o: util.cpp util.h

ai.o: ai.cpp ai.h

hash.o: hash.cpp hash.h

toebot: $(OBJECTS)
	g++ $(OBJECTS) -O3 -o toebot

clean:
	rm *.o toebot
