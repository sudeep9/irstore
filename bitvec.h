
#ifndef bitvec_h
#define bitvec_h

#include <string.h>
#include <cstddef>
#include <fstream>
#include <irerror.h>
#include <cstring>

//TODO: Make m_bits as unique_ptr

struct BitVec {

    static IRErrorPtr marshal(std::ostream& o, const BitVec& b);
    static IRErrorPtr unmarshal(std::istream& o, BitVec& b);

    BitVec():m_len(0), m_bits(nullptr) {
    }

    BitVec(size_t len) {
        this->init(len);
    }

    void init(size_t len) {
        m_len = len;
        this->_init();
    }

    const unsigned char* get_data() const {
        return m_bits;
    }

    size_t get_len() const {
        return m_len;
    }

    void set(size_t index);
    bool get(size_t index);
    void clear(size_t index);
    void setall(bool val);

    ~BitVec() {
        delete[] m_bits;
        m_len = 0;
    }

private:
    friend IRErrorPtr unmarshal(std::istream& o, BitVec& b);
    void _init() {
        m_bits = new unsigned char[m_len];
        this->setall(false);
    }

    size_t m_len;
    unsigned char* m_bits;
};

inline void BitVec::set(size_t index) {
    auto byte_no =  index >> 3;
    m_bits[byte_no] |= (1 << (7 - index&7));
}

inline void BitVec::clear(size_t index) {
    auto byte_no =  index >> 3;
    m_bits[byte_no] &= ~(1 << index);
}

inline bool BitVec::get(size_t index) {
    size_t byte_no = index >> 3;
    return (m_bits[byte_no] >> (7 - index&7)) & 1;
}

inline void BitVec::setall(bool val) {
    memset(m_bits, val, m_len);
}

IRErrorPtr BitVec::marshal(std::ostream& o, const BitVec& b) {
    auto len = (static_cast<std::streamsize>(b.get_len())) >> 3;
    o.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if(!o.good())
        return make_err(errno, strerror(errno));

    o.write(reinterpret_cast<const char*>(b.get_data()), len);
    if(!o.good())
        return make_err(errno, strerror(errno));


    return nullptr;
}

IRErrorPtr BitVec::unmarshal(std::istream& i, BitVec& b) {
    i.read(reinterpret_cast<char*>(&b.m_len), sizeof(b.m_len));
    if(!i.good())
        return make_err(errno, strerror(errno));

    b.init(b.m_len);

    i.read(reinterpret_cast<char*>(b.m_bits), static_cast<std::streamsize>(b.m_len));
    if(!i.good())
        return make_err(errno, strerror(errno));

    return nullptr;
}

#endif
