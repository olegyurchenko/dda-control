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
#ifndef LOOP_BUFF_H_1354088461
#define LOOP_BUFF_H_1354088461
/*----------------------------------------------------------------------------*/
#include <stdint.h>

typedef struct
{
  uint8_t *buffer;
  uint16_t size;
  uint16_t head;
  uint16_t tail;
} LOOP_BUFFER;
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**Loop buffer init*/
void lb_init(LOOP_BUFFER *b, uint8_t *buffer, uint16_t buffer_size);
/**Return data size in buffer*/
unsigned lb_size(LOOP_BUFFER *b);
/**Return free size in buffer*/
unsigned lb_free(LOOP_BUFFER *b);
/**Get data pointer at index*/
uint8_t *lb_at(LOOP_BUFFER *b, uint16_t index);
/**Put byte to buffer. Return 0 if buffer full*/
int lb_push(LOOP_BUFFER *b, uint8_t c);
/**Get byte from buffer. Return 0 if buffer empty*/
int lb_pop(LOOP_BUFFER *b, uint8_t *c);
/**Clear buffer*/
void lb_clear(LOOP_BUFFER *b);


#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*LOOP_BUFF_H_1354088461*/

