
CC=gcc
CXX=g++
AR=ar
CXXFLAGS=-c -fPIC -I.
CFLAGS=-c -fPIC -I.
LDFLAGS=-L.


%.cpp.o:%.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) $^ -o $@


artifacts= libirstore.so storetest

libirstore_obj_deps=store.cpp.o irerror.cpp.o
libirstore_deps=$(libirstore_obj_deps) irerror.h irstore.h

all: $(artifacts)

libirstore.so: $(libirstore_obj_deps)
	$(CXX) --shared -o $@ $^

storetest: storetest.o
	$(CC) $^ $(LDFLAGS) -lirstore -o $@

clean:
	rm -fR *.o
	rm -fR $(artifacts)