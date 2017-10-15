#ifndef irstorelog_h
#define irstorelog_h

#include <memory>

#include <spdlog/spdlog.h>
#include "spdlog/sinks/null_sink.h"

//Note: We define functions below as "inline" not for any performance gain.
//      This is done to satisfy C++ compiler's ODR rule.

inline
std::shared_ptr<spdlog::logger> create_null_log() {
    auto null_sink = std::make_shared<spdlog::sinks::null_sink_st>();
    auto log = std::make_shared<spdlog::logger>("irstore", null_sink);
    return log;
}

inline
std::shared_ptr<spdlog::logger> get_irstore_log() {
    auto log = spdlog::get("irstore");
    if(log)
        return log;

    log = create_null_log();
    return log;
}


#endif