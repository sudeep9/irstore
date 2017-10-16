
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sstream>

#include <errno.h>

#include <spdlog/spdlog.h>
#include <sparsefile.h>

using namespace std;

string join_path(const string& a, const string& b) {
    ostringstream path;
    path<<a<<"/"<<b;

    return path.str();
}

IRErrorPtr SparseFile::open_src_file() {
    m_srcfd = ::open(m_srcfile.c_str(), O_RDONLY);
    if(m_srcfd < 0) {
        return make_err(ErrorType::sys, errno, "Failed to open file: ", m_srcfile);
    }

    struct stat statbuf;
    if(fstat(m_srcfd, &statbuf) < 0) {
        return make_err(ErrorType::sys, errno, "Failed to stat file: ", m_srcfile);
    }

    m_src_filesz = statbuf.st_size;

    return nullptr;
}

IRErrorPtr SparseFile::open_cow_file(off_t filesz) {
    auto cow_path = join_path(m_store_path, m_cowfile);
    Try(this->construct_bvec(cow_path));

    m_cowfd = ::open(cow_path.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    if(m_cowfd < 0) {
        return make_err(ErrorType::sys, errno, "Failed to open file: ", strerror(errno), cow_path);
    }

    auto rc = ::ftruncate(m_cowfd, filesz);
    if(rc < 0) {
        return make_err(ErrorType::sys, errno, "Failed to truncate file: ", strerror(errno), cow_path);
    }

    return nullptr;
}

IRErrorPtr SparseFile::construct_bvec(const string& cow_path) {
    auto fd = ::open(cow_path.c_str(), O_RDONLY);
    if(fd < 0) {
        auto rc = errno;
        if(rc != ENOENT){
            return make_err(ErrorType::sys, rc, "Failed to open file: ", strerror(rc), cow_path);
        }

        auto bit_vec_len = static_cast<size_t>(m_src_filesz/m_blocksz) + 1;
        m_bvec.resize(bit_vec_len, false);

        return nullptr;
    }

    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) {
        ::close(fd);
        return make_err(ErrorType::sys, errno, "Failed to stat file: ", cow_path);
    }

    auto bit_vec_len = static_cast<size_t>(statbuf.st_size/m_blocksz) + 1;
    m_bvec.resize(bit_vec_len, false);
    off_t off = 0;

    IRErrorPtr err = nullptr; 

    while(off < statbuf.st_size) {
        auto rc = lseek(fd, off, SEEK_DATA);
        if(rc < 0) {
            if(errno != ENXIO) {
                err = make_err(ErrorType::sys, errno, "Failed to seek data: ", strerror(errno), cow_path);
            }
            break;
        }

        auto block_no = rc/m_blocksz;
        m_bvec[block_no] = true;
        cout<<"data at offset: "<<rc<<endl;

        off = rc + m_blocksz;
    }

    cout<<">> Bit vec: ";
    for(auto e: m_bvec) {
        cout<<e<<" ";
    }
    cout<<endl;

    ::close(fd);

    return err ;
}

IRErrorPtr SparseFile::open() {
    Try(this->open_src_file());
    Try(this->open_cow_file(m_src_filesz));

    return nullptr;
}


bool SparseFile::is_cow(int64_t offset) const {
    auto block_no = offset/m_blocksz;

    if(block_no > m_bvec.size())
        return false;

    return m_bvec[block_no];
}

void SparseFile::close() {
    if(m_srcfd > 0) {
        ::close(m_srcfd);
    }
}

IRErrorPtr SparseFile::read(int64_t offset, char* buf, size_t buflen, ssize_t* bytes_read) {
    auto block_no = static_cast<size_t>(offset / m_blocksz);

    auto fd = m_cowfd;
    if(!m_bvec[block_no]) {
        fd = m_srcfd;
    }

    *bytes_read = pread(fd, buf, buflen, offset);
    if(*bytes_read < 0) {
        return make_err(ErrorType::sys, errno, "Failed to read file: ", m_srcfile);
    }

    return nullptr;
}

IRErrorPtr SparseFile::write(int64_t offset, const void *buf, size_t count) {
    auto block_no = static_cast<size_t>(offset / m_blocksz);
    auto block_offset = block_no * m_blocksz;
    auto relative_off = offset - block_offset;

    if(relative_off + count > m_blocksz) {
        return make_err(ErrorType::irstore, 1, "Attempt write past block size");
    }
    
    ssize_t byte_count;
    if(m_bvec[block_no]) {
        byte_count = pwrite(m_cowfd, buf, count, offset);
        if(byte_count < 0) {
            return make_err(ErrorType::sys, errno, "Failed to write file: ", m_cowfile);
        }
    }else{
        unique_ptr<char> read_buf(new char[m_blocksz]);
        Try(this->read(static_cast<int64_t>(block_offset), read_buf.get(), m_blocksz, &byte_count));
        if(relative_off + count > byte_count) {
            return make_err(ErrorType::irstore, 1, "Attempt write past truncated block");
        }
        memcpy(read_buf.get() + relative_off, buf, count);
        byte_count = pwrite(m_cowfd, read_buf.get(), byte_count, block_offset);
        if(byte_count < 0) {
            return make_err(ErrorType::sys, errno, "Failed to write file: ", m_cowfile);
        }
    }

    m_bvec[block_no] = true;
    return nullptr;
}


SparseFile* create_file(const char* path, uint32_t blocksz, const char* src, const char* cow) {
    auto f = new SparseFile(path, blocksz, src, cow);
    return f;
}

void close_file(SparseFile *f) {
    f->close();
}

void free_file(SparseFile* f) {
    delete f;
}

IRError* open_file(SparseFile* f) {
    TryC(f->open());

    return nullptr;
}

IRError* read_file(SparseFile* f, int64_t offset, char* buf, size_t buflen, ssize_t* bytes_read) {
    TryC(f->read(offset, buf, buflen, bytes_read));

    return nullptr;
}

IRError* write_file(SparseFile* f, int64_t offset, const void *buf, size_t count) {
    TryC(f->write(offset, buf, count));

    return nullptr;
}
