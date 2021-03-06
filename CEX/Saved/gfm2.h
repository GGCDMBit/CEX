#ifndef _CEX_GFM2_H
#define _CEX_GFM2_H

#include "CexDomain.h"
//#include "params.h"

NAMESPACE_MCELIECE

//typedef uint16_t gf;

/// <summary>
/// 
/// </summary>
class gfm2
{
public:

	static ushort gf_mul(ushort in0, ushort in1, size_t Dimension)
	{
		int i;

		uint32_t tmp;
		uint32_t t0;
		uint32_t t1;
		uint32_t t;

		t0 = in0;
		t1 = in1;

		tmp = t0 * (t1 & 1);

		for (i = 1; i < Dimension; i++)
			tmp ^= (t0 * (t1 & (1 << i)));

		t = tmp & 0x7FC000;
		tmp ^= t >> 9;
		tmp ^= t >> 12;

		t = tmp & 0x3000;
		tmp ^= t >> 9;
		tmp ^= t >> 12;

		return tmp & ((1 << Dimension) - 1);
	}

	static ushort gf_sq(ushort in, size_t Dimension)
	{
		const uint32_t B[] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF };

		uint32_t x = in;
		uint32_t t;

		x = (x | (x << 8)) & B[3];
		x = (x | (x << 4)) & B[2];
		x = (x | (x << 2)) & B[1];
		x = (x | (x << 1)) & B[0];

		t = x & 0x7FC000;
		x ^= t >> 9;
		x ^= t >> 12;

		t = x & 0x3000;
		x ^= t >> 9;
		x ^= t >> 12;

		return x & ((1 << Dimension) - 1);
	}

	static ushort gf_inv(ushort in, size_t Dimension)
	{
		ushort tmp_11;
		ushort tmp_1111;
		ushort out = in;

		out = gf_sq(out, Dimension);
		tmp_11 = gf_mul(out, in, Dimension); // 11

		out = gf_sq(tmp_11, Dimension);
		out = gf_sq(out, Dimension);
		tmp_1111 = gf_mul(out, tmp_11, Dimension); // 1111

		out = gf_sq(tmp_1111, Dimension);
		out = gf_sq(out, Dimension);
		out = gf_sq(out, Dimension);
		out = gf_sq(out, Dimension);
		out = gf_mul(out, tmp_1111, Dimension); // 11111111

		out = gf_sq(out, Dimension);
		out = gf_sq(out, Dimension);
		out = gf_mul(out, tmp_11, Dimension); // 1111111111

		out = gf_sq(out, Dimension);
		out = gf_mul(out, in, Dimension); // 11111111111

		return gf_sq(out, Dimension); // 111111111110
	}

	static ushort gf_diff(ushort a, ushort b)
	{
		uint32_t t = (uint32_t)(a ^ b);

		t = ((t - 1) >> 20) ^ 0xFFF;

		return (ushort)t;
	}

	///////////////////////////////////////////////////////////

	static void GF_mul(std::vector<ushort> &out, std::vector<ushort> &in0, std::vector<ushort> &in1, size_t Dimension)
	{
		int i, j;

		ushort tmp[123];

		for (i = 0; i < 123; i++)
			tmp[i] = 0;

		for (i = 0; i < 62; i++)
		{
			for (j = 0; j < 62; j++)
				tmp[i + j] ^= gf_mul(in0[i], in1[j], Dimension);
		}

		for (i = 122; i >= 62; i--)
		{
			tmp[i - 55] ^= gf_mul(tmp[i], (ushort)1763, Dimension);
			tmp[i - 61] ^= gf_mul(tmp[i], (ushort)1722, Dimension);
			tmp[i - 62] ^= gf_mul(tmp[i], (ushort)4033, Dimension);
		}

		for (i = 0; i < 62; i++)
			out[i] = tmp[i];
	}

};

NAMESPACE_MCELIECEEND
#endif