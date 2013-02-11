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
#include <dda_mesh.h>
#include <dda_config.h>
#include <dda_settings.h>
/*----------------------------------------------------------------------------*/
#ifndef NO_PROTOCOL
  #ifdef USE_CONSOLE
    #define NO_PROTOCOL
  #endif //USE_CONSOLE
#endif //NO_PROTOCOL
/*----------------------------------------------------------------------------*/
#define ENQ 5
#define ACK 6
#define NAK 0x15
#define RX_TIMEOUT 1000
#define ACK_TIMEOUT 1000
#define RETRY_COUNT 25
#define CURRENT_FORCE_PERIOD 20
/*----------------------------------------------------------------------------*/
static LOOP_BUFFER rx_buffer;
static uint8_t protocol_buffer[128];
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
  SendQueueData,
  SendNextCassete,
  SendEndOfTest
} protocol_state_t;
static protocol_state_t protocol_state = SendSerial;

static timeout_t timeout;
static timeout_t last_force_time = {0, 0};
static int established = 0;
static void send_data(int retry);
static void receive_data();
static void wait_data();
static void send_packet(char header, const char *data /*=0*/, unsigned size /*=0*/);
static void send_queue_data();
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
    lb_clear(&rx_buffer);
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
#ifdef NO_PROTOCOL
  return 1;
#else
  return !lb_size(&queue);
#endif
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
      //protocol_state = SendMeasureData;
      protocol_state = SendSerial;
      break;
    case NexCasseteWait:
      protocol_state = SendNextCassete;
      break;
    case Done:
      protocol_state = SendEndOfTest;
      break;
    }

    if(lb_size(&queue))
      protocol_state = SendQueueData;
  }

  switch(protocol_state)
  {
  case SendSerial:
    send_packet('N', device_serial_str(), 0); ///!!!!!
    //send_packet('N', "0000", 4); ///!!!!!
    break;
  case SendCalibration:
    send_packet('A', "000000", 6);
    break;
  case SendQueueData:
    send_queue_data();
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
        if(b < 3) //Invalid size
        {
          b = NAK;
          uart_write(&b, 1); //Send NAK
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
          b = ACK;
          uart_write(&b, 1); //Send ACK
          execute_cmd();
          set_established();
          lb_clear(&rx_buffer);
        }
        else
        {
          b = NAK;
          uart_write(&b, 1); //Send NAK
          clr_established();
          lb_clear(&rx_buffer);
        }
        return;
      }
      lb_push(&rx_buffer, b);
    }
  }

  if(timeout_riched(&timeout, sys_tick_count()))
  {
    b = NAK;
    uart_write(&b, 1); //Send NAK
    clr_established();
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
      timeout_set(&timeout, RX_TIMEOUT, sys_tick_count());
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
static void send_queue_data()
{
  if(!lb_size(&queue))
    send_packet('N', device_serial_str(), 0);
  else
  {
    uint8_t sz, i;
    uint16_t index = 1;
    char header;
    char buffer[32];
    sz = *lb_at(&queue, 0) - 1;
    header = *lb_at(&queue, index ++);
    for(i = 0; i < sz && i < sizeof(buffer); i++)
      buffer[i] = *lb_at(&queue, index ++);

    send_packet(header, sz ? buffer : 0, sz);
  }
}
/*----------------------------------------------------------------------------*/
static void send_packet(char header, const char *data, unsigned size)
{
  unsigned char crc, b;
  unsigned i;
  if(data != 0 && !size)
    size = strlen(data);

  b = ENQ;
  uart_write(&b, 1);

  uart_write(&header, 1);
  crc = header;

  header = size + 3;
  uart_write(&header, 1);
  crc += header;
  if(data != 0 && size)
  {
    uart_write(data, size);
    for(i = 0; i < size; i++)
      crc += data[i];
  }
  uart_write(&crc, 1);
  transport_state = TxPacket;
}
/*----------------------------------------------------------------------------*/
void protocol_push_grain_size(const decimal32_t *size)
{
  char buffer[16];
  decimal32_t tmp;
  uint8_t sz;

  if(!established)
    return;

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
void protocol_push_no_particle()
{
  char buffer[16];
  uint8_t sz;

  if(!established)
    return;

  sz = snprintf(buffer, sizeof(buffer), "-1    ");
  if(lb_free(&queue) >= (unsigned)sz + 2)
  {
    lb_push(&queue, sz + 1);
    lb_push(&queue, 'M');
    lb_push_buffer(&queue, buffer, sz);
  }
}
/*----------------------------------------------------------------------------*/
void protocol_push_current_force(const decimal32_t *force)
{
  char buffer1[16], buffer2[16];
  decimal32_t tmp;
  uint8_t size;

  if(!established)
    return;

  if(!timeout_riched(&last_force_time, sys_tick_count()))
    return;

  timeout_set(&last_force_time, CURRENT_FORCE_PERIOD, sys_tick_count());

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

  if(!established)
    return;

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
static void set_established()
{
  lb_clear(&rx_buffer);
  if(protocol_state == SendQueueData && lb_size(&queue))
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
  lb_clear(&rx_buffer);
  transport_state = TxPacket;
  if(established)
    established --;

  if(established)
    send_data(1); //Retry
  else
  {
    transport_state = TransportIdle;
    lb_clear(&queue);
  }
}
/*----------------------------------------------------------------------------*/
static void execute_cmd()
{
  uint8_t b, sz;

  lb_pop(&rx_buffer, &b); //ENQ
  lb_pop(&rx_buffer, &b); //CMD
  lb_pop(&rx_buffer, &sz); //SIZE

  switch(b)
  {
  case 'P':
    start_work();
    break;
  case 'S':
    set_work_mode(ManualMode);
    break;
  case 'L':
    set_work_mode(AutoMode);
    break;
  case 'I':
  {
    unsigned _mesh_index, samples;
    work_mode_t mode;

    lb_pop(&rx_buffer, &b);
    _mesh_index = b;
    lb_pop(&rx_buffer, &b);
    _mesh_index |= (unsigned)b << 8;
    lb_pop(&rx_buffer, &b);
    samples = b;
    lb_pop(&rx_buffer, &b);
    mode = (b & 2) ? AutoMode : ManualMode;
    set_mesh_index(_mesh_index);
    set_samples(samples);
    set_work_mode(mode);
    break;
  }

  case 'R': //Read parameter request
  {
    char name[16], buffer[32];
    const char *value;
    int i, count, index = 0;

    sz -= 3;
    while(sz && index < (int) sizeof(name) - 1)
    {
      lb_pop(&rx_buffer, &b);
      name[index ++] = b;
      sz --;
    }
    name[index] = '\0';
    if(index)
    {
      value = setting_get(name);
      index = snprintf(buffer, sizeof(buffer), "%s=%s", name, value);
      lb_push(&queue, index + 1);
      lb_push(&queue, 'R');
      lb_push_buffer(&queue, buffer, index);
    }
    else
    {
      count = setting_count();
      for(i = 0; i < count; i++)
      {
        const DDA_SETTING *s;
        s = setting(i);
        index = snprintf(buffer, sizeof(buffer), "%s=%s", s->name, s->value);
        lb_push(&queue, index + 1);
        lb_push(&queue, 'R');
        lb_push_buffer(&queue, buffer, index);
      }
    }
    lb_push(&queue, 1); //Empty R packet after all parameters
    lb_push(&queue, 'R');
    break;
  }

  case 'W': //Write parameter request
  {
    char buffer[32];
    char *value = 0;
    int op = -1;
    int index = 0;

    sz -= 3;
    if(sz) //Operation
    {
      lb_pop(&rx_buffer, &b);
      op = b;
      sz --;
    }

    while(sz && index < (int) sizeof(buffer) - 1)
    {
      lb_pop(&rx_buffer, &b);
      buffer[index] = b;
      if(b == '=')
      {
        buffer[index] = '\0';
        value = &buffer[index + 1];
      }
      index ++;
      sz --;
    }
    buffer[index] = '\0';
    if(op == 1) //Erase table
      settings_clear();
    if(index && value)
      setting_set(buffer, value);
    if(op == 2 && setting_modified()) //Write table
      settings_write();
    break;
  }
  default:
    break;
  }
}
/*----------------------------------------------------------------------------*/

