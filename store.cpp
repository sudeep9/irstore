
#include <string>

#include <irstore.h>

using namespace std;

const string& Store::get_path() const {
    return this->m_path;
}

Store* init_store(const char* path) {
    return new Store(path);
}

void free_store(Store* s) {
    delete s;
}

const char* get_store_path(Store* s) {
    return s->get_path().c_str();
}