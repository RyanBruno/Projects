PROJDIRS := src/

SRCFILES := $(shell find $(PROJDIRS) -type f -name "*.c*")
HDRFILES := $(shell find $(PROJDIRS) -type f -name "*.h")
OBJFILES := $(SRCFILES:.c=.o)
OBJFILES := $(OBJFILES:.cpp=.o)
PCHDRS := $(HDRFILES:%=%.gch)
#TSTFILES := $(patsubst %.c,%_t,$(SRCFILES))
DEPFILES    := $(SRCFILES:.c=.d)
DEPFILES    := $(DEPFILES:.cpp=.d)
#TSTDEPFILES := $(patsubst %,%.d,$(TSTFILES))
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES)

# For reference
#.c  : $(CC) $(CPPFLAGS) $(CFLAGS) -c
#.cpp: $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c
#.o  : $(CC) $(LDFLAGS) n.o $(LOADLIBES) $(LDLIBS)
include_dirs = -I/usr/include/tirpc
CFLAGS = $(include_dirs) $(libs) -Wall -Wextra -fPIC
CXXFLAGS = $(CFLAGS)
LDLIBS = -ltirpc -pthread

todo:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

headers: $(PCHDRS)

%.gch: % Makefile
	@$(CXX) -ggdb $(CFLAGS) -c $< -o $@

%.o: %.cpp Makefile headers
	@$(CXX) -ggdb $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -DTEST -c $< -o $@

%.o: %.c Makefile headers
	@$(CC) -ggdb $(CFLAGS) -MMD -MP -DTEST -c $< -o $@


.PHONY: todo all clean web dev_web clean_web

# Actual Targets
all: orset liborset.so test web 
clean : clean_web clean_orset

## OrSet

orset: $(filter src/orset/src/%,$(OBJFILES))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

clean_orset:
	-@$(RM) $(wildcard $(OBJFILES) $(DEPFILES) $(PCHDRS) test orset liborset.so)

liborset.so: $(filter-out src/orset/src/init.o,$(filter src/orset/src/%,$(OBJFILES)))
	@$(CXX) -shared -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

test: $(filter src/orset/tests/%,$(OBJFILES)) $(filter-out src/orset/src/init.o,$(filter src/orset/src/%,$(OBJFILES)))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

## Web
web:
	@cd web/ && npx webpack

dev_web: clean_web web
	@cp web/dist/*.html web/dist/index.html

clean_web:
	-@$(RM) -rf web/dist/*
