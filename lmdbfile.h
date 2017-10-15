
#ifndef lmdbfile_h
#define lmdbfile_h

#include <string>
#include <stdint.h>

#include <lmdb.h>
#include <irerror.h>


struct LmdbFile {

    LmdbFile(uint32_t blocksz): m_opened(false), m_mdb_env(nullptr), m_blocksz(blocksz){
        m_write_off = 0;
        m_read_off = 0;
    }

    IRErrorPtr get_page_size(uint32_t *pagesz) const;
    IRErrorPtr open(const std::string& path);
    IRErrorPtr write(uint64_t off, void* buf, size_t buflen);
    IRErrorPtr read(uint64_t off, void* buf, size_t readsz, size_t* bytes_read);
    void close();

    ~LmdbFile();

private:
    bool m_opened;
    std::string m_path;
    MDB_env *m_mdb_env;
    MDB_dbi m_dbi;
    uint32_t m_blocksz;
    uint64_t m_write_off;
    uint64_t m_read_off;
};

#endif