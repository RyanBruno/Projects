VPATH = src:src/std_unordered_map:src/orset:src/xdr_orset:tests/:src/ospc/:src/sequence/:src/osec/:../include

CFLAGS = $(include_dirs) $(libs)
include_dirs = -I/usr/include/tirpc
LDLIBS = -ltirpc -pthread

all: orset.o xdr_orset.o std_unordered_map.o ospc.o node.o utils.o demo.o sequence.o osec.o

node : node.o orset.o std_unordered_map.o ospc.o xdr_orset.o utils.o demo.o osec.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

test : test_all.o orset.o std_unordered_map.o ospc.o xdr_orset.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

orset.o : unordered_map/unordered_map.h
xdr_orset.o : orset.h unordered_map/unordered_map.h
std_unordered_map.o : unordered_map/unordered_map.h
ospc.o : orset.h unordered_map/unordered_map.h
node.o : utils.h config.h ospc.h xdr_orset.h orset.h unordered_map/unordered_map.h
utils.o : orset.h unordered_map/unordered_map.h
demo.o : node.h orset.h unordered_map/unordered_map.h
sequence.o : 
osec.o : ospc.h orset.h unordered_map/unordered_map.h
test_all.o : test_orset.c test_xdr_orset.c test_ospc.c unordered_map/unordered_map.h all

clean :
	rm -f *.o
	rm -f test
	rm -f node
