/*----------------------------------------------------------------------------*/
/**
* @pkg loop_buff
*/
/**
* Loop buffer module.
*
* Loop buffer for use as FIFO for input/output.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 28.11.2012  9:41:01<br>
* @pkgdoc loop_buff
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "loop_buff.h"
/*----------------------------------------------------------------------------*/
void lb_init(LOOP_BUFFER *b, uint8_t *buffer, uint16_t buffer_size)
{
  b->buffer = buffer;
  b->size = buffer_size;
  b->tail = b->head = 0;
}
/*----------------------------------------------------------------------------*/
unsigned lb_size(LOOP_BUFFER *b)
{
  if(b->head < b->tail)
  {
   /*[**H--T**]*/
    return b->size - b->tail + b->head;
  }
  else
  if(b->head > b->tail)
  {
    /*[--T**H--]*/
    return  b->head - b->tail;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
int lb_push(LOOP_BUFFER *b, uint8_t c)
{
  if(lb_size(b) >= (unsigned)b->size - 1)
    return 0;
  b->buffer[b->head ++] = c;
  if(b->head >= b->size)
    b->head = 0;
  return 1;
}
/*----------------------------------------------------------------------------*/
int lb_pop(LOOP_BUFFER *b, uint8_t *c)
{
  if(b->head == b->tail)
    return 0;
  *c = b->buffer[b->tail ++];
  if(b->tail >= b->size)
    b->tail = 0;
  return 1;
}
/*----------------------------------------------------------------------------*/
