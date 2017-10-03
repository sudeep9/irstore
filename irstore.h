
#ifndef irstore_h
#define irstore_h

#ifdef __cplusplus

#include <string>

struct Store {

    Store(const std::string& path): m_path(path){}

    const std::string& get_path() const;

private:
    std::string m_path;
};

#endif


/* For C interop - Start */

#ifndef __cplusplus
typedef struct Store Store;
#endif

/* For C interop - End */

#ifdef __cplusplus
extern "C" {
#endif 

Store* init_store(const char* path);
void free_store(Store* s);

const char* get_store_path(Store* s);

#ifdef __cplusplus
}
#endif 


#endif
