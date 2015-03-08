/**
 * Common definitions and macros.
 */

#include <stdbool.h>
#include <stdint.h>

#ifndef COMMON_H
#define COMMON_H

/* Array size */

#define ARSIZE(a) ((sizeof(a))/(sizeof((a)[0])))

/* Boolean macros */

#define BOOL(x) ((x) != 0)

/* Bit manipulation */

#define BIT(n) (1<<(n))

#define POW_2(n) BIT(n)

typedef union {
	uint8_t v;
	struct {
		int b0:1;
		int b1:1;
		int b2:1;
		int b3:1;
		int b4:1;
		int b5:1;
		int b6:1;
		int b7:1;
	};
} __attribute__((__packed__)) bits8;

typedef union {
	uint16_t v;
	struct {
		int b0:1;
		int b1:1;
		int b2:1;
		int b3:1;
		int b4:1;
		int b5:1;
		int b6:1;
		int b7:1;
		int b8:1;
		int b9:1;
		int b10:1;
		int b11:1;
		int b12:1;
		int b13:1;
		int b14:1;
		int b15:1;
	};
} __attribute__((__packed__)) bits16;

typedef union {
	uint16_t v;
	struct {
		int b0:1;
		int b1:1;
		int b2:1;
		int b3:1;
		int b4:1;
		int b5:1;
		int b6:1;
		int b7:1;
		int b8:1;
		int b9:1;
		int b10:1;
		int b11:1;
		int b12:1;
		int b13:1;
		int b14:1;
		int b15:1;
		int b16:1;
		int b17:1;
		int b18:1;
		int b19:1;
		int b20:1;
		int b21:1;
		int b22:1;
		int b23:1;
		int b24:1;
		int b25:1;
		int b26:1;
		int b27:1;
		int b28:1;
		int b29:1;
		int b30:1;
		int b31:1;
	};
} __attribute__((__packed__)) bits32;

/* Arithmetic macros */

/* Round-up division (UNSAFE)
 * Performs ceil(a,b), using integer arithmetic only.
 */
#define DIV_CEIL(a,b) (((a) % (b) == 0) ? ((a)/(b)) : (((a)/(b))+1))

/* Use this macro to select between rom and non-rom functions */

#ifdef USE_ROM_FUNCTIONS
#define R_(f) ROM_##f
#else
#define R_(f) f
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif /* COMMON_H */
