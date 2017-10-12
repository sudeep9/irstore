
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

    auto buf = new unsigned char[1];

    for(size_t i = 0; i<(len/8); i++) {
        buf[i] = 85;
    }

    auto r = memcmp(b.get_data(), buf, b.get_len()/8);
    cout<<r<<endl;
    {
        ofstream f("abc.dat");
        BitVec::marshal(f, b);
    }

    {
        BitVec b2;

        ifstream f("abc.dat");
        BitVec::unmarshal(f, b2);
    }
}

/*
void test() {
    BitVec b(1024);
    b.set(0x30);
    cout<<b.get(0x30)<<endl;

    ofstream f("abc.dat");
    auto data = reinterpret_cast<const char*>(b.get_data());
    f.write(data, static_cast<long>(b.get_len()));
}
*/

int main() {
    test();
    return 0;
}
