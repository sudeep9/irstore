#include <memory>
#include <functional>

#include <lmdb.h>

template<typename T>
using MDB_env_ptr = std::unique_ptr<MDB_env*, std::function<void(T*)>>;