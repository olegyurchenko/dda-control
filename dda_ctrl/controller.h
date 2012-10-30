/*----------------------------------------------------------------------------*/
/**
* @pkg controller
*/
/**
* DDA controller.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 01.10.2012  9:47:44<br>
* @pkgdoc controller
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef CONTROLLER_H_1349074064
#define CONTROLLER_H_1349074064
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QList>
#include <QTime>
/*----------------------------------------------------------------------------*/
class logfile;
class DDAController : public QThread
{
  Q_OBJECT
public:
  enum DDAStatus
  {
    Offline,
    Idle,
    Calibrating,
    Measuring,
    NoParticle,
    NextCasseteWaiting,
    EndOfMeasuring
  };

  enum DDAMode
  {
    Manual = 0,
    Auto = 1
  };

protected:
  typedef QVector<unsigned char> ByteVector;
  typedef QList<ByteVector> BinList;
  virtual void run();
  DDAStatus m_status;
  int m_serial;
  bool m_terminated;
  BinList m_txData;
  ByteVector m_rxData;
  QTime m_lastCharTime;
  logfile *m_log;

  double m_size;
  double m_strength;
  QString m_deviceSerial;
  DDAMode m_mode;

  QMutex m_mutex;
  void lock() {m_mutex.lock();}
  void unlock() {m_mutex.unlock();}

  int serial_read(int fd, void *buffer, unsigned size, unsigned timeout);
  int serial_write(int fd, void *buffer, unsigned size);

  void setStatus(DDAStatus s);
  bool rxPacket(int serial);
  bool txPacket(int serial);
  bool waitAck(int serial);
  void handleData(const ByteVector& data);

  void setSize(double s) {m_size = s;}
  void setStrength(double s) {m_strength = s;}
  void setDeviceSerial(const QString& s){m_deviceSerial = s;}
public:
  DDAController(QObject *parent = NULL);
  ~DDAController();
  bool open(int serial, int baud);
  void close();
  DDAStatus status() const {return m_status;}
  static QString statusString(int s);

  virtual void setMode(int meshIndex, int samples, DDAMode mode = Auto);
  virtual void resume();
  virtual void manualMode();
  virtual void autoMode();

  double size() {return m_size;}
  double strength() {return m_strength;}
  QString deviceSerial() {return m_deviceSerial;}

signals:
  void statusChanged(int s);
  void cmdSendOk();
  void cmdSendError();
  void serialReceived(const QString&);
  void currentStretch(double);
  void sizeReceived(double);
  void noParticle();
  void measure(double strength, double size, int number);
  void nextCasseteRequest();
  void endOfMeasuring();

};
/*----------------------------------------------------------------------------*/
class DemoController : public DDAController
{
protected:
  virtual void run();
  void doSimulation();
  int m_count;
  QTime m_time;
  double m_strength;
  int m_number;
  int m_nextCell;
  int m_particles;

  enum
  {
    NoCmd,
    StartCmd
  } m_cmd;
public:
  DemoController(QObject *parent = 0);
  virtual void setMode(int meshIndex, int samples, DDAMode mode = Auto);
  virtual void resume();
  virtual void manualMode();
};
/*----------------------------------------------------------------------------*/
extern DDAController *controller;
/*----------------------------------------------------------------------------*/
#endif /*CONTROLLER_H_1349074064*/

