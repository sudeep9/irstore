
CC=g++
AR=ar
CXXFLAGS= -c --std=c++14 -I.

%.o:%.cpp
	$(CC) $(CXXFLAGS) $^ -o $@

artifacts= libirstore.a storetest

libirstore_deps=store.o

all: $(artifacts)

libirstore.a: $(libirstore_deps)
	$(AR) rs $@ $^

storetest: storetest.o libirstore.a
	$(CC) $^ libirstore.a -o $@

clean:
	rm -fR *.o
	rm -fR $(artifacts)
