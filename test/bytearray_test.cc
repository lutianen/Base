#include <Base/bytearray.h>

#include <iostream>

void test() {
#define XX(type, len, writeFun, readFun, baseLen)                      \
    {                                                                  \
        std::vector<type> vec;                                         \
        for (int i = 0; i < len; ++i) {                                \
            vec.push_back(rand());                                     \
        }                                                              \
        Lute::ByteArray::ptr ba(new Lute::ByteArray(baseLen));         \
        for (auto& i : vec) {                                          \
            ba->writeFun(i);                                           \
        }                                                              \
        ba->setPosition(0);                                            \
        for (size_t i = 0; i < vec.size(); ++i) {                      \
            type v = ba->readFun();                                    \
            assert(v == vec[i]);                                  \
        }                                                              \
        assert(ba->readableSize() == 0);                          \
        std::cout << #writeFun "/" #readFun " (" #type ") len=" << len \
                  << " baseLen=" << baseLen << " size=" << ba->size()  \
                  << std::endl;                                        \
    }

    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX

#define XX(type, len, writeFun, readFun, baseLen)                           \
    {                                                                       \
        std::vector<type> vec;                                              \
        for (int i = 0; i < len; ++i) {                                     \
            vec.push_back(rand());                                          \
        }                                                                   \
        Lute::ByteArray::ptr ba(new Lute::ByteArray(baseLen));              \
        for (auto& i : vec) {                                               \
            ba->writeFun(i);                                                \
        }                                                                   \
        ba->setPosition(0);                                                 \
        for (size_t i = 0; i < vec.size(); ++i) {                           \
            type v = ba->readFun();                                         \
            assert(v == vec[i]);                                       \
        }                                                                   \
        assert(ba->readableSize() == 0);                               \
        std::cout << #writeFun "/" #readFun " (" #type ") len=" << len      \
                  << " baseLen=" << baseLen << " size=" << ba->size()       \
                  << std::endl;                                             \
        ba->setPosition(0);                                                 \
        assert(                                                        \
            ba->writeToFile("/tmp/" #type "_" #len "-" #readFun ".dat"));   \
        Lute::ByteArray::ptr ba2(new Lute::ByteArray(baseLen * 2));         \
        assert(                                                        \
            ba2->readFromFile("/tmp/" #type "_" #len "-" #readFun ".dat")); \
        ba2->setPosition(0);                                                \
        assert(ba->toString() == ba2->toString());                     \
        assert(ba->position() == 0);                                   \
        assert(ba2->position() == 0);                                  \
    }

    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX
}

int main() {
    test();
    Lute::ByteArray::ptr ba(new Lute::ByteArray(10));

    ba->writeFloat(1.234f);
    ba->setPosition(0);
    std::cout << ba->readFloat() << std::endl;
    assert(ba->readableSize() == 0);
    ba->clear();

    ba->writeDouble(1.234);
    ba->setPosition(0);
    std::cout << ba->readDouble() << std::endl;
    assert(ba->readableSize() == 0);
    ba->clear();

    Lute::string_view sva = "Hello";
    ba->writeStringF16(sva);
    ba->setPosition(0);
    std::cout << ba->readStringF16() << std::endl;
    assert(ba->readableSize() == 0);
    ba->clear();

    Lute::string_view svb = "Lute";
    ba->writeStringF32(svb);
    ba->setPosition(0);
    std::cout << ba->readStringF32() << std::endl;
    assert(ba->readableSize() == 0);
    ba->clear();

    Lute::string_view svc = "Polaris";
    ba->writeStringF64(svc);
    ba->setPosition(0);
    std::cout << ba->readStringF64() << std::endl;
    assert(ba->readableSize() == 0);
    ba->clear();

    return 0;
}