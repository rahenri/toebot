all: toebot
CXXFLAGS=-Wall -O3 --std=c++1y -Werror -ggdb3 -D_LOCAL

OBJECTS = main.o test.o board.o util.o ai.o random.o hash.o search_tree_printer.o hash_table.o score_table.o opening_table.o

test.o: util.h test.cpp

main.o: util.h main.cpp board.h ai.h random.h score_table.h flags.h

board.o: board.cpp board.h score_table.h

util.o: util.cpp util.h

ai.o: ai.cpp ai.h search_tree_printer.h board.h flags.h

hash.o: hash.cpp hash.h

search_tree_printer.o: search_tree_printer.cpp search_tree_printer.h board.h hash.h

hash_table.o: hash_table.cpp hash_table.h

score_table.o: score_table.cpp score_table.h board.h

opening_table.o: opening_table.h opening_table.cpp

toebot: $(OBJECTS)
	g++ $(OBJECTS) $(LDLIBS) -O3 -o toebot

clean:
	rm *.o toebot
