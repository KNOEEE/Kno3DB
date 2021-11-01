// fastrange/FastRange: A faster alternative to % for mapping a hash value
// to an arbitrary range. See https://github.com/lemire/fastrange
//
// Generally recommended are FastRange32 for mapping results of 32-bit
// hash functions and FastRange64 for mapping results of 64-bit hash
// functions. FastRange is less forgiving than % if the input hashes are
// not well distributed over the full range of the type (32 or 64 bits).
//
// Also included is a templated implementation FastRangeGeneric for use
// in generic algorithms, but not otherwise recommended because of
// potential ambiguity. Unlike with %, it is critical to use the right
// FastRange variant for the output size of your hash function.

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace detail {
// 承载未实现的Hash
template <typename Hash, typename Range>
struct FastRangeGenericImpl {};

template <typename Range>
struct FastRangeGenericImpl<uint32_t, Range> {
  static inline Range Fn(uint32_t hash, Range range) {
    static_assert(std::is_unsigned<Range>::value, "must be unsigned");
    static_assert(sizeof(Range) <= sizeof(uint32_t),
                  "can't be larger than hash (32bits)");
    uint64_t product = uint64_t{range} * hash;
    return static_cast<Range>(product >> 32);
  }
};
} // namespace detail

template <typename Hash, typename Range>
inline Range FastRangeGeneric(Hash hash, Range range) {
  return detail::FastRangeGenericImpl<Hash, Range>::Fn(hash, range);
}

inline uint32_t FastRange32(uint32_t hash, uint32_t range) {
  return FastRangeGeneric(hash, range);
}