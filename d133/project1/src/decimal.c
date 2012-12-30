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
/*----------------------------------------------------------------------------*/
#include "decimal.h"


#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)              (((a) < 0) ? (- (a)) : (a))
#endif

#define TRACE(f) fprintf(stderr, "%s:%d: data:%d decimals:%d\n", __FILE__, __LINE__, (int)((f)->data), (int)((f)->decimals))
/*
Default:
RETURN values - 0 if Ok, else != 0
*/
/*----------------------------------------------------------------------------*/
static int fast_division = 0;
void set_fast_division(int f)
{
  fast_division = f;
}
/*----------------------------------------------------------------------------*/
//(src * 205) >> 11;  // 205/2048 is nearly the same as /10
#define div10(src) ((sizeof(src) > sizeof(int) && fast_division) ? (((src) * 205) >> 11) : ((src)/10))
#define mul10(src) (((src) << 3) + ((src) << 1))
#define mod10(src) ((src) - mul10(div10(src)))
/*----------------------------------------------------------------------------*/
decimal64_t decimal64_init(int64_t data, decimals_t decimals)
{
  decimal64_t result;
  result.data = data;
  result.decimals = decimals;
  return result;
}
/*----------------------------------------------------------------------------*/
int64_t mulhs64(int64_t u, int64_t v)
{
  uint64_t u0, v0, w0;
  int64_t u1, v1, w1, w2, t;
  u0 = u & 0xffffffff;  u1 = u >> 32;
  v0 = v & 0xffffffff;  v1 = v >> 32;
  w0 = u0 * v0;
  t  = u1 * v0 + (w0 >> 32);
  w1 = t & 0xffffffff;
  w2 = t >> 32;
  w1 = u0 * v1 + w1;
  return u1 * v1 + w2 + (w1 >> 32);
}
/*----------------------------------------------------------------------------*/
int adjust_decimals64(/*IN/OUT*/ decimal64_t *arg1, /*IN*/ decimals_t decimals)
{
#ifndef __BORLANDC__
  #define ONE_64 1ll
#else
  #define ONE_64 1i64
#endif
  int64_t mul = ONE_64, tmp;

  if(arg1->decimals >= decimals)
    return 0;

  if(arg1->data && (decimals > MAX_CURRENCY_DECIMALS || arg1->decimals > MAX_CURRENCY_DECIMALS))
    return CUR_OVERFLOAT;

  tmp = ABS(arg1->data);
  for(;arg1->decimals < decimals; arg1->decimals ++)
  {
    /* *= 10*/
    mul = mul10(mul);
    arg1->data = mul10(arg1->data);
  }

  return mulhs64(tmp, mul) == 0 ? 0 : CUR_OVERFLOAT;
}
/*----------------------------------------------------------------------------*/
void int2decimal64(/*IN*/long src, /*OUT*/  decimal64_t *dst)
{
  /*Dummy function (for compability)*/
  dst->data = src;
  dst->decimals = 0;
}
/*----------------------------------------------------------------------------*/
int decimal64_cmp(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2)
{
  decimal64_t tmp1, tmp2;
  decimals_t max_decimals;
  tmp1 = *arg1;
  tmp2 = *arg2;

  max_decimals = MAX(tmp1.decimals, tmp2.decimals);
  if(adjust_decimals64(&tmp1, max_decimals)) return CUR_OVERFLOAT;
  if(adjust_decimals64(&tmp2, max_decimals)) return CUR_OVERFLOAT;

  if(tmp1.data > tmp2.data)
    return 1;
  if(tmp1.data < tmp2.data)
    return -1;

  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal64_add(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2, /*OUT*/  decimal64_t *dst)
{
  decimal64_t tmp1, tmp2;
  decimals_t max_decimals;
  int sig1, sig2;

  tmp1 = *arg1;
  tmp2 = *arg2;
  max_decimals = MAX(tmp1.decimals, tmp2.decimals);
  if(adjust_decimals64(&tmp1, max_decimals)) return CUR_OVERFLOAT;
  if(adjust_decimals64(&tmp2, max_decimals)) return CUR_OVERFLOAT;
  sig1 = (tmp1.data < 0);
  sig2 = (tmp2.data < 0);

  dst->data = tmp1.data + tmp2.data;
  dst->decimals = max_decimals;

  if(sig1 == sig2) /*May be owerload, check...*/
    return sig1 != (dst->data < 0) ? CUR_OVERFLOAT : 0;

  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal64_sub(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2, /*OUT*/  decimal64_t *dst)
{
  decimal64_t tmp1, tmp2;
  decimals_t max_decimals;
  int sig1, sig2;

  tmp1 = *arg1;
  tmp2 = *arg2;
  max_decimals = MAX(tmp1.decimals, tmp2.decimals);
  if(adjust_decimals64(&tmp1, max_decimals)) return CUR_OVERFLOAT;
  if(adjust_decimals64(&tmp2, max_decimals)) return CUR_OVERFLOAT;
  sig1 = (tmp1.data < 0);
  sig2 = (tmp2.data >= 0);

  dst->data = tmp1.data - tmp2.data;
  dst->decimals = max_decimals;

  if(sig1 == sig2) /*May be owerload, check...*/
    return sig1 != (dst->data < 0) ? CUR_OVERFLOAT : 0;

  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal64_mul(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2, /*OUT*/  decimal64_t *dst)
{
  int sig;
  if(mulhs64(ABS(arg1->data), ABS(arg2->data)))
    return CUR_OVERFLOAT;

  if(arg1->data && arg2->data && arg1->decimals + arg2->decimals > MAX_CURRENCY_DECIMALS)
    return CUR_OVERFLOAT;

  sig = ((arg1->data < 0) != (arg2->data < 0)) && arg1->data != 0 && arg2->data != 0;
  dst->data = arg1->data * arg2->data;
  dst->decimals = arg1->decimals + arg2->decimals;

  return sig != (dst->data < 0) ? CUR_OVERFLOAT : 0;;
}
/*----------------------------------------------------------------------------*/
int decimal64_div(/*IN*/ const decimal64_t *arg1, /*IN*/  const decimal64_t *arg2, /*OUT*/  decimal64_t *dst)
{
#ifndef __BORLANDC__
  #define MASK1_64 0xffffffffffffffll
#else
  #define MASK1_64 0xffffffffffffffi64
#endif

  decimal64_t tmp1, tmp2;

  tmp1 = *arg1;
  tmp2 = *arg2;
  if(!arg2->data) return CUR_ZERODIVIDE;
  while(tmp1.data /*&& tmp2.decimals*/ && tmp1.decimals < MAX_CURRENCY_DECIMALS && ABS(tmp1.data) < (MAX_INT_64 & MASK1_64))
  {
    tmp1.data = mul10(tmp1.data);
    tmp1.decimals ++;
  }

  dst->decimals = MAX(tmp1.decimals, tmp2.decimals) - MIN(tmp1.decimals, tmp2.decimals);

//  if(adjust_decimals64(&tmp1, arg1->decimals + arg2->decimals)) return CUR_OVERFLOAT;
//  if(adjust_decimals64(&tmp2, dst->decimals)) return CUR_OVERFLOAT;
  dst->data = tmp1.data / tmp2.data;
  return 0;
}
/*----------------------------------------------------------------------------*/
/*ROUND*/
int decimal64_math_round(/*IN*/ const decimal64_t *arg, decimals_t decimals, /*OUT*/  decimal64_t *dst)
{
  char cy = 0;
  unsigned i;
  int64_t tmp;

  dst->data = arg->data;
  dst->decimals = arg->decimals;

  if(dst->decimals <= decimals)
   	return adjust_decimals64(dst, decimals);

  tmp = ABS(arg->data);
  for(i = 0; i < dst->decimals - decimals; i++)
  {
    char c = (char) mod10(tmp) + cy;
    tmp = div10(tmp);
    cy = c < 5 ? 0 : 1;
    dst->data = div10(dst->data);
  }

  dst->data += dst->data < 0 ? - cy : cy;
  dst->decimals = decimals;
  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal64_cash_round(/*IN*/ const decimal64_t *arg, decimals_t decimals, /*OUT*/  decimal64_t *dst)
{
  char cy = 0, c;
  dst->data = arg->data;
  dst->decimals = arg->decimals;

  if(dst->decimals <= decimals)
   	return adjust_decimals64(dst, decimals);

  decimal64_cut_round(dst, decimals + 1, dst);
  c = (char) mod10(ABS(dst->data)) + cy;
  dst->data  = div10(dst->data);
  cy = c < 5 ? 0 : 1;

  dst->data += dst->data < 0 ? - cy : cy;
  dst->decimals = decimals;
  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal64_cut_round(/*IN*/ const decimal64_t *arg, decimals_t decimals, /*OUT*/  decimal64_t *dst)
{
  dst->data = arg->data;
  dst->decimals = arg->decimals;
  if(dst->decimals <= decimals)
   	return adjust_decimals64(dst, decimals);

  for(; dst->decimals > decimals; dst->decimals --)
    dst->data = div10(dst->data);

  return 0;
}
/*----------------------------------------------------------------------------*/
/*ASCII CONVERSIONS*/
int decimal64_str(/*IN*/ const decimal64_t *arg, /*OUT*/ char *dst, /*IN*/unsigned buf_size)
{
  /*9223372036854775807*/
  char buffer[32];
  unsigned index = 0, i;
  int64_t tmp;

  if(arg->decimals >= buf_size || arg->decimals >= sizeof(buffer))
    return -1;

  tmp = ABS(arg->data);
  while(tmp && index < sizeof(buffer))
  {
    buffer[index ++] = (char) mod10(tmp);
    tmp = div10(tmp);
    if(index == arg->decimals)
      buffer[index ++] = '.' - '0';
  }

  if(index >= sizeof(buffer))
    return -1;

/*  if(!index) //0
    buffer[index ++] = 0;
  else */
  {
  	while(index < arg->decimals && index < sizeof(buffer))
      buffer[index ++] = 0;

    if(index == arg->decimals)
    {
    	/*0.00x*/
      if(arg->decimals)
        buffer[index ++] = '.' - '0';
    	buffer[index ++] = 0;
    }
    else
    if(arg->decimals && index - 1 == arg->decimals)
    {
      /*.xx*/
    	buffer[index ++] = 0;
    }

  }


  if(arg->data < 0)
  {
  	if(buf_size)
  	{
  		*(dst ++) = '-';
  		buf_size --;
  	}
  }

  if((unsigned) index > buf_size)
    return -1;

  for(i = 0; i < index; i++)
  {
  	dst[i] = buffer[index - i - 1] + '0';
/*    printf("%d %02x\n", i, (int)buffer[index - i - 1] & 0xff);*/
  }

  dst[index] = 0;
  return 0;
}
/*----------------------------------------------------------------------------*/
int str_decimal64(/*IN*/const char *src, /*OUT*/ decimal64_t *dst)
{
  int sig = 0, index = 0, size = 0, dec_index = -1;
//  int64_t tmp = 1;

  dst->data = 0;
  dst->decimals = 0;


  if(*src && *src == '-')
  {
  	sig = 1;
    src ++;
  }

  /*strlen*/
  while(src[size]) size ++;

  for(index = 0; index < size; index ++)
  {
    char c = src[index];
    if(c == '.')
    {
      if(dec_index != -1)
        return -1;
      dec_index = index;
    }
    else
    {
    	if(c > '9' || c < '0')
      {
//        trace(__FILE__, __LINE__, "Not number char '%c'\r\n", c);
      	return -1;
      }


//      trace(__FILE__, __LINE__, "Before mul l:%d h:%d * %d\n", (int)dst->data, (int)(dst->data >> 32), (int) c);
      dst->data = mul10(dst->data);
      dst->data += (int64_t)(c & 0xf);
//      trace(__FILE__, __LINE__, "After mul l:%d h:%d * %d\n", (int)dst->data, (int)(dst->data >> 32), (int) c);
    }
  }

  if(dst->data < 0)
    return CUR_OVERFLOAT;

  if(sig)
    dst->data = - dst->data;

  if(dec_index != -1)
    dst->decimals = size - dec_index - 1;

/*  printf("%d\n",dst->decimals);*/
/*
  {
    char buf[24];
    cur2asc(dst, buf, sizeof(buf));
    trace(__FILE__, __LINE__, "%s\n", buf);
  }
*/
  return 0;
}
/*----------------------------------------------------------------------------*/
void decimal64_abs(/*IN*/ const decimal64_t *src, /*OUT*/  decimal64_t *dst)
{
  dst->data = ABS(src->data);
  dst->decimals = src->decimals;
}
/*----------------------------------------------------------------------------*/
/* 32 -bit routines */
/*----------------------------------------------------------------------------*/
decimal32_t decimal32_init(int32_t data, decimals_t decimals)
{
  decimal32_t dst;
  dst.data = data;
  dst.decimals = decimals;
  return dst;
}
/*----------------------------------------------------------------------------*/
int decimal32_64( const decimal32_t *src,  decimal64_t *dst)
{
  dst->data = (int64_t) src->data;
  dst->decimals = src->decimals;
  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal64_32( const decimal64_t *src,  decimal32_t *dst)
{
#ifndef __BORLANDC__
  #define MASK2_64 0xffffffff80000000ll
#else
  #define MASK2_64 0xffffffff80000000i64
#endif

  decimal64_t tmp;
  tmp = *src;
  tmp.data = ABS(tmp.data);
  while(((tmp.data & (int64_t)MASK2_64) || tmp.decimals >= MAX_FLOAT_DECIMALS) && tmp.decimals )
  {
    tmp.data = div10(tmp.data);
    tmp.decimals --;
  }

  if(tmp.data >> 32)
    return CUR_OVERFLOAT;
  dst->data = (int32_t) tmp.data;
  if(src->data < 0)
    dst->data = -1 * dst->data;
  dst->decimals = tmp.decimals;
  return 0;
}
/*----------------------------------------------------------------------------*/
int32_t mulhs32(int32_t u, int32_t v)
{
  uint32_t u0, v0, w0;
  int32_t u1, v1, w1, w2, t;
  u0 = u & 0xffff;  u1 = u >> 16;
  v0 = v & 0xffff;  v1 = v >> 16;
  w0 = u0 * v0;
  t  = u1 * v0 + (w0 >> 16);
  w1 = t & 0xffff;
  w2 = t >> 16;
  w1 = u0 * v1 + w1;
  return u1 * v1 + w2 + (w1 >> 16);
}
/*----------------------------------------------------------------------------*/
int adjust_decimals32(/*IN/OUT*/ decimal32_t *arg1, /*IN*/ decimals_t decimals)
{
  int32_t mul = 1, tmp;

  if(arg1->decimals >= decimals)
    return 0;

  if(arg1->data && (decimals > MAX_FLOAT_DECIMALS || arg1->decimals > MAX_FLOAT_DECIMALS))
    return CUR_OVERFLOAT;

  tmp = ABS(arg1->data);
  for(;arg1->decimals < decimals; arg1->decimals ++)
  {
    /* *= 10*/
    mul = mul10(mul);
    arg1->data = (arg1->data << 3) + (arg1->data << 1);
  }

  return mulhs32(tmp, mul) == 0 ? 0 : CUR_OVERFLOAT;
}
/*----------------------------------------------------------------------------*/
int decimal32_cmp(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2)
{
   decimal32_t tmp1, tmp2;
  decimals_t max_decimals;
  tmp1 = *arg1;
  tmp2 = *arg2;

  max_decimals = MAX(tmp1.decimals, tmp2.decimals);
  if(adjust_decimals32(&tmp1, max_decimals)) return CUR_OVERFLOAT;
  if(adjust_decimals32(&tmp2, max_decimals)) return CUR_OVERFLOAT;

  if(tmp1.data > tmp2.data)
    return 1;
  if(tmp1.data < tmp2.data)
    return -1;

  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal32_add(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst)
{
   decimal32_t tmp1, tmp2;
  decimals_t max_decimals;
  int sig1, sig2;

  tmp1 = *arg1;
  tmp2 = *arg2;
  max_decimals = MAX(tmp1.decimals, tmp2.decimals);
  if(adjust_decimals32(&tmp1, max_decimals)) return CUR_OVERFLOAT;
  if(adjust_decimals32(&tmp2, max_decimals)) return CUR_OVERFLOAT;
  sig1 = (tmp1.data < 0);
  sig2 = (tmp2.data < 0);

  dst->data = tmp1.data + tmp2.data;
  dst->decimals = max_decimals;

  if(sig1 == sig2) /*May be owerload, check...*/
    return sig1 != (dst->data < 0) ? CUR_OVERFLOAT : 0;

  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal32_sub(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst)
{
   decimal32_t tmp1, tmp2;
  decimals_t max_decimals;
  int sig1, sig2;

  tmp1 = *arg1;
  tmp2 = *arg2;
  max_decimals = MAX(tmp1.decimals, tmp2.decimals);
  if(adjust_decimals32(&tmp1, max_decimals)) return CUR_OVERFLOAT;
  if(adjust_decimals32(&tmp2, max_decimals)) return CUR_OVERFLOAT;
  sig1 = (tmp1.data < 0);
  sig2 = (tmp2.data >= 0);

  dst->data = tmp1.data - tmp2.data;
  dst->decimals = max_decimals;

  if(sig1 == sig2) /*May be owerload, check...*/
    return sig1 != (dst->data < 0) ? CUR_OVERFLOAT : 0;

  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal32_mul(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst)
{
  int sig;
  if(mulhs32(ABS(arg1->data), ABS(arg2->data)))
    return CUR_OVERFLOAT;

  if(arg1->data && arg2->data && arg1->decimals + arg2->decimals > MAX_FLOAT_DECIMALS)
    return CUR_OVERFLOAT;

  sig = ((arg1->data < 0) != (arg2->data < 0)) && arg1->data != 0 && arg2->data != 0;
  dst->data = arg1->data * arg2->data;
  dst->decimals = arg1->decimals + arg2->decimals;

  return sig != (dst->data < 0) ? CUR_OVERFLOAT : 0;;
}
/*----------------------------------------------------------------------------*/
int decimal32_div(/*IN*/ const decimal32_t *arg1, /*IN*/  const decimal32_t *arg2, /*OUT*/  decimal32_t *dst)
{
  decimal32_t tmp1, tmp2;

  tmp1 = *arg1;
  tmp2 = *arg2;
//  TRACE(&tmp1);
//  TRACE(&tmp2);
  if(!arg2->data) return CUR_ZERODIVIDE;
  while(tmp1.data && /*tmp2.decimals &&*/ tmp1.decimals < MAX_FLOAT_DECIMALS && ABS(tmp1.data) < (MAX_INT_32 & 0xffffff))
  {
    tmp1.data = mul10(tmp1.data);
    tmp1.decimals ++;
  }

  //TRACE(&tmp1);
  //TRACE(&tmp2);
  dst->decimals = MAX(tmp1.decimals, tmp2.decimals) - MIN(tmp1.decimals, tmp2.decimals);
//  if(adjust_decimals32(&tmp1, dst->decimals)) return CUR_OVERFLOAT;
//  if(adjust_decimals32(&tmp2, dst->decimals)) return CUR_OVERFLOAT;
  dst->data = tmp1.data / tmp2.data;
  //TRACE(dst);
  return 0;
}
/*----------------------------------------------------------------------------*/
/*ROUND*/
int decimal32_math_round(/*IN*/ const decimal32_t *arg, decimals_t decimals, /*OUT*/  decimal32_t *dst)
{
  char cy = 0;
  unsigned i;
  int32_t tmp;

  dst->data = arg->data;
  dst->decimals = arg->decimals;

  if(dst->decimals <= decimals)
   	return adjust_decimals32(dst, decimals);

  tmp = ABS(arg->data);
  for(i = 0; i < dst->decimals - decimals; i++)
  {
    char c = (char) mod10(tmp) + cy;
    tmp = div10(tmp);
    cy = c < 5 ? 0 : 1;
    dst->data /= 10;
  }

  dst->data += dst->data < 0 ? - cy : cy;
  dst->decimals = decimals;
  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal32_cash_round(/*IN*/ const decimal32_t *arg, decimals_t decimals, /*OUT*/  decimal32_t *dst)
{
  char cy = 0, c;
  dst->data = arg->data;
  dst->decimals = arg->decimals;

  if(dst->decimals <= decimals)
   	return adjust_decimals32(dst, decimals);

  decimal32_cut_round(dst, decimals + 1, dst);
  c = (char) ABS(mod10(dst->data)) + cy;
  dst->data = mul10(dst->data);
  cy = c < 5 ? 0 : 1;

  dst->data += dst->data < 0 ? - cy : cy;
  dst->decimals = decimals;
  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal32_cut_round(/*IN*/ const decimal32_t *arg, decimals_t decimals, /*OUT*/  decimal32_t *dst)
{
  dst->data = arg->data;
  dst->decimals = arg->decimals;
  if(dst->decimals <= decimals)
   	return adjust_decimals32(dst, decimals);

  for(; dst->decimals > decimals; dst->decimals --)
    dst->data = div10(dst->data);

  return 0;
}
/*----------------------------------------------------------------------------*/
int decimal32_str(/*IN*/ const decimal32_t *arg, /*OUT*/ char *dst, /*IN*/unsigned buf_size)
{
  char buffer[24];
  unsigned index = 0, i;
  int32_t tmp;

  if(arg->decimals >= buf_size || arg->decimals >= sizeof(buffer))
    return -1;

  tmp = ABS(arg->data);
  while(tmp && index < sizeof(buffer))
  {
    buffer[index ++] = (char) mod10(tmp);
    tmp = div10(tmp);
    if(index == arg->decimals)
      buffer[index ++] = '.' - '0';
  }

  if(index >= sizeof(buffer))
    return -1;

/*  if(!index) //0
    buffer[index ++] = 0;
  else */
  {
  	while(index < arg->decimals && index < sizeof(buffer))
      buffer[index ++] = 0;

    if(index >= sizeof(buffer))
      return -1;

    if(index == arg->decimals)
    {
    	/*0.00x*/
      if(arg->decimals)
        buffer[index ++] = '.' - '0';
    	buffer[index ++] = 0;
    }
    else
    if(arg->decimals && index - 1 == arg->decimals)
    {
      /*.xx*/
    	buffer[index ++] = 0;
    }

  }


  if(arg->data < 0)
  {
  	if(buf_size)
  	{
  		*(dst ++) = '-';
  		buf_size --;
  	}
  }

  if((unsigned) index > buf_size)
    return -1;

  for(i = 0; i < index; i++)
  {
  	dst[i] = buffer[index - i - 1] + '0';
/*    printf("%d %02x\n", i, (int)buffer[index - i - 1] & 0xff);*/
  }

  dst[index] = 0;
  return 0;
}
/*----------------------------------------------------------------------------*/
int str_decimal32(/*IN*/const char *src, /*OUT*/ decimal32_t *dst)
{
  int sig = 0, index = 0, size = 0, dec_index = -1;
//  int32_t tmp = 1;

  dst->data = 0;
  dst->decimals = 0;


  if(*src && *src == '-')
  {
  	sig = 1;
    src ++;
  }

  /*strlen*/
  while(src[size]) size ++;

  for(index = 0; index < size; index ++)
  {
    char c = src[index];
    if(c == '.')
    {
      if(dec_index != -1)
        return -1;
      dec_index = index;
    }
    else
    {
    	if(c > '9' || c < '0')
        return -1;

      dst->data = mul10(dst->data);
      dst->data += (int32_t)(c & 0xf);
    }
  }

  if(dst->data < 0)
    return CUR_OVERFLOAT;

  if(sig)
    dst->data = - dst->data;

  if(dec_index != -1)
    dst->decimals = size - dec_index - 1;

/*  printf("%d\n",dst->decimals);*/

  return 0;
}
/*----------------------------------------------------------------------------*/
void decimal32_abs(/*IN*/ const decimal32_t *src, /*OUT*/  decimal32_t *dst)
{
  dst->data = ABS(src->data);
  dst->decimals = src->decimals;
}
/*----------------------------------------------------------------------------*/
void decimal32_strip(decimal32_t *arg)
{
  while(arg->decimals && !mod10(arg->data))
  {
    arg->data = div10(arg->data);
    arg->decimals --;
  }
}
/*----------------------------------------------------------------------------*/
void decimal64_strip(decimal64_t *arg)
{
  while(arg->decimals && !mod10(arg->data))
  {
    arg->data = div10(arg->data);
    arg->decimals --;
  }
}
/*----------------------------------------------------------------------------*/


