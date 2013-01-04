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
static uint8_t protocol_buffer[128];
/*----------------------------------------------------------------------------*/
static LOOP_BUFFER queue;
static uint8_t queue_buffer[128];
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
static int current_force = -1;
static int current_cell = -1;
static void send_data(int retry);
static void receive_data();
static void wait_data();
static void send_packet(char header, const char *data /*=0*/, unsigned size /*=0*/);
static void send_measure_data();
/*----------------------------------------------------------------------------*/
void protocol_init()
{
  lb_init(&rx_buffer, protocol_buffer, sizeof(protocol_buffer));
  lb_init(&queue, queue_buffer, sizeof(queue_buffer));
  transport_state = TransportIdle;
  protocol_state = SendSerial;
  current_force = -1;
  current_cell = -1;
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
}
/*----------------------------------------------------------------------------*/
static void wait_data()
{
  uint8_t b;
  if(uart_read(&b, 1))
  {
    if(b == ACK)
    {
      transport_state = TransportIdle;
      established = RETRY_COUNT;
      return;
    }
    else
    if(b == ENQ)
    {
      lb_push(&rx_buffer, b);
      transport_state = RxPacket;
      established = RETRY_COUNT;
      return;
    }
    else
    if(b == NAK)
    {
      transport_state = TxPacket;
      if(established)
        established --;
      send_data(established);
      return;
    }
  }

  if(timeout_riched(&timeout, sys_tick_count()))
  {
    transport_state = TxPacket;
    if(established)
      established --;
    send_data(established);
  }
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
    char buffer[16];
    sz = *lb_at(&queue, 0) - 1;
    header = *lb_at(&queue, index ++);
    for(i = 0; i < sz; i++)
      buffer[i] = *lb_at(&queue, index ++);


  }
}
/*----------------------------------------------------------------------------*/
static void send_packet(char header, const char *data, unsigned size)
{
}
/*----------------------------------------------------------------------------*/

