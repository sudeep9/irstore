
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

    BitVec():m_len(0), m_byte_len(0), m_bits(nullptr) {
    }

    BitVec(size_t len) {
        this->init(len);
    }

    void init(size_t len) {
        m_len = len;

        //if m_len is not multiple of 8 then add 1 to m_byte_len
        m_byte_len = m_len/8 + (m_len % 8 != 0);
        this->_init();
    }

    const unsigned char* get_data() const {
        return m_bits;
    }

    size_t get_len() const {
        return m_len;
    }

    size_t get_byte_len() const {
        return m_byte_len;
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
    size_t m_byte_len;
    unsigned char* m_bits;
};

inline void BitVec::set(size_t index) {
    auto byte_no =  index >> 3;
    m_bits[byte_no] |= (1 << (7 - (index&7)));
}

inline void BitVec::clear(size_t index) {
    auto byte_no =  index >> 3;
    m_bits[byte_no] &= ~(1 << index);
}

inline bool BitVec::get(size_t index) {
    size_t byte_no = index >> 3;
    return (m_bits[byte_no] >> (7 - (index&7))) & 1;
}

inline void BitVec::setall(bool val) {
    memset(m_bits, val, m_len);
}

IRErrorPtr BitVec::marshal(std::ostream& o, const BitVec& b) {
    auto byte_len = (static_cast<std::streamsize>(b.get_byte_len()));
    o.write(reinterpret_cast<const char*>(&byte_len), sizeof(byte_len));
    if(!o.good())
        return make_err(ErrorType::sys, errno, strerror(errno));

    auto bit_len = b.get_len();
    o.write(reinterpret_cast<const char*>(&bit_len), sizeof(bit_len));
    if(!o.good())
        return make_err(ErrorType::sys, errno, strerror(errno));

    o.write(reinterpret_cast<const char*>(b.get_data()), byte_len);
    if(!o.good())
        return make_err(ErrorType::sys, errno, strerror(errno));


    return nullptr;
}

IRErrorPtr BitVec::unmarshal(std::istream& i, BitVec& b) {
    i.read(reinterpret_cast<char*>(&b.m_byte_len), sizeof(b.m_byte_len));
    if(!i.good())
        return make_err(ErrorType::sys, errno, strerror(errno));

    i.read(reinterpret_cast<char*>(&b.m_len), sizeof(b.m_len));
    if(!i.good())
        return make_err(ErrorType::sys, errno, strerror(errno));

    b.init(b.m_len);

    i.read(reinterpret_cast<char*>(b.m_bits), static_cast<std::streamsize>(b.m_byte_len));
    if(!i.good())
        return make_err(ErrorType::sys, errno, strerror(errno));

    return nullptr;
}

#endif
