all: toebot

CXXFLAGS=-Wall -O0 --std=c++1y -Werror -D_LOCAL -ggdb3

OBJECTS = test.o board.o util.o ai.o random.o hash.o search_tree_printer.o hash_table.o score_table.o opening_table.o interruption.o line_reader.o generated_opening_table.o cmd_args.o flags.o model.o
MAIN_OBJECT = main.o

TESTS = board_test

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

INPUT_DATA = $(shell find history/ -name '*.txt' 2> /dev/null)
OUTPUT_DATA = $(patsubst %.txt,%.npz,$(INPUT_DATA))

INPUT_OPENING = $(shell find spider/ -name '*.in')
OUTPUT_OPENING = $(patsubst %.in,%.out,$(INPUT_OPENING))

data: $(OUTPUT_DATA)

clean-data:
	rm -f history/*.npz

%.npz: %.txt learning/compile_data.py
	@echo "Compiling data $<"
	@./learning/compile_data.py $< --output $@

opening: $(OUTPUT_OPENING)

clean-opening:
	rm -f $(OUTPUT_OPENING)

%.out: %.in
	@echo "Generating $@"
	@./toebot --enable-opening-table --hash-size 300000017 < $< > $@ 2> $@.stderr

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d
	@echo "Compiling $<..."
	@$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	@$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJECTS)))
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(MAIN_OBJECT)))
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(board_test.o)))

interruption.o: interruption.h interruption.cpp

line_reader.o: line_reader.h line_reader.cpp

toebot: $(OBJECTS) $(MAIN_OBJECT)
	@echo "Linking $@..."
	@$(CXX) $^ $(LDLIBS) $(CXXFLAGS) -o $@

board_test: board_test.o $(OBJECTS)
	@echo "Linking $@..."
	@$(CXX) $^ $(LDLIBS) $(CXXFLAGS) -o $@
	@echo "Running $@..."
	@./board_test
	@rm board_test

tests: board_test


clean:
	rm -f *.o toebot

