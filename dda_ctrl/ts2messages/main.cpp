#include <QtCore/QCoreApplication>
#include <stdio.h>
#include <get_opts.h>
#include <message_file.h>
#include <QDomDocument>
#include <QFile>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
#define PRODUCT "Qt language .ts file convertor to message file format"
#define VERSION "0.0.1"
#define COPYRIGHT "(C) DDA Team 2012"
#define BUILD __DATE__
void help(char *argv0, opt_args *args);
void version();
bool verbose = false;
/*----------------------------------------------------------------------------*/
MessageFile messageFile;
/*----------------------------------------------------------------------------*/
bool processFile(const char *fileName);
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  opt_args args;
  args.add("help", 'h', "This help");
  args.add("version", 'V', "Print version and exit");
  args.add("verbose", 'v', "Be verbose");
  args.add("create", 'c', "Create new message file");
  args.add("message-file", 'm', "", "Set message file name");
  if(!args.parse(argc, argv))
  {
    fprintf(stderr,"%s\n", args.error().c_str());
    help(argv[0], &args);
    return 1;
  }

  if(args.get("help") || args.non_options.empty())
  {
    help(argv[0], &args);
    return 0;
  }

  if(args.get("version"))
  {
    version();
    return 0;
  }

  verbose = args.get("verbose");

  std::string fileName;
  args.get("message-file", &fileName);
  if(fileName.empty())
  {
    fprintf(stderr, "Message file not defined. Use -m option\n");
    return 1;
  }

  if(!args.get("create"))
  {
    if(!messageFile.load(fileName.c_str()))
    {
      fprintf(stderr, "Error load message file: %s\n", messageFile.errorString().toLocal8Bit().constData());
      return 2;
    }
  }

  size_t size = args.non_options.size();
  for(size_t i = 0; i < size; i++)
  {
    if(!processFile(args.non_options[i].c_str()))
      return 3;
  }

  if(!messageFile.save(fileName.c_str()))
  {
    fprintf(stderr, "Error load message file: %s\n", messageFile.errorString().toLocal8Bit().constData());
    return 2;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
void help(char *argv0, opt_args *args)
{
  printf("%s V%s (build %s)\n", PRODUCT, VERSION, BUILD);
  printf("Using: %s [switches] ts-file [ts-file2, ...] \nWhere switches are:\n%s\n", argv0, args->help().c_str());
}
//---------------------------------------------------------------------------
void version()
{
  printf("%s V%s (build %s)\n%s\n", PRODUCT, VERSION, BUILD, COPYRIGHT);
}
//---------------------------------------------------------------------------
bool processFile(const char *fileName)
{
  QFile tsFile(fileName);

  if(!tsFile.open(QIODevice::ReadOnly))
  {
    qDebug() << "Error open file" << fileName << tsFile.errorString();
    return false;
  }

  QDomDocument doc;

  QString errMsg;
  int errorLine, errorColumn;
  if(!doc.setContent(&tsFile, &errMsg, &errorLine, &errorColumn))
  {
    qDebug() << "Error parse file" << fileName << QString("Error %1:%2:%3: %4").arg(tsFile.fileName()).arg(errorLine).arg(errorColumn).arg(errMsg);
    return false;
  }

  QDomElement el = doc.firstChildElement("TS");
  QString lang = el.attribute("language");
  if(verbose)
    qDebug() << "Lang" << lang;

  messageFile.setLang(lang);
  for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
    if(n.nodeName() == "context")
    {
      for(QDomNode nn = n.firstChild(); !nn.isNull(); nn = nn.nextSibling())
      {
        if(nn.nodeName() == "message")
        {
          QString source;
          for(QDomNode nnn = nn.firstChild(); !nnn.isNull(); nnn = nnn.nextSibling())
          {
            if(nnn.nodeName() == "source")
            {
              source = nnn.toElement().text();
              messageFile.addMessage(source, source, "en_US");
            }
            else
              if(nnn.nodeName() == "translation")
              {
                QString txt = nnn.toElement().text();
                messageFile.setMessage(source, txt);
                if(verbose)
                  qDebug() << source << txt;
                break;
              }
          }
        }
      }
    }
  return true;
}
//---------------------------------------------------------------------------
