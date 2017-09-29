
CC=gcc
CXX=g++
AR=ar
CXXFLAGS=-c --std=c++14 -fPIC -I.
CFLAGS=-c -fPIC -I.
LDFLAGS=-L.


%.cpp.o:%.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) $^ -o $@


artifacts= libirstore.so storetest

libirstore_obj_deps=store.cpp.o irerror.cpp.o file.cpp.o
libirstore_deps=$(libirstore_obj_deps) irerror.h irstore.h file.h

all: $(artifacts)

libirstore.so: $(libirstore_obj_deps)
	$(CXX) --shared -o $@ $^

storetest_obj_deps= storetest.o 
storetest_deps=$(storetest_obj_deps) file.h irerror.h irstore.h libirstore.so

storetest: $(storetest_deps)
	$(CC) $(storetest_obj_deps) $(LDFLAGS) -lirstore -o $@

clean:
	rm -fR *.o
	rm -fR $(artifacts)
