/*
 * sqrt.c
 *
 * by Juan I Carrano
 * <juan@carrano.com.ar>
 */

#include <fixed_point/fixed_point.h>

#define TBL_SZ_BITS 4
#define TBL_SZ (1<<TBL_SZ_BITS)
#define TBL_MASK ((unsigned short)(~((0xFFFF) >> (TBL_SZ_BITS+1))))
#define F_TO_TBL(x, tbl) ((tbl)[((x)&TBL_MASK) >> (16 - TBL_SZ_BITS-1)])
#define F_TO_INVTBL(x) F_TO_TBL(x, invtbl)
#define F_TO_STARTTBL(x) F_TO_TBL(x, starttbl)

frac f_sqrt(frac b, int iters)
{
	static const int invtbl[TBL_SZ] = {
		248, 83, 50, 35, 28, 23, 19, 17, 15, 13, 12, 11, 10, 9, 9, 8
	};
	static const int starttbl[TBL_SZ] = {
		4885, 10194, 13160, 15571, 17656, 19519, 21220, 22794, 24266,
		25653, 26970, 28225, 29427, 30581, 31693, 32251
	};

	frac x = _frac(F_TO_STARTTBL(b.v));
	int i;

	for (i = 0; i < iters; i++) {
		x.v = ((b.v - f_mul(x,x).v)*F_TO_INVTBL(x.v))/16 + x.v;
	}

	return x;
}


