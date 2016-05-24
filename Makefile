all: toebot
CXXFLAGS=-Wall -O3 --std=c++1y -Werror -ggdb3 -D_LOCAL

OBJECTS = main.o test.o board.o util.o ai.o random.o hash.o search_tree_printer.o hash_table.o score_table.o opening_table.o interruption.o line_reader.o generated_opening_table.o cmd_args.o flags.o

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = g++ $(DEPFLAGS) $(CXXFLAGS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d
	@echo "Compiling $<..."
	@$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	@$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJECTS)))

interruption.o: interruption.h interruption.cpp

line_reader.o: line_reader.h line_reader.cpp

toebot: $(OBJECTS)
	g++ $(OBJECTS) $(LDLIBS) $(CXXFLAGS) -o $@

clean:
	rm -f *.o toebot

