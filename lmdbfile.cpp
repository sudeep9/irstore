
#include <iostream>

#include <lmdbfile.h>

using namespace std;


IRErrorPtr LmdbFile::open(const std::string& path) {
    if(m_opened){
        return make_err(ErrorType::irstore, 1, "File already opened: ", path);
    }
    m_path = path;

    auto rc = mdb_env_create(&m_mdb_env);
    cout<<path<<rc<<endl;

    return nullptr;
}

LmdbFile::~LmdbFile() {
    if(!m_mdb_env)
        return;

    mdb_env_close(m_mdb_env);

}