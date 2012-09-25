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
  int serial;
  int baud;
};
/*----------------------------------------------------------------------------*/
struct DDASettings
{
  QString databaseFileName;
};
/*----------------------------------------------------------------------------*/
class QDomNode;
class QDomDocument;
class DDAConfig : public QObject
{
  Q_OBJECT

protected:
  QString m_path;
  int m_profileIndex;
  typedef QList<DDAProfile> ProfileList;
  ProfileList m_profileList;
  DDASettings m_settings;
  bool m_fileExists;
  bool m_isError;
  QString m_message;

  bool load();
  bool save();
  void parceProfile(QDomNode *node, DDAProfile *dst);
  void saveProfile(const DDAProfile *src, QDomDocument *doc, QDomNode *dst);
  void parceSettings(QDomNode *node, DDASettings *dst);
  void saveSettings(const DDASettings *src, QDomDocument *doc, QDomNode *dst);
public:
  DDAConfig(QObject *parent = 0);
  ~DDAConfig();
  QStringList profileList();
  void setProfileIndex(int index);
  void defaultProfle(DDAProfile *dst) const;
  void defaultSettings(DDASettings *dst) const;
  int profileIndex() const {return m_profileIndex;}
  const DDAProfile &profile() const;
  bool setProfile(const DDAProfile& profile);
  bool addProfile(const DDAProfile& profile);
  const DDASettings& settings() {return m_settings;}
  bool setSettings(const DDASettings& s) {m_settings = s; emit settingsChanged(); return save();}
  bool rmProfile(int index);

  bool fileExists() const {return m_fileExists;}
  bool isError() const {return m_isError;}
  QString message() {return m_message;}
signals:
  void profileChanged();
  void settingsChanged();
};
/*----------------------------------------------------------------------------*/
extern DDAConfig *config;
/*----------------------------------------------------------------------------*/
#endif /*CONFIGURATION_H_1348473493*/

