#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <patch.h>

using namespace std;

/*
typedef std::tuple<
    int64_t,    // aligned offset
    int64_t,    // relative offset w.r.t to block 
    int64_t,    // relative offset w.r.t to buffer
    int64_t     // buf len
> patch_offset_t;
*/

TESTCASE("within block") {
    size_t blocksz = 10;
    int64_t off = 3;
    size_t buflen = 4;

    vector<patch_offset_t> v(5);
    vector<patch_offset_t> expected = {
        patch_offset_t(0, 3, 0, 4),
    };

    split_offsets(blocksz, off, buflen, v);

    CHECK(v == expected);

}