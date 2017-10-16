
#ifndef patch_h
#define patch_h

#include <vector>
#include <tuple>

typedef std::tuple<
    int64_t,    // aligned offset
    int64_t,    // relative offset w.r.t to block 
    int64_t,    // relative offset w.r.t to buffer
    int64_t     // buf len
> patch_offset_t;

void split_offsets(size_t blocksz, int64_t off, size_t buflen, std::vector<patch_offset_t>&);


#endif