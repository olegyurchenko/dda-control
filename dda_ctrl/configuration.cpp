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
  QDir dir = QDir::home();
  if(!dir.exists(".dda"))
    dir.mkpath(".dda");
  dir.setPath(dir.filePath(".dda"));
  m_path = dir.filePath("config.xml");
  m_fileExists = dir.exists("config.xml");
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
  save();
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
void DDAConfig :: defaultProfle(DDAProfile *dst)
{
  dst->name = QString("Profile_%1").arg(m_profileList.size() + 1);
  dst->databaseFileName = "";
  dst->serial = 1;
  dst->baud = 9600;

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
      if(e.tagName() == "databaseFileName")
        dst->databaseFileName = e.text();
      else
      if(e.tagName() == "serial")
        dst->serial = e.text().toInt();
      else
      if(e.tagName() == "baud")
        dst->baud = e.text().toInt();
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

  e = doc->createElement("databaseFileName");
  e.appendChild(doc->createTextNode(src->databaseFileName));
  dst->appendChild(e);

  e = doc->createElement("serial");
  e.appendChild(doc->createTextNode(QString::number(src->serial)));
  dst->appendChild(e);

  e = doc->createElement("baud");
  e.appendChild(doc->createTextNode(QString::number(src->baud)));
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
}
/*----------------------------------------------------------------------------*/
void DDAConfig :: addDefualtProfile()
{
}
/*----------------------------------------------------------------------------*/
bool DDAConfig :: setProfile(const DDAProfile *profile)
{
}
/*----------------------------------------------------------------------------*/
