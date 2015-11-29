CC := g++
CXX_FLAGS := -Wall -Wno-parentheses -Wno-write-strings \
             -fno-rtti -fno-exceptions -O -fomit-frame-pointer -c

SRCS := $(wildcard *.cc)
ifneq (lex.cc, $(wildcard lex.cc))
SRCS := lex.cc $(SRCS)
endif
ifneq (gram.cc, $(wildcard gram.cc))
SRCS := gram.cc $(SRCS)
endif
OBJS := $(subst .cc,.o,$(SRCS))
TARGET := c99

$(TARGET): $(OBJS)
	make ctags
	$(CC) -o $@ $^

gram.cc: gram.y
	bison -d -t -v $< && mv gram.tab.c gram.cc && mv gram.tab.h gram.h

lex.cc: lex.l
	flex -o$@ $<

%.o: %.cc
	$(CC) $(CXX_FLAGS) -o $@ $<

clean:
	@rm -f tags $(TARGET) *.o .dep gram.output gram.cc gram.h lex.cc

run:
	@./$(TARGET) -O <bench/8q.c

ctags:
	@ctags --c++-kinds=+p --fields=+iaS --extra=+q *.h *.cc

back:
	cd .. && tar zcvf ./back/$(TARGET)-`date  +%m%d-%H%M`.tgz $(TARGET)

count:
	@wc -l *.cc *.h gram.y lex.l

dep: gram.cc lex.cc
	@set -e 
	@rm -f .dep
	@echo $(SRCS) | xargs -n 1 cpp -MM >>.dep

ifeq (.dep, $(wildcard .dep))
include .dep	
endif
