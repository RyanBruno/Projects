VPATH := src/orset/:
VPATH += src/ospc/:
VPATH += src/rpc/:
VPATH += src/sequence/:
VPATH += src/std_unordered_map/:
VPATH += src/unordered_map/:
VPATH += src/xdr_orset/:
VPATH += src/:
VPATH += tests/:

# For reference
#.c  : $(CC) $(CPPFLAGS) $(CFLAGS) -c
#.cpp: $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c
#.o  : $(CC) $(LDFLAGS) n.o $(LOADLIBES) $(LDLIBS)
include_dirs = -I/usr/include/tirpc
CFLAGS = $(include_dirs) $(libs) -Wall -Wextra
LDLIBS = -ltirpc -pthread

all : orset.o 
all : ospc.o
all : rpc.o
all : sequence.o
all : std_unordered_map.o
all : xdr_orset.o
all : config.o
all : node.o
all : init.o

node : orset.o
node : ospc.o
node : rpc.o
node : std_unordered_map.o
node : xdr_orset.o
node : config.o
node : node.o
node : init.o
	$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

orset.o : unordered_map.h
ospc.o : orset.h unordered_map.h
rpc.o : node.h
sequence.o : 
std_unordered_map.o : unordered_map.h
xdr_orset.o : orset.h unordered_map.h
config.o :
node.o : orset.h ospc.h xdr_orset.h config.h
init.o : node.h
test_all.o : test_orset.c test_xdr_orset.c test_ospc.c all

clean :
	rm -f *.o
	rm -f test
	rm -f node
