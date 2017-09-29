
CC=gcc
CXX=g++
AR=ar
CXXWARNINGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
-Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
-Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 \
-Wswitch-default -Wundef -Werror -Wno-unused

CXXFLAGS=-c -std=c++1y -fPIC $(CXXWARNINGS) -I.
CFLAGS=-c -fPIC -I.
LDFLAGS=-L.

CXXSTDLIB=$(shell $(CXX) $(CXXFLAGS) -print-file-name=libstdc++.a)


%.cpp.o:%.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) $^ -o $@


artifacts= libirstore.a storetest

libirstore_obj_deps=store.cpp.o irerror.cpp.o file.cpp.o
libirstore_deps=$(libirstore_obj_deps) irerror.h irstore.h file.h

all: $(artifacts)

#libirstore.so: $(libirstore_obj_deps)
#	$(CXX) --shared -o $@ -static-libstdc++ -static-libgcc  $^

libirstore.a: $(libirstore_obj_deps)
	$(AR) rsT $@ $^ $(CXXSTDLIB)

storetest_obj_deps= storetest.o 
#storetest_deps=$(storetest_obj_deps) file.h irerror.h irstore.h libirstore.so
storetest_deps=$(storetest_obj_deps) file.h irerror.h irstore.h libirstore.a

storetest: $(storetest_deps)
#	$(CC) $(storetest_obj_deps) $(LDFLAGS) libirstore.so -static -o $@
	$(CC) $(storetest_obj_deps) $(LDFLAGS) libirstore.a -static -o $@

clean:
	rm -fR *.o
	rm -fR $(artifacts)
