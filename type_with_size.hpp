#ifndef TYPE_WITH_SIZE_HPP
#define TYPE_WITH_SIZE_HPP

#include <cstdint>

template <unsigned size> struct type_with_size 
{ 
  // Empty by default.  Rely on specializations for details.
  // Unsupported values of size will give compile-time errors on purpose.
};
 
template <> struct type_with_size<1> 
{
  using t_int = int8_t;      // 1 byte signed integer type
  using t_uint = uint8_t;    // 1 byte unsigned integer type
};
 
template <> struct type_with_size<2> 
{
  using t_int = int16_t;     // 2 byte signed integer type
  using t_uint = uint16_t;   // 2 byte unsigned integer type
};
 
template <> struct type_with_size<4> 
{
  using t_int = int32_t;     // 4 byte signed integer type
  using t_uint = uint32_t;   // 4 byte unsigned integer type
  using t_float = float;     // 4 byte floating point type
};
 
template <> struct type_with_size<8> 
{
  using t_int = int64_t;     // 8 byte signed integer type
  using t_uint = uint64_t;   // 8 byte unsigned integer type
  using t_float = double;    // 8 byte floating point type
};

#endif // TYPE_WITH_SIZE_HPP
