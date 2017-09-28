
#include <irerror.h>

using namespace std;


const char* get_errmsg(const IRError* err) {
    return err->msg().c_str();
}

int64_t get_errno(const IRError* err) {
    return err->err();
}

void free_err(IRError* err) {
    delete err;
}
