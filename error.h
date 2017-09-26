
#ifndef __error_h
#define __error_h

#include <utility>

struct Error {
    virtual string what() = 0;
}

#endif