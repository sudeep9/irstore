
#ifndef irerror_h
#define irerror_h

#ifdef __cplusplus

#include <cstdarg>
#include <sstream>
#include <utility>
#include <string>
#include <memory>

#include <errortype.h>

struct _IRErrorInfo{
    std::string m_msg;
    int64_t m_errno;
    ErrorType m_type;
};

#define Try(f) {\
    auto err = f; \
    if(err != nullptr) { \
        return err; \
    } \
} 

#define TryC(f) {\
    auto err = f; \
    if(err != nullptr) { \
        return err.release(); \
    } \
} 

struct IRError {
    IRError(ErrorType errtype, const std::string& s) {
        m_info = std::make_shared<_IRErrorInfo>();

        m_info->m_msg = s;
        m_info->m_errno = 0;
        m_info->m_type= errtype;
    }

    IRError(ErrorType errtype, int64_t _errno) {
        m_info = std::make_shared<_IRErrorInfo>();

        std::ostringstream buf;
        buf<<"errno: "<<_errno;
        m_info->m_errno = _errno;

        m_info->m_type= errtype;
        m_info->m_msg = buf.str();
    }

    IRError(ErrorType errtype, int64_t _errno, const std::string& s) {
        m_info = std::make_shared<_IRErrorInfo>();
        m_info->m_errno = _errno;
        m_info->m_msg = s;
        m_info->m_type= errtype;
    }

    int64_t err() const {
        return m_info->m_errno;
    }

    const std::string& msg() const {
        return m_info->m_msg;
    }

    ErrorType errtype() const {
        return m_info->m_type;
    }

private:
    std::shared_ptr<_IRErrorInfo> m_info;
};

typedef std::unique_ptr<IRError> IRErrorPtr;

inline
std::ostream& operator<<(std::ostream& o, const IRError& e) {
    o<<e.err()<<" msg = " << e.msg();

    return o;
}

template<typename Args>
inline
void _make_err(std::ostringstream& o, Args&& arg) {
    o<<std::forward<Args>(arg);
}

template<typename ...Args>
inline
std::unique_ptr<IRError> make_err(ErrorType errtype, int64_t _errno, const std::string& first, Args&&... args) {
    std::ostringstream buf;

    buf<<first;
    _make_err(buf, std::forward<Args>(args)...);
    return std::make_unique<IRError>(errtype, _errno, buf.str());
} 

inline
std::unique_ptr<IRError> make_err(ErrorType errtype, const std::string& s) {
    return std::make_unique<IRError>(errtype, s);
} 

inline
std::unique_ptr<IRError> make_err(ErrorType errtype, int64_t _errno) {
    return std::make_unique<IRError>(errtype, _errno);
}

inline
std::unique_ptr<IRError> make_err(ErrorType errtype, int64_t _errno, const std::string& s) {
    return std::make_unique<IRError>(errtype, _errno, s);
}

inline
IRError* clone_err(const IRError& e) {
    auto new_err = new IRError(e.errtype(), e.err(), e.msg());
    return new_err;
}

#endif

#ifndef __cplusplus
typedef struct IRError IRError;
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

const char* get_errmsg(const IRError* err);
int64_t get_errno(const IRError* err);
void free_err(IRError* err);

#ifdef __cplusplus
}
#endif


#endif
