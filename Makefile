all: toebot
CXXFLAGS=-Wall -O3 --std=c++1y -Werror -ggdb3 -D_LOCAL

OBJECTS = main.o test.o board.o util.o ai.o random.o hash.o search_tree_printer.o hash_table.o score_table.o opening_table.o

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

toebot: $(OBJECTS)
	g++ $(OBJECTS) $(LDLIBS) $(CXXFLAGS) -o $@

clean:
	rm -f *.o toebot

