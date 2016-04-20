all: toebot
CXXFLAGS=-Wall -O3 --std=c++1y -Werror -ggdb3

OBJECTS = main.o test.o board.o util.o ai.o random.o hash.o search_tree_printer.o

test.o: util.h test.cpp

main.o: util.h main.cpp board.h ai.h random.h

board.o: board.cpp board.h

util.o: util.cpp util.h

ai.o: ai.cpp ai.h search_tree_printer.h

search_tree_printer.o: search_tree_printer.cpp search_tree_printer.h board.h

hash.o: hash.cpp hash.h

search_tree_printer.o: search_tree_printer.cpp search_tree_printer.h board.h hash.h

toebot: $(OBJECTS)
	g++ $(OBJECTS) $(LDLIBS) -O3 -o toebot

clean:
	rm *.o toebot
