
#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <chrono>

#include <lmdbfile.h>

using namespace std;
using namespace std::chrono;

IRErrorPtr test();

IRErrorPtr test() {
    LmdbFile f(4096);
    string path = string(getenv("HOME")) + "/work/irdata/f1.dat";

    Try(f.open(path));

    uint32_t pagesz;

    Try(f.get_page_size(&pagesz));
    cout<<"page_size = "<<pagesz<<endl;

    const char* buf = "hello world";
    uint64_t off = 0;
    char rdbuf[100];
    size_t bytes_read;

    auto start = steady_clock::now();
    for(int i=0; i<1000; i++) {
        Try(f.write(off, reinterpret_cast<void*>(const_cast<char*>(buf)), strlen(buf) + 1));
        //Try(f.read(off, reinterpret_cast<void*>(const_cast<char*>(rdbuf)), 100, &bytes_read));
        //cout<<rdbuf<<endl;

        //Try(f.write(6, reinterpret_cast<void*>(const_cast<char*>("*****")), 5));
        //Try(f.read(0, reinterpret_cast<void*>(const_cast<char*>(rdbuf)), 100, &bytes_read));
        //cout<<bytes_read<<": "<<rdbuf<<endl;
    }
    auto end = steady_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(end - start);
    cout<<"time elapsed = "<<time_span.count()<<" secs"<<endl;
    f.close();

    return nullptr;
}

int main() {
    auto err = test();
    if(err) {
        cout<<*err<<endl;
    }
    return 0;
}