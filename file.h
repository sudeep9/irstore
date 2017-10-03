#ifndef file_h
#define file_h

#ifdef __cplusplus

#include <string>
#include <memory>
#include <vector>

#include <irerror.h>

struct File {
    File(const std::string& store_path, 
        uint32_t blocksz, 
        const std::string& src, 
        const std::string& cow)
        :m_blocksz(blocksz), m_store_path(store_path), m_srcfile(src), m_cowfile(cow) {
    }

    IRErrorPtr open();
    IRErrorPtr read(int64_t offset, char* buf, size_t buflen, ssize_t* bytes_read);
    IRErrorPtr write(int64_t offset, const void *buf, size_t count);

    void close();

private:

    IRErrorPtr open_src_file(); 
    IRErrorPtr open_cow_file(); 

    uint32_t m_blocksz;
    std::string m_store_path;
    std::string m_srcfile;
    std::string m_cowfile;
    int m_srcfd;
    int m_cowfd;
    std::vector<bool> m_bvec;
};

std::string join_path(const std::string& a, const std::string& b); 

#endif

#ifndef __cplusplus
typedef struct File File;
#endif

#ifdef __cplusplus
extern "C" {
#endif

File* create_file(const char* path, uint32_t blocksz, const char* src, const char* cow);
IRError* open_file(File* f);
IRError* read_file(File* f, int64_t offset, char* buf, size_t buflen, ssize_t* bytes_read);
IRError* write_file(File* f, int64_t offset, const void *buf, size_t count);
void close_file(File* f);
void free_file(File* f);

#ifdef __cplusplus
}
#endif

#endif
