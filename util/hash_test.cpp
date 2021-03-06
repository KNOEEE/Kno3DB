#include "util/hash.h"

#include <cstring>
#include <vector>

#include "test_util/testharness.h"
#include "util/coding_lean.h"


// The hash algorithm is part of the file format, for example for the Bloom
// filters. Test that the hash values are stable for a set of random strings of
// varying lengths.
void test_values() {
  constexpr uint32_t kSeed = 0xbc9f1d34;  // Same as BloomHash.

  EXPECT_EQ(Hash("", 0, kSeed), 3164544308u);
  EXPECT_EQ(Hash("\x08", 1, kSeed), 422599524u);
  EXPECT_EQ(Hash("\x17", 1, kSeed), 3168152998u);
  EXPECT_EQ(Hash("\x9a", 1, kSeed), 3195034349u);
  EXPECT_EQ(Hash("\x1c", 1, kSeed), 2651681383u);
  EXPECT_EQ(Hash("\x4d\x76", 2, kSeed), 2447836956u);
  EXPECT_EQ(Hash("\x52\xd5", 2, kSeed), 3854228105u);
  EXPECT_EQ(Hash("\x91\xf7", 2, kSeed), 31066776u);
  EXPECT_EQ(Hash("\xd6\x27", 2, kSeed), 1806091603u);
  EXPECT_EQ(Hash("\x30\x46\x0b", 3, kSeed), 3808221797u);
  EXPECT_EQ(Hash("\x56\xdc\xd6", 3, kSeed), 2157698265u);
  EXPECT_EQ(Hash("\xd4\x52\x33", 3, kSeed), 1721992661u);
  EXPECT_EQ(Hash("\x6a\xb5\xf4", 3, kSeed), 2469105222u);
  EXPECT_EQ(Hash("\x67\x53\x81\x1c", 4, kSeed), 118283265u);
  EXPECT_EQ(Hash("\x69\xb8\xc0\x88", 4, kSeed), 3416318611u);
  EXPECT_EQ(Hash("\x1e\x84\xaf\x2d", 4, kSeed), 3315003572u);
  EXPECT_EQ(Hash("\x46\xdc\x54\xbe", 4, kSeed), 447346355u);
  EXPECT_EQ(Hash("\xd0\x7a\x6e\xea\x56", 5, kSeed), 4255445370u);
  EXPECT_EQ(Hash("\x86\x83\xd5\xa4\xd8", 5, kSeed), 2390603402u);
  EXPECT_EQ(Hash("\xb7\x46\xbb\x77\xce", 5, kSeed), 2048907743u);
  EXPECT_EQ(Hash("\x6c\xa8\xbc\xe5\x99", 5, kSeed), 2177978500u);
  EXPECT_EQ(Hash("\x5c\x5e\xe1\xa0\x73\x81", 6, kSeed), 1036846008u);
  EXPECT_EQ(Hash("\x08\x5d\x73\x1c\xe5\x2e", 6, kSeed), 229980482u);
  EXPECT_EQ(Hash("\x42\xfb\xf2\x52\xb4\x10", 6, kSeed), 3655585422u);
  EXPECT_EQ(Hash("\x73\xe1\xff\x56\x9c\xce", 6, kSeed), 3502708029u);
  EXPECT_EQ(Hash("\x5c\xbe\x97\x75\x54\x9a\x52", 7, kSeed), 815120748u);
  EXPECT_EQ(Hash("\x16\x82\x39\x49\x88\x2b\x36", 7, kSeed), 3056033698u);
  EXPECT_EQ(Hash("\x59\x77\xf0\xa7\x24\xf4\x78", 7, kSeed), 587205227u);
  EXPECT_EQ(Hash("\xd3\xa5\x7c\x0e\xc0\x02\x07", 7, kSeed), 2030937252u);
  EXPECT_EQ(Hash("\x31\x1b\x98\x75\x96\x22\xd3\x9a", 8, kSeed), 469635402u);
  EXPECT_EQ(Hash("\x38\xd6\xf7\x28\x20\xb4\x8a\xe9", 8, kSeed), 3530274698u);
  EXPECT_EQ(Hash("\xbb\x18\x5d\xf4\x12\x03\xf7\x99", 8, kSeed), 1974545809u);
  EXPECT_EQ(Hash("\x80\xd4\x3b\x3b\xae\x22\xa2\x78", 8, kSeed), 3563570120u);
  EXPECT_EQ(Hash("\x1a\xb5\xd0\xfe\xab\xc3\x61\xb2\x99", 9, kSeed),
            2706087434u);
  EXPECT_EQ(Hash("\x8e\x4a\xc3\x18\x20\x2f\x06\xe6\x3c", 9, kSeed),
            1534654151u);
  EXPECT_EQ(Hash("\xb6\xc0\xdd\x05\x3f\xc4\x86\x4c\xef", 9, kSeed),
            2355554696u);
  EXPECT_EQ(Hash("\x9a\x5f\x78\x0d\xaf\x50\xe1\x1f\x55", 9, kSeed),
            1400800912u);
  EXPECT_EQ(Hash("\x22\x6f\x39\x1f\xf8\xdd\x4f\x52\x17\x94", 10, kSeed),
            3420325137u);
  EXPECT_EQ(Hash("\x32\x89\x2a\x75\x48\x3a\x4a\x02\x69\xdd", 10, kSeed),
            3427803584u);
  EXPECT_EQ(Hash("\x06\x92\x5c\xf4\x88\x0e\x7e\x68\x38\x3e", 10, kSeed),
            1152407945u);
  EXPECT_EQ(Hash("\xbd\x2c\x63\x38\xbf\xe9\x78\xb7\xbf\x15", 10, kSeed),
            3382479516u);
}

// ??????????????????????????????
// undefined reference to `Hash(char const*, unsigned long, unsigned int)'
// ???????????????????????????
// g++ -c hash.cpp -I../ -I../include
// g++ -c hash_test.cpp -I ../ -I../include
// g++ -o hash_test hash_test.o hash.o
// ref: https://blog.csdn.net/aiwoziji13/article/details/7330333
// ??????????????????????????????????????????????????????????????? ????????????
// ??????makefile??? ?????????blade
int main(int argc, char** argv) {
  fprintf(stderr, "test hash32\n");
  test_values();
  return 0;
}