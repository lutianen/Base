#include <gtest/gtest.h>
#include <Base/md5.h>

using md5_state_t = Lute::md5::md5_state_t;
using md5_byte_t = Lute::md5::md5_byte_t;

static const char *const test[7] = {
    "",                           /*d41d8cd98f00b204e9800998ecf8427e*/
    "a",                          /*0cc175b9c0f1b6a831c399e269772661*/
    "abc",                        /*900150983cd24fb0d6963f7d28e17f72*/
    "message digest",             /*f96b697d7cb7938d525a2f31aaf161d0*/
    "abcdefghijklmnopqrstuvwxyz", /*c3fcd3d76192e4007dfb496cca67e13b*/
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    /*d174ab98d277d9f5a5611c2c9f419d9f*/
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890" /*57edf4a22be3c955ac49da2e2107b67a*/
};

TEST(md5, md5_hash_string) {
  for (size_t i = 0; i < 7; ++i) {
    EXPECT_EQ(
        Lute::md5::md5_hash_string(test[i]),
        Lute::md5::md5_hash_string(reinterpret_cast<const uint8_t *>(test[i]),
                                   std::strlen(test[i])));
  }
}

TEST(md5, md5_hash_hex) {
  EXPECT_EQ(Lute::md5::md5_hash_hex(test[0]),
            "d41d8cd98f00b204e9800998ecf8427e");
  EXPECT_EQ(Lute::md5::md5_hash_hex(test[1]),
            "0cc175b9c0f1b6a831c399e269772661");
  EXPECT_EQ(Lute::md5::md5_hash_hex(test[2]),
            "900150983cd24fb0d6963f7d28e17f72");
  EXPECT_EQ(Lute::md5::md5_hash_hex(test[3]),
            "f96b697d7cb7938d525a2f31aaf161d0");
  EXPECT_EQ(Lute::md5::md5_hash_hex(test[4]),
            "c3fcd3d76192e4007dfb496cca67e13b");
  EXPECT_EQ(Lute::md5::md5_hash_hex(test[5]),
            "d174ab98d277d9f5a5611c2c9f419d9f");
  EXPECT_EQ(Lute::md5::md5_hash_hex(test[6]),
            "57edf4a22be3c955ac49da2e2107b67a");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
