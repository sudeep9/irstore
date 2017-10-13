
#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>

#include <lmdbfile.h>

using namespace std;

IRErrorPtr test();

IRErrorPtr test() {
    LmdbFile f(4096);
    string path = string(getenv("HOME")) + "/work/irdata/f1.dat";

    Try(f.open(path));

    const char* buf = "hello world";
    uint64_t off = 0;
    char rdbuf[100];
    size_t bytes_read;

    Try(f.write(off, reinterpret_cast<void*>(const_cast<char*>(buf)), strlen(buf) + 1));
    Try(f.read(off, reinterpret_cast<void*>(const_cast<char*>(rdbuf)), 100, &bytes_read));
    cout<<rdbuf<<endl;

    Try(f.write(6, reinterpret_cast<void*>(const_cast<char*>("12345")), 5));


    Try(f.read(10, reinterpret_cast<void*>(const_cast<char*>(rdbuf)), 100, &bytes_read));
    cout<<bytes_read<<": "<<rdbuf<<endl;

    return nullptr;
}

int main() {
    auto err = test();
    if(err) {
        cout<<*err<<endl;
    }
    return 0;
}