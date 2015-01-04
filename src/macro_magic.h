/**
 * Magical macro definitions.
 *
 * This macros are used to create other macros.
 *
 * @author Juan I Carrano
 * @author Nicolás Venturo
 *
 * @copyright Copyright 2014 Juan I Carrano, Andrés Calcabrini, Juan I Ubeira,
 *              Nicolás venturo.
 */

#ifndef MACRO_MAGIC_H
#define MACRO_MAGIC_H

/* discard arguments */

#define MDUMMY(p)

/* Count the number of arguments
 * This macro expands to the number of arguments passed.
 * Supports a maximum of 20 parameters.
 * It does NOT support 0 arguments (it can be done in GCC but it is non portable)
 */

#define _N_ARGS(_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,\
	_4,_3,_2,_1,N,...)  (N)

#define N_ARGS(...) \
	_N_ARGS(__VA_ARGS__, 20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

/* Polymorphic macros: Use this to make macros that can do COMPLETELY different
 * things depending on the number of parameters passed */

/* How to use:
 * Define your arternatives. All of them must take the same number of arguments.
 * Those that take less arguments should use dummy args:
 *
 * LIMITATION: Cant be used with macros that take zero arguments !!!!
 *
 * Example:
 *	#define PIN_CFG4(port, pin, type, func) f(P1##port##pin##type##func)
 *
 *	#define PIN_CFG3(port, pin, type, _0) g(NN##port##pin##type)
 *
 *
 * To define your polymorphic macro, use one of the _POLYARGSn macros, where n
 * is the greatest number of arguments your macro can take.
 * Define your macro as a variadic macro that call _POLYARGSn with __VA_ARGS__
 * as the first argument, followed my the name of the macro that uses all n
 * arguments, then the one that uses n-1, etc. You DONT NEED to go all the way
 * up to n = 2.
 *
 * Example:
 * 	#define PIN_CFG(...) _POLYARGS4(__VA_ARGS__, PIN_CFG4,PIN_CFG3)
 *
 */

/* MDUMMY is used to suppress warnings caused by not using all arguments */

#define _POLYARGS2(_1,_2, F, ...) F(_1,_2) MDUMMY((__VA_ARGS__))
#define _POLYARGS3(_1,_2,_3, F, ...) F(_1,_2,_3) MDUMMY((__VA_ARGS__))
#define _POLYARGS4(_1,_2,_3,_4, F,...) F(_1,_2,_3,_4) MDUMMY((__VA_ARGS__))
#define _POLYARGS5(_1,_2,_3,_4,_5, F,...) F(_1,_2,_3,_4,_5) MDUMMY((__VA_ARGS__))
#define _POLYARGS6(_1,_2,_3,_4,_5,_6, F,...) F(_1,_2,_3,_4,_5,_6) MDUMMY((__VA_ARGS__))
#define _POLYARGS7(_1,_2,_3,_4,_5,_6,_7, F,...) F(_1,_2,_3,_4,_5,_6,_7) MDUMMY((__VA_ARGS__))
#define _POLYARGS8(_1,_2,_3,_4,_5,_6,_7,_8, F,...) F(_1,_2,_3,_4,_5,_6,_7,_8) MDUMMY((__VA_ARGS__))
#define _POLYARGS9(_1,_2,_3,_4,_5,_6,_7,_8,_9, F,...) F(_1,_2,_3,_4,_5,_6,_7,_8,_9) MDUMMY((__VA_ARGS__))

/* Glue: use this macros to create macros "on the fly"
 */

/* Paste 2 tokens */
#define GLUE(a,b) GLUE_AGAIN(a,b)
#define GLUE_AGAIN(a,b) a ## b

/* Paste 3 tokens */
#define GLUE3(a,b,c) GLUE_AGAIN3(a,b,c)
#define GLUE_AGAIN3(a,b,c) a ## b ## c

#endif /* MACRO_MAGIC_H */
