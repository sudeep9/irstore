
#include <patch.h>
#include <algorithm>

using namespace std;

void split_offsets(size_t blocksz, int64_t off, size_t buflen, std::vector<patch_offset_t>& offset_info) {
    int64_t aligned_off = (off / blocksz) * blocksz;
    int64_t relative_off = off - aligned_off;
    int64_t end_aligned_off = ((off + buflen) / blocksz) * blocksz;

    auto buf_start_off = 0;
    for(;aligned_off <= end_aligned_off; aligned_off += blocksz) {
        auto cut_len = min(static_cast<int64_t>(relative_off + buflen - buf_start_off), static_cast<int64_t>(blocksz)) - relative_off;
        if(!cut_len) continue;

        offset_info.push_back(make_tuple(aligned_off, relative_off, buf_start_off, cut_len));

        buf_start_off += cut_len;
        relative_off = 0;
    }

}
