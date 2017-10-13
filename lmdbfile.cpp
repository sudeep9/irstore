
#include <iostream>

#include <lmdbfile.h>

using namespace std;


IRErrorPtr LmdbFile::open(const std::string& path) {
    if(m_opened){
        return make_err(ErrorType::irstore, 1, "File already opened: ", path);
    }
    m_path = path;

    auto rc = mdb_env_create(&m_mdb_env);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to create environment");
    }

    rc = mdb_env_open(m_mdb_env, path.c_str(), MDB_NOSUBDIR, 0664);
    if(rc) {
        mdb_env_close(m_mdb_env);
        return make_err(ErrorType::lmdb, rc, "Failed to open env");
    }

    MDB_txn *txn;

    rc = mdb_txn_begin(m_mdb_env, nullptr, 0, &txn);
    if(rc) {
        mdb_env_close(m_mdb_env);
        return make_err(ErrorType::lmdb, rc, "Failed to start transaction");
    }
    rc = mdb_dbi_open(txn, NULL, 0, &m_dbi);
    if(rc) {
        mdb_env_close(m_mdb_env);
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to open database");
    }

    m_opened = true;

    return nullptr;
}

LmdbFile::~LmdbFile() {
    if(!m_mdb_env)
        return;

    mdb_env_close(m_mdb_env);

}