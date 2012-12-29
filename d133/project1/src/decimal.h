/*----------------------------------------------------------------------------*/
/**
* @pkg DECIMAL
*/
/**
* Handle decimal values.
*
* Handle decimal values.
* (C) T&T, Kiev, Ukraine 2004.
* @pkgdoc DECIMAL
* @author Oleg Yurchenko
* @version 1.0
*/
#ifndef DECIMAL_H
#define DECIMAL_H
#include <stdint.h>

/*
#ifndef __BORLANDC__
	#ifndef __BIT_TYPES_DEFINED__
		typedef long long int64_t;
	#endif
	typedef unsigned long long uint64_t;
#else
	typedef __int64 int64_t;
	typedef unsigned __int64 uint64_t;
#endif //__BORLANDC__

#ifndef __BIT_TYPES_DEFINED__
typedef short int16_t;
#endif
typedef unsigned short uint16_t;

#ifndef __BIT_TYPES_DEFINED__
typedef int int32_t;
#endif
typedef unsigned uint32_t;
*/

typedef unsigned decimals_t;

/**Currency representation*/
typedef struct
{
  int64_t data;
  decimals_t decimals;
} decimal64_t;

/**Float number representation*/
typedef struct
{
  int32_t data;
  decimals_t decimals;
} decimal32_t;

/*
Default:
RETURN values - 0 if Ok, else != 0
*/
#define CUR_OVERFLOAT (-1)
#define CUR_ZERODIVIDE (-2)


#ifndef __BORLANDC__
  #define MAX_INT_64 0x7fffffffffffffffll
  /*9223372036854775807*/
#else
  #define MAX_INT_64 0x7fffffffffffffffi64
  /*9223372036854775807*/
#endif

#define MAX_INT_32 0x7fffffffl
/*2147483647*/
#define MAX_CURRENCY_DECIMALS 19
#define MAX_FLOAT_DECIMALS 9

#ifdef __cplusplus
extern "C"
{
#endif  /*__cplusplus*/

/*Be fast and dirty*/
void set_fast_division(int f);

/**64bit Constructor*/
decimal64_t decimal64_init(int64_t data, decimals_t decimals);
#define CURRENCY_INIT(a,b) decimal64_init((int64_t)(a), (decimals_t)(b))
/**64bit Constructor*/
void int2decimal64(/*IN*/long src, /*OUT*/  decimal64_t *dst);

/**
 * Compare 2 number.
 *
 * @return value: >0 if arg1 > arg2 ;<0 if arg1 < arg2; == 0  if arg1 == arg2
 */
int decimal64_cmp(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2);
/**64bit Add 2 number*/
int decimal64_add(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2, /*OUT*/  decimal64_t *dst);
/**64bit Subtract 2 number*/
int decimal64_sub(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2, /*OUT*/  decimal64_t *dst);
/**64bit Multiply 2 number*/
int decimal64_mul(/*IN*/ const decimal64_t *arg1, /*IN*/ const  decimal64_t *arg2, /*OUT*/  decimal64_t *dst);
/**64bit Divide 2 number*/
int decimal64_div(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2, /*OUT*/  decimal64_t *dst);

/*ROUND*/
/**64bit Math round*/
int decimal64_math_round(/*IN*/ const decimal64_t *arg, decimals_t decimals, /*OUT*/  decimal64_t *dst);
/**64bit Cash round*/
int decimal64_cash_round(/*IN*/ const decimal64_t *arg, decimals_t decimals, /*OUT*/  decimal64_t *dst);
/**64bit Cut round*/
int decimal64_cut_round(/*IN*/ const decimal64_t *arg, decimals_t decimals, /*OUT*/  decimal64_t *dst);

void decimal64_abs(/*IN*/ const decimal64_t *arg, /*OUT*/  decimal64_t *dst);

#define decimal64_round decimal64_cash_round

/*ASCII CONVERSIONS*/
/**64bit Convert number to ascii string*/
int decimal64_str(/*IN*/ const decimal64_t *arg, /*OUT*/ char *dst, /*IN*/unsigned buf_size);
/**64bit Create number from ascii string*/
int str_decimal64(/*IN*/const char *src, /*OUT*/ decimal64_t *dst);
/*----------------------------------------------------------------------------*/
/* 32 -bit routines */
/*----------------------------------------------------------------------------*/
/**32bit Constructor*/
decimal32_t decimal32_init(int32_t data, decimals_t decimals);
#define FLOAT_INIT(a,b) decimal32_init((int32_t)(a), (decimals_t)(b))
/**32bit number to 64bit converson*/
int decimal32_64( const decimal32_t *src,  decimal64_t *dst);
/**64bit number to 32bit converson*/
int decimal64_32( const decimal64_t *src,  decimal32_t *dst);
/*----------------------------------------------------------------------------*/
/*ARIFMETIC*/
/**
 * 32bit Compare 2 number.
 *
 * @return value: >0 if arg1 > arg2 ;<0 if arg1 < arg2; == 0  if arg1 == arg2
 */
int decimal32_cmp(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2);
/*RETURN value: >0 if arg1 > arg2 ;<0 if arg1 < arg2; == 0  if arg1 == arg2*/

/**32bit Add 2 number*/
int decimal32_add(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst);
/**32bit Subtract 2 number*/
int decimal32_sub(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst);
/**32bit Multiply 2 number*/
int decimal32_mul(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst);
/**32bit Divide 2 number*/
int decimal32_div(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst);

/*ROUND*/
/**32bit Math round*/
int decimal32_math_round(/*IN*/ const decimal32_t *arg, decimals_t decimals, /*OUT*/  decimal32_t *dst);
/**32bit Cash round*/
int decimal32_cash_round(/*IN*/ const decimal32_t *arg, decimals_t decimals, /*OUT*/  decimal32_t *dst);
/**32bit Cut round*/
int decimal32_cut_round(/*IN*/ const decimal32_t *arg, decimals_t decimals, /*OUT*/  decimal32_t *dst);

void decimal32_abs(/*IN*/ const decimal32_t *arg, /*OUT*/  decimal32_t *dst);

#define decimal32_round decimal32_cash_round
/*ASCII CONVERSIONS*/
/**32bit Convert number to ascii string*/
int decimal32_str(/*IN*/ const decimal32_t *arg, /*OUT*/ char *dst, /*IN*/unsigned buf_size);
/**32bit Create number from ascii string*/
int str_decimal32(/*IN*/const char *src, /*OUT*/ decimal32_t *dst);
/*----------------------------------------------------------------------------*/
void decimal64_strip(decimal64_t *arg);
void decimal32_strip(decimal32_t *arg);

#ifdef __cplusplus
};
#endif  /*__cplusplus*/

#endif /*DECIMAL_H*/

