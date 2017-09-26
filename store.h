
#ifndef __store_h
#define __store_h

#include<memory>
#include<string>

using namespace std;

class Store {
public:
    static shared_ptr<Store> get_instance();
    
    void init();

private:
    static shared_ptr<Store> m_inst;
    string m_store_path;
};
    


#endif