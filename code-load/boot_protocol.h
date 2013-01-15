/*----------------------------------------------------------------------------*/
/**
* @pkg boot_protocol
*/
/**
* Short description of boot_protocol.
*
* Long description of boot_protocol.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 13.11.2012 11:49:52<br>
* @pkgdoc boot_protocol
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef BOOT_PROTOCOL_H_1352800192
#define BOOT_PROTOCOL_H_1352800192
/*----------------------------------------------------------------------------*/
#include <string>
#include <vector>

#define PAGE_SIZE 2048
class BootLoaderProtocol
{
public:
  typedef std::vector<unsigned char> ByteVector;
protected:
  int m_serial;
  std::string m_errorString;
  ByteVector m_allowedCmd;
  unsigned char m_version;

  bool m_verbose;
protected:
  bool sendComplement(unsigned char);
  bool waitAck();
public:
  BootLoaderProtocol();
  virtual ~BootLoaderProtocol();

  bool open(int serial, int baud);
  bool open(const char *device, int baud);

  std::string errorString() {return m_errorString;}
  void setVerbose(bool v) {m_verbose = v;}
  bool verbose() {return m_verbose;}
  unsigned char version() {return m_version;}
  const ByteVector& allowedCmd() {return m_allowedCmd;}

  bool getVersionAndAllowedCmd();
  bool readMemory(unsigned addr, unsigned short size, unsigned char *dst);
  bool writeMemory(unsigned addr, unsigned short size, const unsigned char *src);
  bool eraseFlash(unsigned short pageCount, const unsigned char *pages);


};

/*----------------------------------------------------------------------------*/
#endif /*BOOT_PROTOCOL_H_1352800192*/

