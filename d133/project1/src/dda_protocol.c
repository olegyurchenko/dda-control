/*----------------------------------------------------------------------------*/
/**
* @pkg dda_protocol
*/
/**
* Serial protocol with PC.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 03.01.2013 10:31:16<br>
* @pkgdoc dda_protocol
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_protocol.h"
#include <sys_timer.h>
#include <loop_buff.h>
#include <dda_uart.h>
#include <dda_mode.h>
#include <dda_work_mode.h>
#include <dda_db.h>
#include <dda_clib.h>
/*----------------------------------------------------------------------------*/
#define ENQ 5
#define ACK 6
#define NAK 0x15
#define RX_TIMEOUT 1000
#define ACK_TIMEOUT 1000
#define RETRY_COUNT 25
/*----------------------------------------------------------------------------*/
static LOOP_BUFFER rx_buffer;
static uint8_t protocol_buffer[32];
/*----------------------------------------------------------------------------*/
static LOOP_BUFFER queue;
static uint8_t queue_buffer[1024];
/*----------------------------------------------------------------------------*/
typedef enum
{
  TransportIdle,
  RxPacket,
  TxPacket,
  WaitAck
} transport_state_t;
static transport_state_t transport_state = TransportIdle;

typedef enum
{
  SendSerial,
  SendCalibration,
  SendMeasureData,
  SendNextCassete,
  SendEndOfTest
} protocol_state_t;
static protocol_state_t protocol_state = SendSerial;

static timeout_t timeout;
static int established = 0;
static void send_data(int retry);
static void receive_data();
static void wait_data();
static void send_packet(char header, const char *data /*=0*/, unsigned size /*=0*/);
static void send_measure_data();
static void set_established();
static void clr_established();
static void execute_cmd();
/*----------------------------------------------------------------------------*/
void protocol_init()
{
  lb_init(&rx_buffer, protocol_buffer, sizeof(protocol_buffer));
  lb_init(&queue, queue_buffer, sizeof(queue_buffer));
  transport_state = TransportIdle;
  protocol_state = SendSerial;
}
/*----------------------------------------------------------------------------*/
void protocol_handler()
{
  switch(transport_state)
  {
  default:
  case TransportIdle:
    transport_state = TxPacket;
    send_data(0);
    break;
  case RxPacket:
    receive_data();
    break;
  case TxPacket:
    if(!uart_tx_size())
    {
      transport_state = WaitAck;
      timeout_set(&timeout, ACK_TIMEOUT, sys_tick_count());
      lb_clear(&rx_buffer);
    }
    break;
  case WaitAck:
    wait_data();
    break;
  }
}
/*----------------------------------------------------------------------------*/
int is_measure_data_transmitted()
{
  return !lb_size(&queue);
}
/*----------------------------------------------------------------------------*/
static void send_data(int retry)
{
  if(!retry)
  {
    switch(work_state())
    {
    case Idle:
    default:
      protocol_state = SendSerial;
      break;
    case Calibration:
      protocol_state = SendCalibration;
      break;
    case Measuring:
      protocol_state = SendMeasureData;
      break;
    case NexCasseteWait:
      protocol_state = SendNextCassete;
      break;
    case Done:
      protocol_state = SendEndOfTest;
      break;
    }

    if(lb_size(&queue))
      protocol_state = SendMeasureData;
  }

  switch(protocol_state)
  {
  case SendSerial:
    send_packet('N', device_serial_str(), 0);
    break;
  case SendCalibration:
    send_packet('A', "000000", 6);
    break;
  case SendMeasureData:
    send_measure_data();
    break;
  case SendNextCassete:
    send_packet('C', 0, 0);
    break;
  case SendEndOfTest:
    send_packet('E', 0, 0);
    break;
  }
}
/*----------------------------------------------------------------------------*/
static void receive_data()
{
  uint8_t b;
  if(uart_read(&b, 1))
  {
    if(lb_size(&rx_buffer) < 3)
    {
      lb_push(&rx_buffer, b);
      if(lb_size(&rx_buffer) == 3) //Size received
      {
        if(b < 4) //Invalid size
        {
          clr_established();
          return;
        }
      }
    }
    else
    {
      uint8_t size, i;
      size = *lb_at(&rx_buffer, 2);
      if(lb_size(&rx_buffer) == size) //CRC received
      {
        unsigned char crc = 0;
        for(i = 1; i < size; i++)
          crc += *lb_at(&rx_buffer, i);

        if(crc == b)
        {
          execute_cmd();
          set_established();
        }
        else
          clr_established();

      }
      lb_push(&rx_buffer, b);
    }
  }
}
/*----------------------------------------------------------------------------*/
static void wait_data()
{
  uint8_t b;
  if(uart_read(&b, 1))
  {
    if(b == ACK)
    {
      set_established();
      return;
    }
    else
    if(b == ENQ)
    {
      lb_push(&rx_buffer, b);
      set_established();
      transport_state = RxPacket;
      return;
    }
    else
    if(b == NAK)
    {
      clr_established();
      return;
    }
  }

  if(timeout_riched(&timeout, sys_tick_count()))
    clr_established();
}
/*----------------------------------------------------------------------------*/
static void send_measure_data()
{
  if(!lb_size(&queue))
    send_packet('N', device_serial_str(), 0);
  else
  {
    uint8_t sz, i;
    uint16_t index = 1;
    char header;
    char buffer[3];
    sz = *lb_at(&queue, 0) - 1;
    header = *lb_at(&queue, index ++);
    for(i = 0; i < sz && i < sizeof(buffer); i++)
      buffer[i] = *lb_at(&queue, index ++);

    send_packet(header, buffer, sz);
  }
}
/*----------------------------------------------------------------------------*/
static void send_packet(char header, const char *data, unsigned size)
{
  unsigned char crc;
  unsigned i;
  if(data != 0 && !size)
    size = strlen(data);

  uart_write(&header, 1);
  crc = header;

  header = size + 2;
  uart_write(&header, 1);
  crc += header;
  if(data != 0 && size)
  {
    uart_write(data, size);
    for(i = 0; i < size; i++)
      crc += data[i];
  }
  uart_write(&crc, 1);
}
/*----------------------------------------------------------------------------*/
void protocol_push_grain_size(const decimal32_t *size)
{
  char buffer[16];
  decimal32_t tmp;
  uint8_t sz;

  decimal32_math_round(size, 0, &tmp);
  sz = snprintf(buffer, sizeof(buffer), "%4d.", tmp.data);
  if(lb_free(&queue) >= (unsigned)sz + 2)
  {
    lb_push(&queue, sz + 1);
    lb_push(&queue, 'G');
    lb_push_buffer(&queue, buffer, sz);
  }
}
/*----------------------------------------------------------------------------*/
void protocol_push_current_force(const decimal32_t *force)
{
  char buffer1[16], buffer2[16];
  decimal32_t tmp;
  uint8_t size;

  decimal32_math_round(force, 1, &tmp);
  decimal32_str(&tmp, buffer2, sizeof(buffer2));
  size = snprintf(buffer1, sizeof(buffer1), "%6s", buffer2);
  if(lb_free(&queue) >= (unsigned)size + 2 + 32 /*reserve for strength*/)
  {
    lb_push(&queue, size + 1);
    lb_push(&queue, 'A');
    lb_push_buffer(&queue, buffer1, size);
  }
}
/*----------------------------------------------------------------------------*/
void protocol_push_strength(int index, int cell, const decimal32_t *strength)
{
  char buffer1[16], buffer2[16];
  decimal32_t tmp;
  uint8_t size;

  decimal32_math_round(strength, 2, &tmp);
  decimal32_str(&tmp, buffer2, sizeof(buffer2));
  size = snprintf(buffer1, sizeof(buffer1), "%6s,%2d,%2d", buffer2, index, cell);
  if(lb_free(&queue) >= (unsigned)size + 2)
  {
    lb_push(&queue, size + 1);
    lb_push(&queue, 'M');
    lb_push_buffer(&queue, buffer1, size);
  }
}
/*----------------------------------------------------------------------------*/
static void execute_cmd()
{
}
/*----------------------------------------------------------------------------*/
static void set_established()
{
  if(protocol_state == SendMeasureData && lb_size(&queue))
  {
    uint8_t sz;
    lb_pop(&queue, &sz);
    lb_pop_buffer(&queue, 0, sz);
  }

  transport_state = TransportIdle;
  established = RETRY_COUNT;
}
/*----------------------------------------------------------------------------*/
static void clr_established()
{
  transport_state = TxPacket;
  if(established)
    established --;

  if(established)
    send_data(1); //Retry
  else
    transport_state = TransportIdle;
}
/*----------------------------------------------------------------------------*/

