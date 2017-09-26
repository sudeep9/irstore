

#include<iostream>
#include<store.h>

using namespace std;

int main() {

    auto s = Store::get_instance();

    if(s)
        cout<<"ok"<<endl;

    return 0;
}