#pragma once

namespace crt {
	inline void* memcpy(
		void* dest,
		const void* src,
		size_t len
	) {
		char* d = ( char* )dest;
		const char* s = ( const char* )src;
		while ( len-- )
			*d++ = *s++;
		return dest;
	}

	inline int memcmp(
		const void* s1,
		const void* s2,
		size_t n
	) {
		const unsigned char* p1 = ( const unsigned char* )s1;
		const unsigned char* end1 = p1 + n;
		const unsigned char* p2 = ( const unsigned char* )s2;
		int                   d = 0;
		for ( ;;) {
			if ( d || p1 >= end1 ) break;
			d = ( int )*p1++ - ( int )*p2++;
			if ( d || p1 >= end1 ) break;
			d = ( int )*p1++ - ( int )*p2++;
			if ( d || p1 >= end1 ) break;
			d = ( int )*p1++ - ( int )*p2++;
			if ( d || p1 >= end1 ) break;
			d = ( int )*p1++ - ( int )*p2++;
		}
		return d;
	}
}