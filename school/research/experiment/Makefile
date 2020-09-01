VPATH = src:src/std_unordered_map:src/orset:src/xdr_orset:tests/:../include

CFLAGS = $(include_dirs) $(libs)
include_dirs = -I/usr/include/tirpc
LDLIBS = -ltirpc

all: orset.o xdr_orset.o std_unordered_map.o

test : test_all.o orset.o std_unordered_map.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

orset.o : unordered_map/unordered_map.h
xdr_orset.o : orset.c
std_unordered_map.o : unordered_map/unordered_map.h
test_all.o : test_orset.c test_xdr_orset.c unordered_map/unordered_map.h all

clean :
	rm -f *.o
	rm -f test
