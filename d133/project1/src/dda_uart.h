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
#ifndef DDA_UART_H_1354090990
#define DDA_UART_H_1354090990
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void uart_init();
int uart_write(const void *data, uint32_t size);
int uart_read(void *buffer, uint32_t buffer_size);
void uart_rx_clear();
void uart_tx_clear();
int uart_rx_size();


#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_UART_H_1354090990*/

