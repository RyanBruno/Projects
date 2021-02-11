
include_dirs =
#CFLAGS = $(include_dirs) $(libs) -Wall -Wextra
LDLIBS = -lmicrohttpd

former : former.o
	$(CXX) -ggdb $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

former.o : former.c

clean :
	rm -f *.o
	rm -f former
