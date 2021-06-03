PROJDIRS := src tests

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

.PHONY: all clean todo

all: orset liborset.so test

clean :
	-@$(RM) $(wildcard $(OBJFILES) $(DEPFILES) $(PCHDRS) test orset liborset.so)

todo:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

headers: $(PCHDRS)

%.gch: % Makefile
	@$(CXX) -ggdb $(CFLAGS) -c $< -o $@

%.o: %.cpp Makefile headers
	@$(CXX) -ggdb $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -DTEST -c $< -o $@

%.o: %.c Makefile headers
	@$(CC) -ggdb $(CFLAGS) -MMD -MP -DTEST -c $< -o $@

orset: $(filter src/%,$(OBJFILES))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

liborset.so: $(filter-out src/init.o,$(filter src/%,$(OBJFILES)))
	@$(CXX) -shared -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

test: $(filter tests/%,$(OBJFILES)) $(filter-out src/init.o,$(filter src/%,$(OBJFILES)))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
