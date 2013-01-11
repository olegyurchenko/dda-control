/*----------------------------------------------------------------------------*/
/**
* @pkg dda_uart
*/
/**
* DDA UART module.
*
* Handle DDA SUART1 for communications.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 28.11.2012 10:23:10<br>
* @pkgdoc dda_uart
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_uart.h"
#include <loop_buff.h>
#include "stm32f10x.h"
/*----------------------------------------------------------------------------*/
typedef struct
{
  LOOP_BUFFER rx;
  LOOP_BUFFER tx;
} UART_BUFFER;
/*----------------------------------------------------------------------------*/
#define BAUD 9600
#define RX_BUFFER_SIZE 128
#define TX_BUFFER_SIZE 512
/*----------------------------------------------------------------------------*/
static uint8_t tx[TX_BUFFER_SIZE], rx[RX_BUFFER_SIZE];
static UART_BUFFER uart_buffer;
/*----------------------------------------------------------------------------*/
static void lock_buffer()
{
}
/*----------------------------------------------------------------------------*/
static void unlock_buffer()
{
}
/*----------------------------------------------------------------------------*/
void uart_init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  lb_init(&uart_buffer.rx, rx, RX_BUFFER_SIZE);
  lb_init(&uart_buffer.tx, tx, TX_BUFFER_SIZE);


  /* Enable GPIOA, USART1 clock                                           */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);

  /* Configure USART1 Rx (PA10) as input floating                         */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART1 Tx (PA9) as alternate function push-pull            */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate            = BAUD;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);

  /* Enabling interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15; // приоритет
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // приоритет субгруппы
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // включаем канал
  NVIC_Init(&NVIC_InitStructure); // инициализируем

  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  // включаем прерывание на передачу
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // включаем прерывание на прием
}
/*----------------------------------------------------------------------------*/
int uart_write(const void *data, uint32_t size)
{
  const uint8_t *buffer;
  int res = 0;

  buffer = (const uint8_t *)data;
  while(size && lb_push(&uart_buffer.tx, *buffer))
  {
    buffer ++;
    size --;
    res ++;
  }

  /* Enable USARTz Receive and Transmit interrupts */
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

  return res;
}
/*----------------------------------------------------------------------------*/
int uart_read(void *data, uint32_t size)
{
  uint8_t *buffer;
  int res = 0;

  buffer = (uint8_t *)data;
  while(size && lb_pop(&uart_buffer.rx, buffer))
  {
    buffer ++;
    size --;
    res ++;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void uart_rx_clear()
{
  lb_init(&uart_buffer.rx, rx, RX_BUFFER_SIZE);
}
/*----------------------------------------------------------------------------*/
void uart_tx_clear()
{
  lb_init(&uart_buffer.tx, tx, TX_BUFFER_SIZE);
}
/*----------------------------------------------------------------------------*/
int uart_rx_size()
{
  return lb_size(&uart_buffer.rx);
}
/*----------------------------------------------------------------------------*/
int uart_tx_size()
{
  return lb_size(&uart_buffer.tx);
}
/*----------------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_RXNE); //?????????
    /* Read one byte from the receive data register */
    lb_push(&uart_buffer.rx, USART_ReceiveData(USART1));
  }

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    uint8_t c;
    USART_ClearITPendingBit(USART1, USART_IT_TXE); //??????????
 /* Write one byte to the transmit data register */
    /* Write one byte to the transmit data register */

    if(lb_pop(&uart_buffer.tx, &c))
      USART_SendData(USART1, c);
    else
    {
      /* Disable the USART1 Transmit interrupt */
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
  }

}
/*----------------------------------------------------------------------------*/

