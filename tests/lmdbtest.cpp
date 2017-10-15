
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <chrono>

#include <lmdbfile.h>
#include <spdlog/spdlog.h>

using namespace std;
using namespace std::chrono;
using namespace spdlog;

IRErrorPtr test();
void setup_log();

IRErrorPtr test() {
    LmdbFile f(5);
    string path("./f1.dat");

    Try(f.open(path));

    uint32_t pagesz;

    Try(f.get_page_size(&pagesz));
    cout<<"page_size = "<<pagesz<<endl;

    const char* hello_str = "hello world";
    const uint8_t* buf = reinterpret_cast<const uint8_t*>(hello_str);
    uint64_t off = 0;
    uint8_t rdbuf[100];
    size_t bytes_read;

    auto start = steady_clock::now();
    for(int i=0; i<10; i++) {
        Try(f.write(off, buf, strlen(hello_str) + 1));
        //Try(f.read(off, rdbuf, 100, &bytes_read));
        //cout<<rdbuf<<endl;

        //Try(f.write(6, num_buf, i % 12));
        Try(f.read(0, rdbuf, 5, &bytes_read));
        rdbuf[bytes_read] = '\0';
        cout<<bytes_read<<": "<<rdbuf<<endl;
        Try(f.read(5, rdbuf, 5, &bytes_read));
        rdbuf[bytes_read] = '\0';
        cout<<bytes_read<<": "<<rdbuf<<endl;
    }
    auto end = steady_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(end - start);
    cout<<"time elapsed = "<<time_span.count()<<" secs"<<endl;
    f.close();

    return nullptr;
}

void setup_log() {
    auto log = spdlog::basic_logger_mt("irstore", "lmdbtest.log", true);
    log->set_level(spdlog::level::trace); 
    log->set_pattern("[%d:%m:%C %H:%M:%S] %L: %v");
}

TEST_CASE("root") {
    setup_log();

    SUBCASE("read-write") {
        test();
    }
}

/*
int main() {
    setup_log();

    auto err = test();
    if(err) {
        cout<<*err<<endl;
    }
    return 0;
}
*/