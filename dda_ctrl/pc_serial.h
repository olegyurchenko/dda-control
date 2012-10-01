/*----------------------------------------------------------------------------*/
/*"$Id$"*/
/**
* @pkg pc_serial
*/
/**
* Cross platform (windows && linux) serial port API (for PC).
*
* (C) T&T, Kiev, Ukraine 2004.
* start 12.11.04 10:01:16
* @pkgdoc pc_serial
* @author Oleg Yurchenko
* @version $Revision$
*/
/*----------------------------------------------------------------------------*/
#ifndef PC_SERIAL_H
#define PC_SERIAL_H

#define SER_DSR 1//(data set ready/line enable)
#define SER_DTR	2//DTR (data terminal ready)
#define SER_RTS	4//RTS (request to send)
#define SER_CTS	8//CTS (clear to send)
#define SER_DCD 16//(data carrier detect)
#define SER_RNG	32//RNG (ring)

#ifdef __cplusplus
extern "C"
{
#endif  /*__cplusplus*/
  /**Open serial device ('/dev/ttyS0', '/dev/ttyUSB0' and so on */
  int serial_open_dev(const char *device, int bits, int baud, char parity, int stopbits);
  /**Open serial device by index 1 - COM1, 2 - COM2 ...*/
  int serial_open(int ncom /*1,2...*/, int bits, int baud, char parity, int stopbits);
  /**Close serial*/
  void serial_close(int fd);
  /**Flush*/
  void serial_flush(int fd);
  /**Read serial*/
  int serial_read(int fd, void *buffer, unsigned size, unsigned timeout);
  /**Write serial*/
  int serial_write(int fd, void *buffer, unsigned size);
  /**Wait for read*/
  unsigned serial_waitrx(int fd, unsigned timeout);
  /**Get queue size*/
  unsigned serial_rxlen(int fd);
  /**Flush transmitter*/
  void serial_flush_tx(int fd);
  /**Flush receiver*/
  void serial_flush_rx(int fd);
  /**Get modem controll lines satus*/
  int serial_get_ctrl(int fd, int *_signals);
  /**Set modem controll lines satus*/
  int serial_set_ctrl(int fd, int _signals);

#ifdef __cplusplus
};
#endif  /*__cplusplus*/

#endif /*PC_SERIAL_H*/

