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
#include "configuration.h"
#include <QDir>
#include <QDomDocument>
#include <QFile>
/*----------------------------------------------------------------------------*/
DDAConfig *config = NULL;
/*----------------------------------------------------------------------------*/
DDAConfig :: DDAConfig(QObject *parent)
  : QObject(parent)
{
  m_profileIndex = -1;
  QDir dir = QDir::home();
  if(!dir.exists(".dda"))
    dir.mkpath(".dda");
  dir.setPath(dir.filePath(".dda"));
  m_path = dir.filePath("config.xml");
  m_fileExists = dir.exists("config.xml");
  defaultSettings(&m_settings);
  if(m_fileExists)
    m_isError = !load();
}
/*----------------------------------------------------------------------------*/
DDAConfig :: ~DDAConfig()
{
}
/*----------------------------------------------------------------------------*/
bool DDAConfig :: load()
{
  QFile confFile(m_path);
  QDomDocument doc("configuration");

  if(!confFile.open(QIODevice::ReadOnly))
  {
    m_message = confFile.errorString();
    return false;
  }
  //QString xml = QString::fromUtf8(confFile.readAll());

  QString errMsg;
  int errorLine, errorColumn;
  if(!doc.setContent(&confFile, &errMsg, &errorLine, &errorColumn))
  {
    m_message = QString("Error %1:%2:%3: %4").arg(m_path).arg(errorLine).arg(errorColumn).arg(errMsg);
    return false;
  }

  QDomElement el = doc.firstChildElement("config");
  for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    if(n.nodeName() == "profile")
    {
      DDAProfile profile;
      parceProfile(&n, &profile);
      m_profileList.append(profile);
    }
  }
  parceSettings(&el, &m_settings);
  //save();
  if(m_profileIndex == -1 && m_profileList.size())
    m_profileIndex = 0;
  return true;
}
/*----------------------------------------------------------------------------*/
bool DDAConfig :: save()
{
  QDomDocument doc;

  doc.appendChild(doc.createProcessingInstruction(
        "xml",
        "version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\""
        ));

  QDomElement root = doc.createElement("config");
  doc.appendChild(root);

  int size = m_profileList.size();
  for(int i = 0; i < size; i++)
  {
    QDomElement tag = doc.createElement("profile");
    saveProfile(&m_profileList.at(i), &doc, &tag);
    root.appendChild(tag);
  }

  saveSettings(&m_settings, &doc, &root);

  QString xml = doc.toString();
  QFile confFile(m_path);

  if(!confFile.open(QIODevice::WriteOnly))
  {
    m_message = confFile.errorString();
    return false;
  }

  if(!confFile.write(xml.toUtf8()))
  {
    m_message = confFile.errorString();
    return false;
  }
  return true;
}
/*----------------------------------------------------------------------------*/
void DDAConfig :: defaultProfle(DDAProfile *dst) const
{
  dst->name = m_profileList.isEmpty() ? tr("Default profile") : tr("Profile_%1").arg(m_profileList.size() + 1);
  dst->serial = 1;
  dst->baud = 9600;
  dst->meshIndex = 0;
  dst->gostIndex = 0;
  dst->giftCount = 1; //!!!!
}
/*----------------------------------------------------------------------------*/
void DDAConfig :: defaultSettings(DDASettings *dst) const
{
  QDir dir = QDir::home();
  if(!dir.exists(".dda"))
    dir.mkpath(".dda");
  dir.setPath(dir.filePath(".dda"));
  dst->databaseFileName = dir.filePath("main.db");
}
/*----------------------------------------------------------------------------*/
void DDAConfig :: parceProfile(QDomNode *node, DDAProfile *dst)
{
  defaultProfle(dst);
  QDomNode n = node->firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull())
    {
      if(e.tagName() == "name")
        dst->name = e.text();
      else
      if(e.tagName() == "serial")
        dst->serial = e.text().toInt();
      else
      if(e.tagName() == "baud")
        dst->baud = e.text().toInt();
      else
      if(e.tagName() == "meshIndex")
        dst->meshIndex = e.text().toInt();
      else
      if(e.tagName() == "gostIndex")
        dst->gostIndex = e.text().toInt();
      else
      if(e.tagName() == "giftCount")
        dst->giftCount = e.text().toInt();
    }
    n = n.nextSibling();
  }

}
/*----------------------------------------------------------------------------*/
void DDAConfig :: saveProfile(const DDAProfile *src, QDomDocument *doc, QDomNode *dst)
{
  QDomElement e = doc->createElement("name");
  e.appendChild(doc->createTextNode(src->name));
  dst->appendChild(e);

  e = doc->createElement("serial");
  e.appendChild(doc->createTextNode(QString::number(src->serial)));
  dst->appendChild(e);

  e = doc->createElement("baud");
  e.appendChild(doc->createTextNode(QString::number(src->baud)));
  dst->appendChild(e);

  e = doc->createElement("meshIndex");
  e.appendChild(doc->createTextNode(QString::number(src->meshIndex)));
  dst->appendChild(e);

  e = doc->createElement("gostIndex");
  e.appendChild(doc->createTextNode(QString::number(src->gostIndex)));
  dst->appendChild(e);

  e = doc->createElement("giftCount");
  e.appendChild(doc->createTextNode(QString::number(src->giftCount)));
  dst->appendChild(e);
}
/*----------------------------------------------------------------------------*/
void DDAConfig :: parceSettings(QDomNode *node, DDASettings *dst)
{
  defaultSettings(dst);
  QDomNode n = node->firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull())
    {
      if(e.tagName() == "databaseFileName")
        dst->databaseFileName = e.text();
      if(e.tagName() == "profileIndex")
        m_profileIndex = e.text().toInt();
    }
    n = n.nextSibling();
  }
}
/*----------------------------------------------------------------------------*/
void DDAConfig :: saveSettings(const DDASettings *src, QDomDocument *doc, QDomNode *dst)
{
  QDomElement e = doc->createElement("databaseFileName");
  e.appendChild(doc->createTextNode(src->databaseFileName));
  dst->appendChild(e);

  e = doc->createElement("profileIndex");
  e.appendChild(doc->createTextNode(QString::number(m_profileIndex)));
  dst->appendChild(e);
}
/*----------------------------------------------------------------------------*/
QStringList DDAConfig :: profileList()
{
  QStringList lst;
  int size = m_profileList.size();
  for(int i = 0; i < size; i++)
    lst << m_profileList[i].name;
  return lst;
}
/*----------------------------------------------------------------------------*/
void DDAConfig :: setProfileIndex(int index)
{
  if(index >= 0 && index < m_profileList.size())
  {
    if(m_profileIndex != index)
    {
      m_profileIndex = index;
      save();
      emit profileChanged();
    }
  }
}
/*----------------------------------------------------------------------------*/
const DDAProfile& DDAConfig :: profile() const
{
  static DDAProfile p;

  if(m_profileIndex >= 0 && m_profileIndex < m_profileList.size())
    return m_profileList.at(m_profileIndex);

  defaultProfle(&p);
  return p;
}
/*----------------------------------------------------------------------------*/
bool DDAConfig :: setProfile(const DDAProfile &profile)
{
  if(m_profileIndex >= 0 && m_profileIndex < m_profileList.size())
  {
    m_profileList[m_profileIndex] = profile;
    if(save())
    {
      emit profileChanged();
      return true;
    }
    return false;
  }
  m_message = tr("Invalid profile index");
  return false;
}
/*----------------------------------------------------------------------------*/
bool DDAConfig :: addProfile(const DDAProfile &profile)
{
  m_profileList.append(profile);
  setProfileIndex(m_profileList.size() - 1);
  return save();
}
/*----------------------------------------------------------------------------*/
bool DDAConfig :: rmProfile(int index)
{
  if(index >= 0 && index < m_profileList.size())
  {
    m_profileList.erase(m_profileList.begin() + index);
    m_profileIndex = -1;
    return save();
  }
  m_message = tr("Invalid profile index");
  return false;
}
/*----------------------------------------------------------------------------*/
