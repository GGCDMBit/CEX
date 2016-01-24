#ifndef _CEXENGINE_CONFIG_H
#define _CEXENGINE_CONFIG_H

// define endianess of CPU
#if !defined(IS_LITTLE_ENDIAN) && (defined(__sparc) || defined(__sparc__) || defined(__hppa__) || defined(__PPC__) || defined(__mips__) || (defined(__MWERKS__) && !defined(__INTEL__)))
#	define IS_BIG_ENDIAN
#else
#	define IS_LITTLE_ENDIAN
#endif

// get register size
#if defined(__GNUC__) || defined(__MWERKS__)
#	define WORD64_AVAILABLE
#	define W64LIT(x) x##LL
#elif defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#	define WORD64_AVAILABLE
#	define W64LIT(x) x##ui64
#endif

// not a 64-bit CPU
#if !defined(WORD64_AVAILABLE) && !defined(__alpha)
#	define SLOW_WORD64
#endif

// define universal data types
typedef unsigned char byte;

// uint should be the size as CPU registers, ulong *=2, short /=2
#if (defined(__GNUC__) && !defined(__alpha)) || defined(__MWERKS__)
typedef unsigned int ushort;
typedef unsigned long uint;
typedef unsigned long long ulong;
#elif defined(_MSC_VER) || defined(__BCPLUSPLUS__)
typedef unsigned __int16 ushort;
typedef unsigned __int32 uint;
typedef unsigned __int64 ulong;
#else
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#endif

// store word size
const unsigned int WORD_SIZE = sizeof(uint);
const unsigned int WORD_BITS = WORD_SIZE * 8;

/*// dword union
union dword_union
{
	dword_union(const ulong &dw) : dw(dw) {}
	ulong dw;
	uint w[2];
};

// endian-wise words
#define LOW_WORD(x) (uint)(x)
#ifdef IS_LITTLE_ENDIAN
#	define HIGH_WORD(x) (dword_union(x).w[1])
#else
#	define HIGH_WORD(x) (dword_union(x).w[0])
#endif*/

// intrensics flags
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#	define INTEL_INTRINSICS
#	define FAST_ROTATE
#elif defined(__MWERKS__) && TARGET_CPU_PPC
#	define PPC_INTRINSICS
#	define FAST_ROTATE
#elif defined(__GNUC__) && defined(__i386__)
// GCC does peephole optimizations which should result in using rotate instructions
#	define FAST_ROTATE
#endif

#endif