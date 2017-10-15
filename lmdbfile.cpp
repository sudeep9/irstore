
#include <iostream>
#include <algorithm>
#include <string.h>
#include <stdint.h>

#include <lmdbfile.h>

using namespace std;

IRErrorPtr LmdbFile::get_page_size(uint32_t *pagesz) const {
    if(!m_opened) {
        return make_err(ErrorType::irstore, 1, "File not opened yet");
    }

    MDB_stat s;
    auto rc = mdb_env_stat(m_mdb_env, &s);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to get db stat");
    }

    *pagesz = s.ms_psize;

    return nullptr;
}

IRErrorPtr LmdbFile::open(const std::string& path) {
    if(m_opened){
        return make_err(ErrorType::irstore, 1, "File already opened: ", path);
    }
    m_path = path;

    auto rc = mdb_env_create(&m_mdb_env);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to create environment");
    }

    rc = mdb_env_open(m_mdb_env, path.c_str(), MDB_WRITEMAP | MDB_NOSUBDIR, 0664);
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

    *bytes_read = min(data.mv_size, static_cast<size_t>(relative_off + readsz)) - relative_off;
    memcpy(buf, static_cast<uint8_t*>(data.mv_data) + relative_off, *bytes_read);

    rc = mdb_txn_commit(txn);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to commit transaction");
    }

    return nullptr;
}

IRErrorPtr LmdbFile::write_block(uint64_t off, uint64_t write_off, MDB_cursor* cur, void* buf, size_t buflen) {
    MDB_val key;
    MDB_val data;

    //off is aligned_off
    key.mv_size = sizeof(off);
    key.mv_data = &off;

    auto rc = mdb_cursor_get(cur, &key, &data, MDB_SET);

    switch (rc) {
    case MDB_NOTFOUND:
        data.mv_size = buflen;
        data.mv_data = buf;
        break;

    case MDB_SUCCESS:
        if( (write_off + buflen) > data.mv_size ) {
            return make_err(ErrorType::irstore, 1, 
                "Attempt to write past (", write_off + buflen,") the data size (",data.mv_size,")");
        }

        memcpy(static_cast<uint8_t*>(data.mv_data) + write_off, buf, buflen);
        break;

    default:
        return make_err(ErrorType::lmdb, rc, "Failed to read data");
    }
    rc = mdb_cursor_put(cur, &key, &data, MDB_CURRENT);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to put data");
    }

    return nullptr;
}

IRErrorPtr LmdbFile::write(uint64_t off, void* buf, size_t buflen) {
    uint64_t aligned_off = off / m_blocksz;
    uint64_t relative_off = off - aligned_off;

    MDB_txn *txn;

    auto rc = mdb_txn_begin(m_mdb_env, nullptr, 0, &txn);
    if(rc) {
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to start transaction");
    }

    MDB_cursor* cur;
    rc = mdb_cursor_open(txn, m_dbi, &cur);
    if(rc) {
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to open cursor");
    }

    auto err = this->write_block(aligned_off, relative_off, cur, buf, buflen);
    mdb_cursor_close(cur);

    if(err) {
        mdb_txn_abort(txn);
        return err;
    }

    rc = mdb_txn_commit(txn);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to commit transaction");
    }
    return nullptr;
}

void LmdbFile::close() {
    mdb_dbi_close(m_mdb_env, m_dbi);
    mdb_env_close(m_mdb_env);
    m_opened = false;
}

LmdbFile::~LmdbFile() {
}