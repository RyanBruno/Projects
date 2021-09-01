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

# Actual Targets
all: orset liborset.so test web ppm ppm2
clean : clean_web clean_orset clean_shellbridge clean_ppm clean_ppm2
	-@$(RM) $(wildcard $(OBJFILES) $(DEPFILES) $(PCHDRS))

# C Building
# For reference
#.c  : $(CC) $(CPPFLAGS) $(CFLAGS) -c
#.cpp: $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c
#.o  : $(CC) $(LDFLAGS) n.o $(LOADLIBES) $(LDLIBS)
include_dirs = -I/usr/include/tirpc
CFLAGS = $(include_dirs) $(libs) -fPIC -fdiagnostics-color=always
#-Wall -Wextra
CXXFLAGS = $(CFLAGS)
LDLIBS = -ltirpc -pthread -lmicrohttpd -ldl

todo:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

headers: $(PCHDRS)

%.gch: % Makefile
	@$(CXX) -ggdb $(CFLAGS) -c $< -o $@

%.o: %.cpp Makefile# headers
	@$(CXX) -ggdb $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -DTEST -c $< -o $@

%.o: %.c Makefile# headers
	@$(CC) -ggdb $(CFLAGS) -MMD -MP -DTEST -c $< -o $@


.PHONY: todo all clean web clean_web clean_ppm clean_ppm2

## PPM

ppm: $(filter src/ppm/%,$(OBJFILES))
	$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

clean_ppm:
	-@$(RM) ppm

hello.so: $(filter src/ppm2/%,$(OBJFILES))
	$(CXX) -shared -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

## PPM2


clean_ppm2:
	-@$(RM) hello.so

## OrSet

orset: $(filter-out src/orset/tests/%,$(filter src/orset/%,$(OBJFILES)))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

clean_orset:
	-@$(RM) test orset liborset.so

liborset.so: $(filter-out src/orset/init.o,$(filter-out src/orset/tests/%,$(filter src/orset/%,$(OBJFILES))))
	@$(CXX) -shared -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

test: $(filter-out src/orset/init.o,$(filter src/orset/%,$(OBJFILES)))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

## Web
web:
	@cd web/ && npx webpack > /dev/null

clean_web:
	-@$(RM) -rf web/dist/*

WEB_DEST = /srv/http/drop/
dist_web: web
	@mkdir -p $(WEB_DEST)
	@mkdir -p $(WEB_DEST)/static/
	@cp web/dist/* $(WEB_DEST)
	@cp web/static/* $(WEB_DEST)/static/

## Shell Bridge

shellbridge: $(filter src/shellbridge/%,$(OBJFILES))
	@$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

clean_shellbridge:
	-@$(RM) shellbridge

