PROJDIRS := src

SRCFILES := $(shell find $(PROJDIRS) -type f -name "*.c*")
HDRFILES := $(shell find $(PROJDIRS) -type f -name "*.h")
OBJFILES := $(SRCFILES:.c=.o)
OBJFILES := $(OBJFILES:.cpp=.o)
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
CFLAGS = $(include_dirs) $(libs) -Wall -Wextra
CXXFLAGS = $(CFLAGS)
LDLIBS = -ltirpc -pthread

.PHONY: all clean todo

all: node

clean :
	-@$(RM) $(wildcard $(OBJFILES) $(DEPFILES) test node)

todo:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

%.o: %.cpp Makefile
	@$(CXX) -ggdb $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -DTEST -c $< -o $@

%.o: %.c Makefile
	@$(CC) -ggdb $(CFLAGS) -MMD -MP -DTEST -c $< -o $@

node: $(filter src/%,$(OBJFILES))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
