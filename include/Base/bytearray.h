///
/// @brief 二进制数组 - 序列化 / 反序列化
///
/// 核心算法：Zigzag 编码
///
/// Zigzag 编码的优势在于它可以将有符号整数编码为无符号整数，
/// 并使用可变长度编码格式，从而实现数据的压缩和节省存储空间。
///
/// @details Zigzag 编码可以将有符号整数转换为无符号整数，
///          从而使得存储和传输有符号整数时可以使用无符号整数的编码格式，
///          这通常比有符号整数的编码格式更加紧凑和高效。
///
///          Zigzag 编码还可以使用可变长度编码格式（如Protobuf、Varint编码等）
///          根据数据的实际大小动态调整编码长度，并避免不必要的数据填充，进一步提高数据的压缩比和存储效率。
///
///          Zigzag 编码还可以避免对有符号整数进行显式的符号扩展或截断操作，
///          从而避免了在不同的平台或语言之间处理有符号整数的兼容性问题。
///
///          综上，Zigzag编码是一种高效和可靠的编码算法，广泛应用于数据压缩、网络传输、存储等领域，
///          尤其在移动设备、嵌入式系统、物联网等资源受限的环境下具有重要的应用价值。
///
/// @example
///     Lute::ByteArray::ptr ba(new Lute::ByteArray(10));
///     ba->writeFloat(1.234f);
///     ba->setPosition(0);
///     std::cout << ba->readFloat() << std::endl;
///     assert(ba->readableSize() == 0);
///     ba->clear();
///

#pragma once

#include <Base/endian.h>  // LUTE_BYTE_ORDER, LUTE_BIG_ENDIAN, LUTE_LITTLE_ENDIAN
#include <Base/string_view.h>  // string_view
#include <sys/socket.h>        // iovec

#include <cstdint>  // int32_t, int64_t, uint32_t, uint64_t
#include <memory>   // shared_ptr
#include <vector>   // vector

namespace Lute {
///
/// @brief 二进制数组 - 提供基础类型的序列化 / 反序列化
///
class ByteArray {
public:
    using ptr = std::shared_ptr<ByteArray>;

    ///
    /// @brief ByteArray 存储节点
    ///
    struct Node {
        /// @brief 默认构造函数
        Node();
        /// @brief 构造函数 - 构造指定大小为 s 字节的内存块
        explicit Node(size_t size = 4096);
        /// @brief 析构函数 - 释放内存
        ~Node();

        /// 内存块指针
        char* ptr_;
        /// 下一个内存块
        Node* next_;
        /// 内存块大小
        size_t size_;
    };

    ///
    /// @brief 使用指定长度的内存块构造 ByteArray
    /// @param[in] base_size 内存块大小
    ///
    explicit ByteArray(size_t base_size = 4096);
    ~ByteArray();

    ///
    /// @brief Write fixed-length int8_t type data
    /// @post position_ += sizeof(int8_t)
    /// @param[in] val
    /// @note If position_ > size_, size = position_
    ///
    void writeFint8(int8_t val);

    /**
     * @brief Write fixed-length uint8_t type data
     *
     * @param[in] val
     * @note If position_ > size_, size = position_
     */
    void writeFuint8(uint8_t val);

    /**
     * @brief Write fixed-length int16_t type data
     * @param[in] val
     * @note If position_ > size_, size = position_
     */
    void writeFint16(int16_t val);

    /**
     * @brief Write fixed-length uint16_t type data
     *
     * @param[in] val
     */
    void writeFuint16(uint16_t val);

    /**
     * @brief Write fixed-length int32_t type data
     *
     * @param[in] val
     */
    void writeFint32(int32_t val);

    /**
     * @brief Write fixed-length uint32_t type data
     *
     * @param[in] val
     *
     */
    void writeFuint32(uint32_t val);

    /**
     * @brief Write fixed-length int64_t type data
     *
     * @param[in] val
     *
     */
    void writeFint64(int64_t val);

    /**
     * @brief Write fixed-length uint64_t type data
     *
     * @param[in] val
     *
     */
    void writeFuint64(uint64_t val);

    /**
     * @brief Write fixed-length float type data
     *
     * @param[in] val
     *
     */
    void writeInt32(int32_t val);

    /**
     * @brief Write fixed-length uint32_t type data
     *
     * @param[in] val
     *
     */
    void writeUint32(uint32_t val);

    /**
     * @brief Write fixed-length int64_t type data
     *
     * @param[in] val
     *
     */
    void writeInt64(int64_t val);

    /**
     * @brief Write fixed-length uint64_t type data
     *
     * @param[in] val
     *
     */
    void writeUint64(uint64_t val);

    /**
     * @brief Write fixed-length float type data
     *
     * @param[in] val
     *
     */
    void writeFloat(float val);

    /**
     * @brief Write fixed-length double type data
     *
     * @param[in] val
     *
     */
    void writeDouble(double val);

    /**
     * @brief Write std::string type data with uint16_t as data type
     * @param[in] val
     * @post position_ += 2 + val.size()
     */
    void writeStringF16(const std::string_view& val);

    /**
     * @brief Write std::string type data with uint32_t as data type
     * @param[in] val
     * @post position_ += 4 + val.size()
     */
    void writeStringF32(const std::string_view& val);

    /**
     * @brief Write std::string type data with uint64_t as data type
     * @param[in] val
     * @post position_ += 8 + val.size()
     */
    void writeStringF64(const std::string_view& val);

    /**
     * @brief Write Lute::string_view type data with uint64_t as data type
     * @param[in] val
     * @post position_ += 8 + val.size()
     */
    void writeStringVint(const std::string_view& val);

    /**
     * @brief Write std::string type data without length
     * @param[in] val
     * @post position_ += val.size()
     */
    void writeStringWithoutLength(const std::string_view& val);

    /**
     * @brief Write size fixed-length data from buf
     * @param[in] buf
     * @param[in] size data length
     */
    void write(const void* buf, size_t size);

    /**
     * @brief Read fixed-length int8_t type data
     * @pre readableSize() >= sizeof(int8_t)
     * @post position_ += sizeof(int8_t)
     * @exception std::out_of_range when readableSize() < sizeof(int8_t)
     * @return The read data
     */
    int8_t readFint8();

    /**
     * @brief Read fixed-length uint8_t type data
     * @pre readableSize() >= sizeof(uint8_t)
     * @post position_ += sizeof(uint8_t)
     * @exception std::out_of_range when readableSize() < sizeof(uint8_t)
     * @return The read data
     */
    uint8_t readFuint8();

    /**
     * @brief Read fixed-length int16_t type data
     * @pre readableSize() >= sizeof(int16_t)
     * @post position_ += sizeof(int16_t)
     * @exception std::out_of_range when readableSize() < sizeof(int16_t)
     * @return The read data
     */
    int16_t readFint16();

    /**
     * @brief Read fixed-length uint16_t type data
     * @pre readableSize() >= sizeof(uint16_t)
     * @post position_ += sizeof(uint16_t)
     * @exception std::out_of_range when readableSize() < sizeof(uint16_t)
     * @return The read data
     */
    uint16_t readFuint16();

    /**
     * @brief Read fixed-length int32_t type data
     * @pre readableSize() >= sizeof(int32_t)
     * @post position_ += sizeof(int32_t)
     * @exception std::out_of_range when readableSize() < sizeof(int32_t)
     * @return The read data
     */
    int32_t readFint32();

    /**
     * @brief Read fixed-length uint32_t type data
     * @pre readableSize() >= sizeof(uint32_t)
     * @post position_ += sizeof(uint32_t)
     * @exception std::out_of_range when readableSize() < sizeof(uint32_t)
     * @return The read data
     */
    uint32_t readFuint32();

    /**
     * @brief Read fixed-length int64_t type data
     * @pre readableSize() >= sizeof(int64_t)
     * @post position_ += sizeof(int64_t)
     * @exception std::out_of_range when readableSize() < sizeof(int64_t)
     * @return The read data
     */
    int64_t readFint64();

    /**
     * @brief Read fixed-length uint64_t type data
     * @pre readableSize() >= sizeof(uint64_t)
     * @post position_ += sizeof(uint64_t)
     * @exception std::out_of_range when readableSize() < sizeof(uint64_t)
     * @return The read data
     */
    uint64_t readFuint64();

    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    uint64_t readUint64();

    /**
     * @brief Read fixed-length float type data
     * @pre readableSize() >= sizeof(float)
     * @post position_ += sizeof(float)
     * @exception std::out_of_range when readableSize() < sizeof(float)
     * @return
     */
    float readFloat();

    /**
     * @brief Read fixed-length double type data
     * @pre readableSize() >= sizeof(double)
     * @post position_ += sizeof(double)
     * @exception std::out_of_range when readableSize() < sizeof(double)
     * @return
     */
    double readDouble();

    /**
     * @brief Read std::string type data with uint16_t as data type
     * @pre readableSize() >= sizeof(uint16_t) + size
     * @post position_ += sizeof(uint16_t) + size
     * @exception std::out_of_range when readableSize() < sizeof(uint16_t)
     * @return
     */
    std::string readStringF16();

    /**
     * @brief Read std::string type data with uint32_t as data type
     * @pre readableSize() >= sizeof(uint32_t) + size
     * @post position_ += sizeof(uint32_t) + size
     * @exception std::out_of_range when readableSize() < sizeof(uint32_t)
     * @return
     */
    std::string readStringF32();

    /**
     * @brief Read std::string type data with uint64_t as data type
     * @pre readableSize() >= sizeof(uint64_t) + size
     * @post position_ += sizeof(uint64_t) + size
     * @exception std::out_of_range when readableSize() < sizeof(uint64_t)
     * @return
     */
    std::string readStringF64();

    std::string readStringVint();

    /**
     * @brief Read size bytes data from position to buf
     * @param[out] buf output buffer
     * @param[in] size data length
     * @exception if size_ < position + size, throw std::out_of_range
     */
    void read(void* buf, size_t size);

    /**
     * @brief Read size bytes data from position to buf
     * @param[out] buf output buffer
     * @param[in] size data length
     * @param[in] position start position
     * @exception if size_ < position + size, throw std::out_of_range
     */
    void read(void* buf, size_t size, size_t position) const;

    size_t position() const { return position_; }
    void setPosition(size_t v);
    size_t baseSize() const { return baseSize_; }
    size_t size() const { return size_; }
    size_t readableSize() const { return size_ - position_; }

    bool isLittleEndian() const { return endian_ == LUTE_LITTLE_ENDIAN; }
    void setLittleEndian(bool val) {
        endian_ = val ? LUTE_LITTLE_ENDIAN : LUTE_BIG_ENDIAN;
    }

    std::string toString() const;
    std::string toHexString() const;

    ///
    /// @brief Get the Readable Buffers, it will be save into iovec
    ///
    /// @param buffers iovec
    /// @param len The length of the data to be read if len > readableSize(), or
    /// readableSize()
    /// @return uint64_t Real readable size
    ///
    uint64_t readableBuffers(std::vector<iovec>& buffers,
                             uint64_t len = ~0ull) const;
    uint64_t readableBuffers(std::vector<iovec>& buffers, uint64_t len,
                             uint64_t position) const;

    uint64_t writableBuffers(std::vector<iovec>& buffers, uint64_t len);
    ///
    /// @brief 从文件中读取数据到 ByteArray
    ///
    bool readFromFile(const std::string_view& name);
    bool writeToFile(const std::string_view& name) const;

    /**
     * @brief Clear the ByteArray
     * @post position_ = 0, size_ = 0
     */
    void clear();

private:
    ///
    /// @brief 扩容 ByteArray，扩容后的容量为 size
    ///
    void ensureCapacity(size_t size);
    ///
    /// @brief 当前剩余可写容量
    ///
    size_t writableCapacity() const { return capacity_ - position_; }

    /// 内存块大小
    size_t baseSize_;
    /// 当前操作位置
    size_t position_;
    /// 总容量
    size_t capacity_;
    /// 当前数据大小
    size_t size_;
    /// 字节序
    int8_t endian_;
    /// 第一个内存块
    Node* root_;
    /// 当前操作的内存块指针
    Node* curr_;
};
}  // namespace Lute