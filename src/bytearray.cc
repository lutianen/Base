
#include <Base/bytearray.h>
#include <Base/logger.h>

#include <iomanip>  // setw, setfill
#include <sstream>  // stringstream

using namespace Lute;

static inline uint32_t EncodeZigzag32(const int32_t& v) {
    return (v << 1) ^ (v >> 31);
}
static inline uint64_t EncodeZigzag64(const int64_t& v) {
    return (v << 1) ^ (v >> 63);
}
static inline int32_t DecodeZigzag32(const uint32_t& v) {
    return (v >> 1) ^ -(v & 1);
}
static inline int64_t DecodeZigzag64(const uint64_t& v) {
    return (v >> 1) ^ -(v & 1);
}

ByteArray::Node::Node() : ptr_(nullptr), next_(nullptr), size_(0) {}
ByteArray::Node::Node(size_t size)
    : ptr_(new char[size]), next_(nullptr), size_(size) {}
ByteArray::Node::~Node() {
    if (nullptr != ptr_) delete[] ptr_;
}

ByteArray::ByteArray(size_t base_size)
    : baseSize_(base_size),
      position_(0),
      capacity_(base_size),
      size_(0),
      endian_(LUTE_BYTE_ORDER),
      root_(new Node(base_size)),
      curr_(root_) {}

ByteArray::~ByteArray() {
    Node* tmp = root_;
    while (nullptr != tmp) {
        curr_ = tmp;
        tmp = tmp->next_;
        delete curr_;
    }
}
void ByteArray::writeFint8(int8_t val) { write(&val, sizeof(val)); }

void ByteArray::writeFuint8(uint8_t val) { write(&val, sizeof(val)); }

void ByteArray::writeFint16(int16_t val) {
    if (endian_ != LUTE_BYTE_ORDER) val = byteswap(val);
    write(&val, sizeof(val));
}

void ByteArray::writeFuint16(uint16_t val) {
    if (endian_ != LUTE_BYTE_ORDER) val = byteswap(val);
    write(&val, sizeof(val));
}

void ByteArray::writeFint32(int32_t val) {
    if (endian_ != LUTE_BYTE_ORDER) val = byteswap(val);
    write(&val, sizeof(val));
}

void ByteArray::writeFuint32(uint32_t val) {
    if (endian_ != LUTE_BYTE_ORDER) val = byteswap(val);
    write(&val, sizeof(val));
}

void ByteArray::writeFint64(int64_t val) {
    if (endian_ != LUTE_BYTE_ORDER) val = byteswap(val);
    write(&val, sizeof(val));
}

void ByteArray::writeFuint64(uint64_t val) {
    if (endian_ != LUTE_BYTE_ORDER) val = byteswap(val);
    write(&val, sizeof(val));
}

void ByteArray::writeInt32(int32_t val) { writeUint32(EncodeZigzag32(val)); }

void ByteArray::writeUint32(uint32_t val) {
    uint8_t tmp[5];
    uint8_t i = 0;
    while (val >= 0x80) {
        tmp[i++] = (val & 0x7F) | 0x80;
        val >>= 7;
    }
    tmp[i++] = val;
    write(tmp, i);
}

void ByteArray::writeInt64(int64_t val) { writeUint64(EncodeZigzag64(val)); }

void ByteArray::writeUint64(uint64_t val) {
    uint8_t tmp[10];
    uint8_t i = 0;
    while (val >= 0x80) {
        tmp[i++] = (val & 0x7F) | 0x80;
        val >>= 7;
    }
    tmp[i++] = val;
    write(tmp, i);
}

void ByteArray::writeFloat(float val) {
    uint32_t v;
    ::memcpy(&v, &val, sizeof(val));
    writeFuint32(v);
}

void ByteArray::writeDouble(double val) {
    uint64_t v;
    ::memcpy(&v, &val, sizeof(val));
    writeFuint64(v);
}

void ByteArray::writeStringF16(const std::string_view& val) {
    writeFuint16(val.size());
    write(val.data(), val.size());
}

void ByteArray::writeStringF32(const std::string_view& val) {
    writeFuint32(val.size());
    write(val.data(), val.size());
}

void ByteArray::writeStringF64(const std::string_view& val) {
    writeFuint64(val.size());
    write(val.data(), val.size());
}

void ByteArray::writeStringVint(const std::string_view& val) {
    writeUint64(val.size());
    write(val.data(), val.size());
}

void ByteArray::writeStringWithoutLength(const std::string_view& val) {
    write(val.data(), val.size());
}

void ByteArray::write(const void* buf, size_t size) {
    if (size == 0) return;

    ensureCapacity(size);

    // Current node writable position.
    size_t npos = position_ % baseSize_;
    // Left writable size in current node.
    size_t ncap = curr_->size_ - npos;
    size_t bpos = 0;

    while (size > 0) {
        if (ncap >= size) {
            ::memcpy(curr_->ptr_ + npos,
                     reinterpret_cast<const char*>(buf) + bpos, size);
            if (curr_->size_ == npos + size) curr_ = curr_->next_;
            position_ += size;
            bpos += size;
            size = 0;
        } else {
            ::memcpy(curr_->ptr_ + npos,
                     reinterpret_cast<const char*>(buf) + bpos, ncap);
            position_ += ncap;
            bpos += ncap;
            size -= ncap;
            curr_ = curr_->next_;
            ncap = curr_->size_;
            npos = 0;
        }
    }

    if (position_ > size_) size_ = position_;
}

int8_t ByteArray::readFint8() {
    int8_t v;
    read(&v, sizeof(v));
    return v;
}

uint8_t ByteArray::readFuint8() {
    uint8_t v;
    read(&v, sizeof(v));
    return v;
}

#define XX(type)                      \
    type v;                           \
    read(&v, sizeof(v));              \
    if (endian_ == LUTE_BYTE_ORDER) { \
        return v;                     \
    } else {                          \
        return byteswap(v);           \
    }

int16_t ByteArray::readFint16() { XX(int16_t); }
uint16_t ByteArray::readFuint16() { XX(uint16_t); }

int32_t ByteArray::readFint32() { XX(int32_t); }

uint32_t ByteArray::readFuint32() { XX(uint32_t); }

int64_t ByteArray::readFint64() { XX(int64_t); }

uint64_t ByteArray::readFuint64() { XX(uint64_t); }

#undef XX

int32_t ByteArray::readInt32() { return DecodeZigzag32(readUint32()); }

uint32_t ByteArray::readUint32() {
    uint32_t result = 0;
    for (int i = 0; i < 32; i += 7) {
        uint8_t b = readFuint8();
        if (b < 0x80) {
            result |= (static_cast<uint32_t>(b)) << i;
            break;
        } else {
            result |= ((static_cast<uint32_t>(b & 0x7f)) << i);
        }
    }
    return result;
}

int64_t ByteArray::readInt64() { return DecodeZigzag64(readUint64()); }

uint64_t ByteArray::readUint64() {
    uint64_t result = 0;
    for (int i = 0; i < 64; i += 7) {
        uint8_t b = readFuint8();
        if (b < 0x80) {
            result |= (static_cast<uint64_t>(b)) << i;
            break;
        } else {
            result |= ((static_cast<uint64_t>(b & 0x7f)) << i);
        }
    }
    return result;
}

float ByteArray::readFloat() {
    uint32_t v = readFuint32();
    float value;
    ::memcpy(&value, &v, sizeof(v));
    return value;
}

double ByteArray::readDouble() {
    uint64_t v = readFuint64();
    double value;
    ::memcpy(&value, &v, sizeof(v));
    return value;
}

std::string ByteArray::readStringF16() {
    uint16_t len = readFuint16();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF32() {
    uint32_t len = readFuint32();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF64() {
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringVint() {
    uint64_t len = readUint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

void ByteArray::read(void* buf, size_t size) {
    if (size > readableSize()) {
        throw std::out_of_range("not enough len");
    }

    size_t npos = position_ % baseSize_;
    size_t ncap = curr_->size_ - npos;
    size_t bpos = 0;
    while (size > 0) {
        if (ncap >= size) {
            ::memcpy(reinterpret_cast<char*>(buf) + bpos, curr_->ptr_ + npos,
                     size);
            if (curr_->size_ == (npos + size)) {
                curr_ = curr_->next_;
            }
            position_ += size;
            bpos += size;
            size = 0;
        } else {
            ::memcpy(reinterpret_cast<char*>(buf) + bpos, curr_->ptr_ + npos,
                     ncap);
            position_ += ncap;
            bpos += ncap;
            size -= ncap;
            curr_ = curr_->next_;
            ncap = curr_->size_;
            npos = 0;
        }
    }
}

void ByteArray::read(void* buf, size_t size, size_t position) const {
    if (size > size_ - position) throw std::out_of_range("not enough len");

    size_t npos = position % baseSize_;
    size_t ncap = curr_->size_ - npos;
    size_t bpos = 0;
    Node* curr = curr_;

    while (size > 0) {
        if (ncap >= size) {
            ::memcpy(reinterpret_cast<char*>(buf) + bpos, curr->ptr_ + npos,
                     size);
            if (curr->size_ == (npos + size)) curr = curr->next_;
            position += size;
            bpos += size;
            size = 0;
        } else {
            ::memcpy(reinterpret_cast<char*>(buf) + bpos, curr->ptr_ + npos,
                     ncap);
            position += ncap;
            bpos += ncap;
            size -= ncap;
            curr = curr->next_;
            ncap = curr->size_;
            npos = 0;
        }
    }
}

void ByteArray::setPosition(size_t val) {
    if (val > capacity_) throw std::out_of_range("set position out of range");

    position_ = val;
    if (position_ > size_) size_ = position_;
    curr_ = root_;
    while (val > curr_->size_) {
        val -= curr_->size_;
        curr_ = curr_->next_;
    }
    if (val == curr_->size_) curr_ = curr_->next_;
}

std::string ByteArray::toString() const {
    std::string str;
    str.resize(readableSize());
    if (str.empty()) return str;
    read(&str[0], str.size(), position_);
    return str;
}

std::string ByteArray::toHexString() const {
    std::string str = toString();
    std::stringstream ss;

    for (size_t i = 0; i < str.size(); ++i) {
        if (i > 0 && i % 32 == 0) ss << std::endl;
        ss << std::setw(2) << std::setfill('0') << std::hex
           << static_cast<int>(static_cast<uint8_t>(str[i])) << " ";
    }

    return ss.str();
}

uint64_t ByteArray::readableBuffers(std::vector<iovec>& buffers,
                                    uint64_t len) const {
    len = len > readableSize() ? readableSize() : len;
    if (len == 0) return 0;

    uint64_t size = len;
    size_t npos = position_ % baseSize_;
    size_t ncap = curr_->size_ - npos;
    struct iovec iov;
    Node* curr = curr_;

    while (len > 0) {
        if (ncap >= len) {
            iov.iov_base = curr->ptr_ + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = curr->ptr_ + npos;
            iov.iov_len = ncap;
            len -= ncap;
            curr = curr->next_;
            ncap = curr->size_;
            npos = 0;
        }
        buffers.push_back(iov);
    }

    return size;
}

uint64_t ByteArray::readableBuffers(std::vector<iovec>& buffers, uint64_t len,
                                    uint64_t position) const {
    len = len > readableSize() ? readableSize() : len;
    if (len == 0) return 0;

    uint64_t size = len;

    size_t npos = position % baseSize_;
    size_t count = curr_->size_ / npos;
    Node* curr = curr_;
    while (count > 0) {
        curr = curr->next_;
        --count;
    }

    size_t ncap = curr->size_ - npos;
    struct iovec iov;
    while (len > 0) {
        if (ncap >= len) {
            iov.iov_base = curr->ptr_ + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = curr->ptr_ + npos;
            iov.iov_len = ncap;
            len -= ncap;
            curr = curr->next_;
            ncap = curr->size_;
            npos = 0;
        }
        buffers.push_back(iov);
    }

    return size;
}

uint64_t ByteArray::writableBuffers(std::vector<iovec>& buffers, uint64_t len) {
    if (len == 0) return 0;
    ensureCapacity(len);
    uint64_t size = len;

    size_t npos = position_ % baseSize_;
    size_t ncap = curr_->size_ - npos;
    struct iovec iov;
    Node* curr = curr_;
    while (len > 0) {
        if (ncap >= len) {
            iov.iov_base = curr->ptr_ + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = curr->ptr_ + npos;
            iov.iov_len = ncap;

            len -= ncap;
            curr = curr->next_;
            ncap = curr->size_;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

bool ByteArray::readFromFile(const std::string_view& name) {
    std::ifstream ifs;
    if (!Lute::FSUtil::openForRead(ifs, name.data(), std::ios_base::binary)) {
        LOG_ERROR << "readFromFile name=" << name.data()
                  << " error, errno= " << errno
                  << " errstr=" << strerror_tl(errno);
        return false;
    }

    std::shared_ptr<char> buff(new char[baseSize_],
                               [](char* ptr) { delete[] ptr; });
    while (!ifs.eof()) {
        ifs.read(buff.get(), baseSize_);
        write(buff.get(), ifs.gcount());
    }

    return true;
}

bool ByteArray::writeToFile(const std::string_view& name) const {
    std::ofstream ofs;
    if (!Lute::FSUtil::openForWrite(ofs, name.data(), std::ios_base::binary)) {
        LOG_ERROR << "writeToFile name=" << name.data()
                  << " error, errno= " << errno
                  << " errstr=" << strerror_tl(errno);
        return false;
    }

    int64_t readSize = readableSize();
    int64_t pos = position_;
    Node* curr = curr_;
    while (readSize > 0) {
        int diff = pos % baseSize_;
        int64_t len = (readSize > static_cast<int64_t>(baseSize_) ? baseSize_
                                                                  : readSize) -
                      diff;
        ofs.write(curr->ptr_ + diff, len);
        curr = curr->next_;
        pos += len;
        readSize -= len;
    }

    return true;
}

void ByteArray::clear() {
    position_ = size_ = 0;
    capacity_ = baseSize_;
    Node* tmp = root_->next_;
    while (nullptr != tmp) {
        curr_ = tmp;
        tmp = tmp->next_;
        delete curr_;
    }
    curr_ = root_;
    root_->next_ = nullptr;
}

void ByteArray::ensureCapacity(size_t size) {
    if (size == 0) return;

    size_t oldCap = writableCapacity();
    if (oldCap >= size) return;

    size -= oldCap;
    size_t count = std::ceil(1.0 * size / baseSize_);
    Node* tmp = root_;
    while (tmp->next_) tmp = tmp->next_;

    Node* first = nullptr;
    for (size_t i = 0; i < count; ++i) {
        tmp->next_ = new Node(baseSize_);
        if (first == nullptr) first = tmp->next_;
        tmp = tmp->next_;
        capacity_ += baseSize_;
    }

    if (oldCap == 0) curr_ = first;
}