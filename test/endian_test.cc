#include <Base/endian.h>

#include <iomanip>  // std::setw, std::setfill
#include <iostream>

#define LUTE_HEX_COUT(x)                                                    \
    std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex << x \
              << '\n';

static uint32_t EncodeZigzag32(const int32_t& v) {
    if (v < 0) {
        return (static_cast<uint32_t>(-v)) * 2 - 1;
    } else {
        return v * 2;
    }
}

static uint32_t EncodeZigzag32_(const int32_t& val) {
    return (val << 1) ^ (val >> 31);
}

int main() {
    uint64_t a = 0x1234567890abcdef;
    uint64_t b = Lute::byteswap(a);

    LUTE_HEX_COUT(a);
    LUTE_HEX_COUT(b);

    size_t cnt = 0;
    for (size_t i = 0; i < 0xFFFFFFFF; ++i) {
        cnt += EncodeZigzag32(i) == EncodeZigzag32_(i) ? 0 : 1;
    }

    std::cout << cnt << std::endl;

    return 0;
}