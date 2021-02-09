#ifndef FLOAT_CMP_HPP
#define FLOAT_CMP_HPP

#include "type_with_size.hpp"
#include <limits>

// std::abs is non constexpr
template<typename T>
constexpr T absolute_value(T const& v)
{
  return v >= T{} ? v : -v;
}

template<typename float_type>
class IEEE_754
{
public:

  // Constants

  // An unsigned integer of the same size as the floating point number.
  // Used to easily access the raw bit data of the float.
  using bit_data = typename type_with_size<sizeof(float_type)>::t_uint;

  // Total number of bits in the float.
  static inline constexpr size_t bitCount = 8 * sizeof(float_type);

  // Number of bits for the significand in a float.
  static inline constexpr size_t significandBitCount = std::numeric_limits<float_type>::digits - 1;

  // Number of bits for the exponent in a float. The 1 is for the sign bit.
  static inline constexpr size_t exponentBitCount = bitCount - 1 - significandBitCount;

  // Mask for the sign bit, which is the most significant bit in IEEE-754.
  static inline constexpr bit_data signBitMask = bit_data(1) << (bitCount - 1);

  // Mask for the significand bits, which are at the least significant end in IEEE-754.
  static inline constexpr bit_data significandBitMask = ~bit_data(0) >> (exponentBitCount + 1);

  // Mask for the exponent bits, which are between sign and significand bits in IEEE-754.
  static inline constexpr bit_data exponentBitMask = ~(signBitMask | significandBitMask);

  // Standard float epsilon, for numbers close to 0.
  static inline constexpr float_type maxDiff = std::numeric_limits<float_type>::epsilon();

  // Industry standard(?) number of ULPs to consider close enough
  static inline constexpr size_t maxUlpsDiff = 4;

  // Constructor

  // Construct from a raw floating point value.
  constexpr IEEE_754(float_type const& f) : data_({ f }) {}

  constexpr IEEE_754(float_type const&& f) : data_({ f }) {}

  // Construct from another IEEE-754 representation class.
  constexpr IEEE_754(IEEE_754 const&& rhs) : data_({ rhs.float_data() }) {}

  // Conversion operator to a raw floating point value, to enable using test floats in expressions.
  explicit operator float_type const&() const
  {
    return data_.float_;
  }

  // Conversion operator to a raw floating point value, to enable using test floats in expressions.
  explicit operator float_type& ()
  {
      return data_.float_;
  }

  // Non-static methods

  // Returns the exponent bits of this float.
  constexpr bit_data exponent_bits() const
  {
    return exponentBitMask & data_.bit_;
  }

  // Returns the fraction bits of this float.
  constexpr bit_data significand_bits() const
  {
    return significandBitMask & data_.bit_;
  }

  // Returns the sign bit of this float.
  constexpr bit_data sign_bit() const
  {
    return signBitMask & data_.bit_;
  }

  constexpr float_type float_data() const
  {
    return data_.float_;
  }

  constexpr bit_data bit_data() const
  {
    return data_.bit_;
  }

  // Returns whether a float represents NaN.
  constexpr bool is_nan() const
  {
    // The IEEE-754 standard defines a NaN as a number with all ones in the exponent, and a non-zero significand.
    return (exponent_bits() == exponentBitMask) && (significand_bits() != 0);
  }

  constexpr bool float_close(float_type const& lhs, float_type const& rhs) const
  {
    return absolute_value(lhs - rhs) <= maxDiff;
  }

  constexpr bool ulp_close(bit_data const& lhs, bit_data const& rhs) const
  {
    return lhs - rhs <= maxUlpsDiff;
  }

  // Tests whether two floats are equal or close enough to be considered equal.
  constexpr bool almost_equal(IEEE_754<float_type> const && rhs) const
  {
    // NaN are not equal to anything, including other NaN.
    if (is_nan() || rhs.is_nan())
    {
      return false;
    }

    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    if (float_close(data_.float_, rhs.data_.float_))
    {
      return true;
    }
    
    // Numbers cannot be equal if they do not share the same sign.
    if (sign_bit() != rhs.sign_bit())
    {
      return false;
    }
    
    // Find the difference in ULPs.
    return ulp_close(data_.bit_, rhs.data_.bit_);
  }

  // Operator overloads

  constexpr bool operator==(IEEE_754<float_type> const&& rhs) const
  {
    return almost_equal(std::forward<decltype(rhs)>(rhs));
  }

  constexpr bool operator==(float_type const&& rhs) const
  {
    return almost_equal(IEEE_754<float_type>(rhs));
  }

  template<typename float_type>
  friend constexpr bool operator==(float_type const&& lhs, IEEE_754<float_type> const&& rhs);

private:

  union data
  {
    constexpr data(float_type f) : float_(f) {}
    mutable float_type float_; // mutable so that runtime version can be used
            bit_data   bit_;
  };

  const data data_;
};

template<typename float_type>
constexpr bool operator==(float_type const&& lhs, IEEE_754<float_type> const&& rhs)
{
  return rhs == lhs;
}

using float_cmp = IEEE_754<float>;
using double_cmp = IEEE_754<double>;

#endif // FLOAT_CMP_HPP
