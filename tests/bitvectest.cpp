
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <bitvec.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <string.h>

using namespace std;

void test();

void test() {
    auto len = static_cast<size_t>(1024);
    BitVec b;
    b.init(len);

    for(size_t i = 0; i<b.get_len(); i++) {
        if(i % 2 != 0) {
            b.set(i);
        }
    }

    unique_ptr<unsigned char[]> buf(new unsigned char[b.get_byte_len()]);

    for(size_t i = 0; i<b.get_byte_len(); i++) {
        buf[i] = 0x55;
    }

    auto r = memcmp(b.get_data(), buf.get(), b.get_byte_len());
    cout<<r<<endl;
    {
        ofstream f("abc.dat");
        BitVec::marshal(f, b);
    }

    {
        BitVec b2;

        ifstream f("abc.dat");
        BitVec::unmarshal(f, b2);
        auto r2 = memcmp(b2.get_data(), buf.get(), b2.get_byte_len());
        cout<<r2<<endl;
    }
}

TEST_CASE("bit-vector") {
    test();
} 
