all: toebot
CXXFLAGS=-Wall -O3 --std=c++1y

test.o: util.h test.cpp

main.o: util.h main.cpp

toebot: main.o test.o
	g++ main.o test.o -O3 -o toebot

clean:
	rm *.o toebot
