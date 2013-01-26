/*----------------------------------------------------------------------------*/
/**
* @pkg dda_protocol
*/
/**
* DDA serial protocol.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 26.01.2013 15:42:02<br>
* @pkgdoc dda_protocol
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_PROTOCOL_H_1359207722
#define DDA_PROTOCOL_H_1359207722
#include <vector>
#include <logfile.h>
/*----------------------------------------------------------------------------*/
class DDAProtocol
{
public:
  typedef std::vector<unsigned char> ByteVector;
protected:
  typedef std::vector<ByteVector> BinList;
  int m_serial;
  bool m_terminated;
  BinList m_txData;
  ByteVector m_rxData;
  unsigned m_lastCharTime;
  logfile *m_log;
  int serial_read(int fd, void *buffer, unsigned size, unsigned timeout);
  int serial_write(int fd, void *buffer, unsigned size);
  bool rxPacket();
  bool txPacket();
  bool waitAck();
  virtual void handleData(const ByteVector&) = 0;
public:
  DDAProtocol();
  virtual ~DDAProtocol();
  bool exec();
  bool open(int serial, int baud);
  bool open(const char *device, int baud);
  void close();
  void pushData(unsigned char header, const ByteVector &data = ByteVector());

};
/*----------------------------------------------------------------------------*/
#endif /*DDA_PROTOCOL_H_1359207722*/

