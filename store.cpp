
#include <store.h>

shared_ptr<Store> Store::m_inst = nullptr;

shared_ptr<Store> Store::get_instance() {
    if(!Store::m_inst)
        Store::m_inst = make_shared<Store>();

    return Store::m_inst;
}