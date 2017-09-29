
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sstream>

#include <file.h>

using namespace std;

string join_path(const string& a, const string& b) {
    ostringstream path;
    path<<a<<"/"<<b;

    return path.str();
}

IRErrorPtr File::open_src_file() {
    m_srcfd = ::open(m_srcfile.c_str(), O_RDONLY);
    if(m_srcfd < 0) {
        return make_err(errno, "Failed to open file: ", m_srcfile);
    }

    struct stat statbuf;
    if(fstat(m_srcfd, &statbuf) < 0) {
        return make_err(errno, "Failed to stat file: ", m_srcfile);
    }

    auto bit_vec_len = statbuf.st_size/m_blocksz + 1;
    m_bvec.resize(bit_vec_len, false);

    return nullptr;
}

IRErrorPtr File::open_cow_file() {
    auto cow_path = join_path(m_store_path, m_cowfile);
    m_cowfd = ::open(cow_path.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    if(m_cowfd < 0) {
        return make_err(errno, "Failed to open file: ", cow_path);
    }

    return nullptr;
}

IRErrorPtr File::open() {
    Try(this->open_src_file());
    Try(this->open_cow_file());

    return nullptr;
}

void File::close() {
    if(m_srcfd > 0) {
        ::close(m_srcfd);
    }
}

IRErrorPtr File::read(int64_t offset, char* buf, size_t buflen, ssize_t* bytes_read) {
    auto block_no = offset / m_blocksz;

    auto fd = m_cowfd;
    if(!m_bvec[block_no]) {
        fd = m_srcfd;
    }

    *bytes_read = pread(fd, buf, buflen, offset);
    if(*bytes_read < 0) {
        return make_err(errno, "Failed to read file: ", m_srcfile);
    }

    return nullptr;
}

File* create_file(const char* path, uint32_t blocksz, const char* src, const char* cow) {
    auto f = new File(path, blocksz, src, cow);
    return f;
}

void close_file(File *f) {
    f->close();
}

void free_file(File* f) {
    delete f;
}

IRError* open_file(File* f) {
    TryC(f->open());

    return nullptr;
}

IRError* read_file(File* f, int64_t offset, char* buf, size_t buflen, ssize_t* bytes_read) {
    TryC(f->read(offset, buf, buflen, bytes_read));

    return nullptr;
}