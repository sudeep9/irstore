
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

    const char* hello_str = "hello world";
    const char* num_str = "1234567890";
    const uint8_t* buf = reinterpret_cast<const uint8_t*>(hello_str);
    const uint8_t* num_buf = reinterpret_cast<const uint8_t*>(num_str);
    uint64_t off = 0;
    uint8_t rdbuf[100];
    size_t bytes_read;

    auto start = steady_clock::now();
    for(int i=0; i<100000; i++) {
        Try(f.write(off, buf, strlen(hello_str) + 1));
        //Try(f.read(off, rdbuf, 100, &bytes_read));
        //cout<<rdbuf<<endl;

        Try(f.write(6, num_buf, i % 12));
        Try(f.read(0, rdbuf, 100, &bytes_read));
        //rdbuf[bytes_read] = '\0';
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