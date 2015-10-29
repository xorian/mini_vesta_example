#if !defined(_BIT_WIDTH_HPP)
#define _BIT_WIDTH_HPP

#include <cstdint>
#include <type_traits>

// Compile time selection of an integer type that can hold "_width"
// bits.  Note: If _width is greater than 32 (or 64 on native 64-bit
// systems) the type will actually be "void" which will likely cause
// compilation errors elsewhere.

template<unsigned int _width> class BitWidth
{
public:
  // ::uint is the unsigned integer type
  typedef typename std::conditional<(_width <= 8u), std::uint8_t,
    typename std::conditional<(_width <= 16u), std::uint16_t,
      typename std::conditional<(_width <= 32u), std::uint32_t,
	// As long as they haven't asked for more than 64 bits and
	// this platform has 64-bit pointers, provide 64-bit integer
	// type.  Make the type void if the width is too large (which
	// will likely cause compilation errors elsewhere as void
	// can't eb used as an integer)
        typename std::conditional<((_width <= 64u)
				   && (sizeof(void*) > sizeof(std::uint32_t))),
				  std::uint64_t, void>::type
        >::type
      >::type
    >::type uint;

  // ::width has a copy of size for convenience
  static const unsigned int width = _width;
};

#endif
