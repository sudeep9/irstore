
#include <iostream>
#include <algorithm>
#include <string.h>
#include <stdint.h>

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
        return make_err(ErrorType::lmdb, rc, "Failed to open env, db = ", path);
    }

    MDB_txn *txn;

    rc = mdb_txn_begin(m_mdb_env, nullptr, 0, &txn);
    if(rc) {
        mdb_env_close(m_mdb_env);
        return make_err(ErrorType::lmdb, rc, "Failed to start transaction");
    }

    rc = mdb_dbi_open(txn, nullptr, 0, &m_dbi);
    if(rc) {
        mdb_txn_abort(txn);
        mdb_env_close(m_mdb_env);
        return make_err(ErrorType::lmdb, rc, "Failed to open database");
    }

    rc = mdb_txn_commit(txn);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to commit transaction");
    }

    m_opened = true;

    return nullptr;
}

IRErrorPtr LmdbFile::read(uint64_t off, void* buf, size_t readsz, size_t* bytes_read) {
    uint64_t aligned_off = off / m_blocksz;
    uint64_t relative_off = off - aligned_off;

    MDB_txn *txn;

    auto rc = mdb_txn_begin(m_mdb_env, nullptr, MDB_RDONLY, &txn);
    if(rc) {
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to start transaction");
    }

    MDB_val key;
    key.mv_size = sizeof(uint64_t);
    key.mv_data = &aligned_off;

    MDB_val data;

    rc = mdb_get(txn, m_dbi, &key, &data);
    if(rc) {
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to read data");
    }

    if(relative_off > data.mv_size) {
        mdb_txn_abort(txn);
        return make_err(ErrorType::irstore, 1, "Attempt to read past the data");
    }

    *bytes_read = min(data.mv_size, relative_off + readsz) - relative_off;
    memcpy(buf, static_cast<uint8_t*>(data.mv_data) + relative_off, *bytes_read);

    rc = mdb_txn_commit(txn);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to commit transaction");
    }

    return nullptr;
}

IRErrorPtr LmdbFile::write(uint64_t off, void* buf, size_t buflen) {
    uint64_t aligned_off = off / m_blocksz;
    uint64_t relative_off = off - aligned_off;

    MDB_val key, data;

    key.mv_size = sizeof(aligned_off);
	key.mv_data = &aligned_off;
	//data.mv_size = buflen;
    //data.mv_data = buf;

    MDB_txn *txn;

    auto rc = mdb_txn_begin(m_mdb_env, nullptr, 0, &txn);
    if(rc) {
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to start transaction");
    }

    rc = mdb_get(txn, m_dbi, &key, &data);
    unique_ptr<uint8_t[]> writebuf;

    switch (rc) {
    case MDB_NOTFOUND:
        data.mv_data = buf;
        data.mv_size = buflen;
        break;

    case MDB_SUCCESS:
        if( (relative_off + buflen) > data.mv_size ) {
            return make_err(ErrorType::irstore, 1, "Attempt to write past the data size");
        }

        writebuf = make_unique<uint8_t[]>(data.mv_size);
        memcpy(writebuf.get(), data.mv_data, data.mv_size);
        memcpy(writebuf.get() + relative_off, buf, buflen);
        data.mv_data = writebuf.get();
        break;

    default:
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to read data");
    }

    rc = mdb_put(txn, m_dbi, &key, &data, 0);
    if(rc) {
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to put data");
    }

    rc = mdb_txn_commit(txn);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to commit transaction");
    }

    return nullptr;
}

LmdbFile::~LmdbFile() {
}