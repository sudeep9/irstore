
#ifndef lmdbfile_h
#define lmdbfile_h

#include <string>

#include <lmdb.h>
#include <irerror.h>


struct LmdbFile {

    LmdbFile(): m_opened(false), m_mdb_env(nullptr){}

    IRErrorPtr open(const std::string& path);

    ~LmdbFile();

private:
    bool m_opened;
    std::string m_path;
    MDB_env *m_mdb_env;
    MDB_dbi m_dbi;
};

#endif