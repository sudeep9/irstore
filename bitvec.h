
#ifndef bitvec_h
#define bitvec_h

#include <string.h>
#include <cstddef>

struct BitVec {

    BitVec():m_len(0), m_bits(nullptr) {
    }

    BitVec(size_t len):m_len(len) {
        this->init();
    }

    void init() {
        m_bits = new unsigned char[m_len];
        this->setall(false);
    }

    const unsigned char* get_data() const {
        return m_bits;
    }

    const size_t get_len() const {
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
    unsigned char* m_bits;
    size_t m_len;
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
    return (m_bits[byte_no] >> (7 - index&7)) & 1);
}

inline void BitVec::setall(bool val) {
    memset(m_bits, val, m_len);
}


#endif