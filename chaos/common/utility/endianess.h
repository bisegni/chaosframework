//
//  endianess.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 20/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//
//http://stackoverflow.com/a/3522853/481864
#ifndef CHAOSFramework_endianess_h
#define CHAOSFramework_endianess_h

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/detail/endian.hpp>
#include <stdexcept>
#include <stdint.h>


namespace chaos {
	namespace common {
		namespace utility {
			
			enum endianness
			{
    little_endian,
    big_endian,
    network_endian = big_endian,
    
#if defined(BOOST_LITTLE_ENDIAN)
    host_endian = little_endian
#elif defined(BOOST_BIG_ENDIAN)
    host_endian = big_endian
#else
#error "unable to determine system endianness"
#endif
			};
			
			template<typename T, size_t sz>
			struct swap_bytes {
				inline T operator()(T val) {
					throw std::out_of_range("data size");
				}
			};
			
			template<typename T>
			struct swap_bytes<T, 1> {
				inline T operator()(T val) {
					return val;
				}
			};
			
			template<typename T>
			struct swap_bytes<T, 2> {
				inline T operator()(T val) {
					return ((((val) >> 8) & 0xff) | (((val) & 0xff) << 8));
				}
			};
			
			template<typename T>
			struct swap_bytes<T, 4> {
				inline T operator()(T val) {
					return ((((val) & 0xff000000) >> 24) |
							(((val) & 0x00ff0000) >>  8) |
							(((val) & 0x0000ff00) <<  8) |
							(((val) & 0x000000ff) << 24));
				}
			};
			
			template<>
			struct swap_bytes<float, 4> {
				inline float operator()(float val) {
					uint32_t mem =swap_bytes<uint32_t, sizeof(uint32_t)>()(*(uint32_t*)&val);
					return *(float*)&mem;
				}
			};
			
			template<typename T>
			struct swap_bytes<T, 8> {
					inline T operator()(T val) {
					return ((((val) & 0xff00000000000000ull) >> 56) |
							(((val) & 0x00ff000000000000ull) >> 40) |
							(((val) & 0x0000ff0000000000ull) >> 24) |
							(((val) & 0x000000ff00000000ull) >> 8 ) |
							(((val) & 0x00000000ff000000ull) << 8 ) |
							(((val) & 0x0000000000ff0000ull) << 24) |
							(((val) & 0x000000000000ff00ull) << 40) |
							(((val) & 0x00000000000000ffull) << 56));
				}
			};
			
			template<>
			struct swap_bytes<double, 8> {
				inline double operator()(double val) {
					uint64_t mem =swap_bytes<uint64_t, sizeof(uint64_t)>()(*(uint64_t*)&val);
					return *(double*)&mem;
				}
			};
			
			template<endianness from, endianness to, class T>
			struct do_byte_swap {
				inline T operator()(T value) {
					return swap_bytes<T, sizeof(T)>()(value);
				}
			};
			// specialisations when attempting to swap to the same endianess
			template<class T> struct do_byte_swap<little_endian, little_endian, T> { inline T operator()(T value) { return value; } };
			template<class T> struct do_byte_swap<big_endian,    big_endian,    T> { inline T operator()(T value) { return value; } };
			
			template<endianness from, endianness to, class T>
			inline T byte_swap(T value) {
				// ensure the data is only 1, 2, 4 or 8 bytes
    BOOST_STATIC_ASSERT(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
				// ensure we're only swapping arithmetic types
    BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value);
    
    return do_byte_swap<from, to, T>()(value);
			}
#define  TO_LITTEL_ENDNS_NUM(type, n) chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, type>(n)
#define  TO_LITTE_ENDNS(type, ptr, offset) chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, type>(*((type*)((char*)ptr)+offset))
#define  FROM_LITTLE_ENDNS(type, ptr, offset) chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian,  type>(*((type*)((char*)ptr)+offset))
#define  FROM_LITTLE_ENDNS_NUM(type, n) chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian, type>(n)
#define	 GO_TO_OFFSET(ptr, off) static_cast<char*>(ptr)+off
		}
	}
}


#endif
