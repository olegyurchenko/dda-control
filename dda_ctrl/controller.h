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
#include <QTime>
/*----------------------------------------------------------------------------*/
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
  virtual void run();
  DDAStatus m_status;
  int m_serial;
  bool m_terminated;
  ByteVector m_txData;
  ByteVector m_rxData;
  QTime m_lastCharTime;

  QMutex m_mutex;
  void lock() {m_mutex.lock();}
  void unlock() {m_mutex.unlock();}

  void setStatus(DDAStatus s);
  bool rxPacket(int serial);
  bool txPacket(int serial);
  bool waitAck(int serial);
  void handleData(const ByteVector& data);
public:
  DDAController(QObject *parent = NULL);
  ~DDAController();
  bool open(int serial, int baud);
  void close();
  DDAStatus status() const {return m_status;}
  static QString statusString(int s);

  void setMode(int meshIndex, int samples, DDAMode mode = Auto);
  void start();
  void manualMode();

signals:
  void statusChanged(int s);
  void cmdSendOk();
  void cmdSendError();
  void serialReceived(const QString&);
  void currentStretch(double);
  void noParticle();
  void measure(double strength, int number, int nextCell);
  void giftSize(double size);
  void nextCasseteRequest();
  void endOfMeasuring();

};
/*----------------------------------------------------------------------------*/
extern DDAController *controller;
/*----------------------------------------------------------------------------*/
#endif /*CONTROLLER_H_1349074064*/

