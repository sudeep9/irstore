
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <thread>
#include <chrono>

#include <doctest.h>

#include <irerror.h>

using namespace std;
using namespace std::chrono;

IRErrorPtr f1() {
    return make_err(ErrorType::irstore, 1, "test error");
}

IRErrorPtr f2() {
    Try(f1());
    return nullptr;
}

IRErrorPtr f3() {
    Try(f2());
    return nullptr;
}

TEST_CASE("test 1") {
    for(int i=0;i<10;i++) {
        auto err = f3();
    }
}