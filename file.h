

#ifndef __file_h
#define __file_h

#include <vector>

#include <store.h>

class File {
public:
    size_t write(uint64_t off, char* buf);
    
private:
    uint32_t m_blocksz;
    string m_filename;
    uint64_t m_filesize;
    vector<bool> m_bitvec;
};

#endif