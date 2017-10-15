
#include <iostream>
#include <algorithm>
#include <string.h>
#include <stdint.h>

#include <irstorelog.h>
#include <lmdbfile.h>

using namespace std;
using namespace spdlog;

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
    SPDLOG_TRACE(m_log, "lmdb env create for {}", path);

    m_log->info("lmdb {} open", path);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to create environment");
    }

    rc = mdb_env_open(m_mdb_env, path.c_str(), MDB_WRITEMAP | MDB_NOSUBDIR, 0664);
    SPDLOG_TRACE(m_log, "lmdb {} env open", path);
    if(rc) {
        mdb_env_close(m_mdb_env);
        return make_err(ErrorType::lmdb, rc, "Failed to open env, db = ", path);
    }

    MDB_txn *txn;

    rc = mdb_txn_begin(m_mdb_env, nullptr, 0, &txn);
    SPDLOG_TRACE(m_log, "lmdb {} open txn begin rc = {}", path, rc);
    if(rc) {
        mdb_env_close(m_mdb_env);
        return make_err(ErrorType::lmdb, rc, "Failed to start transaction");
    }

    rc = mdb_dbi_open(txn, nullptr, 0, &m_dbi);
    SPDLOG_TRACE(m_log, "lmdb {} db open rc = {}", path, rc);
    if(rc) {
        mdb_txn_abort(txn);
        mdb_env_close(m_mdb_env);
        SPDLOG_TRACE(m_log, "lmdb {} txn abort rc = {}", path, rc);
        return make_err(ErrorType::lmdb, rc, "Failed to open database");
    }

    SPDLOG_TRACE(m_log, "lmdb {} dbi = {}", path, m_dbi);
    rc = mdb_txn_commit(txn);
    SPDLOG_TRACE(m_log, "lmdb {} txn commit rc = {}", path, rc);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to commit transaction");
    }

    m_opened = true;

    return nullptr;
}

IRErrorPtr LmdbFile::read(uint64_t off, uint8_t* buf, size_t readsz, size_t* bytes_read) {
    uint64_t aligned_off = (off / m_blocksz)*m_blocksz;
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
        return make_err(ErrorType::lmdb, rc, "Read error. off=", aligned_off, ", msg=", mdb_strerror(rc));
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

IRErrorPtr LmdbFile::write_block(uint64_t off, uint64_t write_off, MDB_cursor* cur, const uint8_t* buf, size_t buflen) {
    MDB_val key;
    MDB_val data;

    //off is aligned_off
    key.mv_size = sizeof(off);
    key.mv_data = &off;

    auto newlen = write_off + buflen;

    auto rc = mdb_cursor_get(cur, &key, &data, MDB_SET);
    uint32_t cur_op_flag = 0;

    unique_ptr<uint8_t[]> overwrite_buf;

    switch (rc) {
    case MDB_NOTFOUND:
        SPDLOG_TRACE(m_log, "lmdb {} write_block get o={} not found", m_dbi, off);
        data.mv_size = buflen;
        data.mv_data = const_cast<void*>(reinterpret_cast<const void*>(buf));
        break;

    case MDB_SUCCESS:
        SPDLOG_TRACE(m_log, "lmdb {} write_block o={} newlen={} sz={} found", m_dbi, off, newlen, data.mv_size);
        if(newlen > m_blocksz) {
            return make_err(ErrorType::irstore, 1, 
                "Attempt to write past (", write_off + buflen,") the block size (",m_blocksz,")");
        }
        if(newlen > data.mv_size) {
            /*
            return make_err(ErrorType::irstore, 1, 
                "Attempt to write past (", write_off + buflen,") the data size (",data.mv_size,")");
            */
            overwrite_buf = unique_ptr<uint8_t[]>(new uint8_t[newlen]);
            memcpy(overwrite_buf.get(), data.mv_data, data.mv_size);
            data.mv_data = overwrite_buf.get();
            data.mv_size = newlen;
        }

        memcpy(static_cast<uint8_t*>(data.mv_data) + write_off, buf, buflen);
        cur_op_flag = MDB_CURRENT;
        break;
    default:
        return make_err(ErrorType::lmdb, rc, "Failed to read data: ", mdb_strerror(rc));
    }

    rc = mdb_cursor_put(cur, &key, &data, cur_op_flag);
    SPDLOG_TRACE(m_log, "lmdb {} write_block put o={} sz={}", m_dbi, off, data.mv_size);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to put data: ", mdb_strerror(rc));
    }

    return nullptr;
}

IRErrorPtr LmdbFile::write(uint64_t off, const uint8_t* buf, size_t buflen) {
    if(!m_opened) {
        return make_err(ErrorType::irstore, 1, "File not opened yet");
    }

#ifdef SPDLOG_TRACE_ON
    m_log->info("Write call");
#endif

    SPDLOG_TRACE(m_log, "lmdb {} write o={}, buflen = {}", m_dbi, off, buflen);
    MDB_txn *txn;

    auto rc = mdb_txn_begin(m_mdb_env, nullptr, 0, &txn);
    SPDLOG_TRACE(m_log, "lmdb {} write o={} txn begin rc={}", m_dbi, off, rc);
    if(rc) {
        SPDLOG_TRACE(m_log, "lmdb {} write o={} txn abort", m_dbi, off);
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to start transaction");
    }

    MDB_cursor* cur;
    rc = mdb_cursor_open(txn, m_dbi, &cur);
    SPDLOG_TRACE(m_log, "lmdb {} write o={} open cursor rc={}", m_dbi, off, rc);
    if(rc) {
        SPDLOG_TRACE(m_log, "lmdb {} write o={} txn abort", m_dbi, off);
        mdb_txn_abort(txn);
        return make_err(ErrorType::lmdb, rc, "Failed to open cursor");
    }

    uint64_t aligned_off = (off / m_blocksz) * m_blocksz;
    uint64_t relative_off = off - aligned_off;
    uint64_t end_aligned_off = ((off + buflen) / m_blocksz)*m_blocksz;

    auto buf_start_off = 0;
    SPDLOG_TRACE(m_log, "lmdb {} write ao={} eao={}", m_dbi, aligned_off, end_aligned_off);
    for(;aligned_off <= end_aligned_off; aligned_off += m_blocksz) {
        auto cut_len = min(relative_off + buflen - buf_start_off, static_cast<uint64_t>(m_blocksz)) - relative_off;
        if(!cut_len) continue;

        SPDLOG_TRACE(m_log, "lmdb {} write off={} ao={} buf+{}:{}", m_dbi, off, aligned_off, buf_start_off, cut_len);
        auto err = this->write_block(aligned_off, relative_off, cur, buf + buf_start_off, cut_len);
        if(err) {
            mdb_cursor_close(cur);
            mdb_txn_abort(txn);
            return err;
        }
        buf_start_off += cut_len;
        relative_off = 0;
    }

    mdb_cursor_close(cur);
    rc = mdb_txn_commit(txn);
    if(rc) {
        return make_err(ErrorType::lmdb, rc, "Failed to commit transaction");
    }
    return nullptr;
}

void LmdbFile::close() {
    if(!m_opened)
        return;
    mdb_dbi_close(m_mdb_env, m_dbi);
    mdb_env_close(m_mdb_env);
    m_opened = false;
}

LmdbFile::~LmdbFile() {
}