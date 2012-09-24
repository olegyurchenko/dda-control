/*----------------------------------------------------------------------------*/
/**
* @pkg configuration
*/
/**
* Configuration for DDA.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 24.09.2012 10:58:13<br>
* @pkgdoc configuration
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef CONFIGURATION_H_1348473493
#define CONFIGURATION_H_1348473493
#include <QObject>
#include <QStringList>
/*----------------------------------------------------------------------------*/
struct DDAProfile
{
  QString name;
  QString databaseFileName;
  int serial;
  int baud;
};
/*----------------------------------------------------------------------------*/
class QDomNode;
class QDomDocument;
class DDAConfig : public QObject
{
protected:
  QString m_path;
  int m_profileIndex;
  DDAProfile m_profile;
  typedef QList<DDAProfile> ProfileList;
  ProfileList m_profileList;
  bool m_fileExists;
  bool m_isError;
  QString m_message;

  bool load();
  bool save();
  void parceProfile(QDomNode *node, DDAProfile *dst);
  void saveProfile(const DDAProfile *src, QDomDocument *doc, QDomNode *dst);
  void defaultProfle(DDAProfile *dst);
public:
  DDAConfig(QObject *parent = 0);
  ~DDAConfig();
  QStringList profileList();
  void setProfileIndex(int index);
  void addDefualtProfile();
  int profileIndex() const {return m_profileIndex;}
  const DDAProfile *profile() const {return &m_profile;}
  bool setProfile(const DDAProfile *profile);
  bool fileExists() const {return m_fileExists;}
  bool isError() const {return m_isError;}
  QString message() {return m_message;}
};
/*----------------------------------------------------------------------------*/
extern DDAConfig *config;
/*----------------------------------------------------------------------------*/
#endif /*CONFIGURATION_H_1348473493*/

