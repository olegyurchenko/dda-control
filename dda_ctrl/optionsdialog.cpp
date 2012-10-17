#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QDir>
#include <QCoreApplication>
#include <configuration.h>
#include <QtDebug>
#include <QMessageBox>
#include <QTranslator>
#include <QLocale>
#include <QFileDialog>
#include <database.h>
/*----------------------------------------------------------------------------*/
OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
  ui->setupUi(this);

  DDASettings settings = config->settings();
  langMap["\0"] = "Unused";
  langMap["\0"] = "C";
  langMap["ab\0"] = "Abkhazian";
  langMap["om\0"] = "Afan";
  langMap["aa\0"] = "Afar";
  langMap["af\0"] = "Afrikaans";
  langMap["sq\0"] = "Albanian";
  langMap["am\0"] = "Amharic";
  langMap["ar\0"] = "Arabic";
  langMap["hy\0"] = "Armenian";
  langMap["as\0"] = "Assamese";
  langMap["ay\0"] = "Aymara";
  langMap["az\0"] = "Azerbaijani";
  langMap["ba\0"] = "Bashkir";
  langMap["eu\0"] = "Basque";
  langMap["bn\0"] = "Bengali";
  langMap["dz\0"] = "Bhutani";
  langMap["bh\0"] = "Bihari";
  langMap["bi\0"] = "Bislama";
  langMap["br\0"] = "Breton";
  langMap["bg\0"] = "Bulgarian";
  langMap["my\0"] = "Burmese";
  langMap["be\0"] = "Byelorussian";
  langMap["km\0"] = "Cambodian";
  langMap["ca\0"] = "Catalan";
  langMap["zh\0"] = "Chinese";
  langMap["co\0"] = "Corsican";
  langMap["hr\0"] = "Croatian";
  langMap["cs\0"] = "Czech";
  langMap["da\0"] = "Danish";
  langMap["nl\0"] = "Dutch";
  langMap["en\0"] = "English";
  langMap["eo\0"] = "Esperanto";
  langMap["et\0"] = "Estonian";
  langMap["fo\0"] = "Faroese";
  langMap["fj\0"] = "Fiji";
  langMap["fi\0"] = "Finnish";
  langMap["fr\0"] = "French";
  langMap["fy\0"] = "Frisian";
  langMap["gd\0"] = "Gaelic";
  langMap["gl\0"] = "Galician";
  langMap["ka\0"] = "Georgian";
  langMap["de\0"] = "German";
  langMap["el\0"] = "Greek";
  langMap["kl\0"] = "Greenlandic";
  langMap["gn\0"] = "Guarani";
  langMap["gu\0"] = "Gujarati";
  langMap["ha\0"] = "Hausa";
  langMap["he\0"] = "Hebrew";
  langMap["hi\0"] = "Hindi";
  langMap["hu\0"] = "Hungarian";
  langMap["is\0"] = "Icelandic";
  langMap["id\0"] = "Indonesian";
  langMap["ia\0"] = "Interlingua";
  langMap["ie\0"] = "Interlingue";
  langMap["iu\0"] = "Inuktitut";
  langMap["ik\0"] = "Inupiak";
  langMap["ga\0"] = "Irish";
  langMap["it\0"] = "Italian";
  langMap["ja\0"] = "Japanese";
  langMap["jv\0"] = "Javanese";
  langMap["kn\0"] = "Kannada";
  langMap["ks\0"] = "Kashmiri";
  langMap["kk\0"] = "Kazakh";
  langMap["rw\0"] = "Kinyarwanda";
  langMap["ky\0"] = "Kirghiz";
  langMap["ko\0"] = "Korean";
  langMap["ku\0"] = "Kurdish";
  langMap["rn\0"] = "Kurundi";
  langMap["lo\0"] = "Laothian";
  langMap["la\0"] = "Latin";
  langMap["lv\0"] = "Latvian";
  langMap["ln\0"] = "Lingala";
  langMap["lt\0"] = "Lithuanian";
  langMap["mk\0"] = "Macedonian";
  langMap["mg\0"] = "Malagasy";
  langMap["ms\0"] = "Malay";
  langMap["ml\0"] = "Malayalam";
  langMap["mt\0"] = "Maltese";
  langMap["mi\0"] = "Maori";
  langMap["mr\0"] = "Marathi";
  langMap["mo\0"] = "Moldavian";
  langMap["mn\0"] = "Mongolian";
  langMap["na\0"] = "Nauru";
  langMap["ne\0"] = "Nepali";
  langMap["nb\0"] = "Norwegian";
  langMap["oc\0"] = "Occitan";
  langMap["or\0"] = "Oriya";
  langMap["ps\0"] = "Pashto";
  langMap["fa\0"] = "Persian";
  langMap["pl\0"] = "Polish";
  langMap["pt\0"] = "Portuguese";
  langMap["pa\0"] = "Punjabi";
  langMap["qu\0"] = "Quechua";
  langMap["rm\0"] = "RhaetoRomance";
  langMap["ro\0"] = "Romanian";
  langMap["ru\0"] = "Russian";
  langMap["sm\0"] = "Samoan";
  langMap["sg\0"] = "Sangho";
  langMap["sa\0"] = "Sanskrit";
  langMap["sr\0"] = "Serbian";
  langMap["sh\0"] = "SerboCroatian";
  langMap["st\0"] = "Sesotho";
  langMap["tn\0"] = "Setswana";
  langMap["sn\0"] = "Shona";
  langMap["sd\0"] = "Sindhi";
  langMap["si\0"] = "Singhalese";
  langMap["ss\0"] = "Siswati";
  langMap["sk\0"] = "Slovak";
  langMap["sl\0"] = "Slovenian";
  langMap["so\0"] = "Somali";
  langMap["es\0"] = "Spanish";
  langMap["su\0"] = "Sundanese";
  langMap["sw\0"] = "Swahili";
  langMap["sv\0"] = "Swedish";
  langMap["tl\0"] = "Tagalog";
  langMap["tg\0"] = "Tajik";
  langMap["ta\0"] = "Tamil";
  langMap["tt\0"] = "Tatar";
  langMap["te\0"] = "Telugu";
  langMap["th\0"] = "Thai";
  langMap["bo\0"] = "Tibetan";
  langMap["ti\0"] = "Tigrinya";
  langMap["to\0"] = "Tonga";
  langMap["ts\0"] = "Tsonga";
  langMap["tr\0"] = "Turkish";
  langMap["tk\0"] = "Turkmen";
  langMap["tw\0"] = "Twi";
  langMap["ug\0"] = "Uigur";
  langMap["uk\0"] = "Ukrainian";
  langMap["ur\0"] = "Urdu";
  langMap["uz\0"] = "Uzbek";
  langMap["vi\0"] = "Vietnamese";
  langMap["vo\0"] = "Volapuk";
  langMap["cy\0"] = "Welsh";
  langMap["wo\0"] = "Wolof";
  langMap["xh\0"] = "Xhosa";
  langMap["yi\0"] = "Yiddish";
  langMap["yo\0"] = "Yoruba";
  langMap["za\0"] = "Zhuang";
  langMap["zu\0"] = "Zulu";
  langMap["nn\0"] = "Nynorsk";
  langMap["bs\0"] = "Bosnian";
  langMap["dv\0"] = "Divehi";
  langMap["gv\0"] = "Manx";
  langMap["kw\0"] = "Cornish";
  langMap["ak\0"] = "Akan";
  langMap["kok"] = "Konkani";
  langMap["gaa"] = "Ga";
  langMap["ig\0"] = "Igbo";
  langMap["kam"] = "Kamba";
  langMap["syr"] = "Syriac";
  langMap["byn"] = "Blin";
  langMap["gez"] = "Geez";
  langMap["kfo"] = "Koro";
  langMap["sid"] = "Sidamo";
  langMap["cch"] = "Atsam";
  langMap["tig"] = "Tigre";
  langMap["kaj"] = "Jju";
  langMap["fur"] = "Friulian";
  langMap["ve\0"] = "Venda";
  langMap["ee\0"] = "Ewe";
  langMap["wal"] = "Walamo";
  langMap["haw"] = "Hawaiian";
  langMap["kcg"] = "Tyap";
  langMap["ny\0"] = "Chewa";
  langMap["fil"] = "Filipino";
  langMap["gsw"] = "SwissGerman";
  langMap["ii\0"] = "SichuanYi";
  langMap["kpe"] = "Kpelle";
  langMap["nds"] = "LowGerman";
  langMap["nr\0"] = "SouthNdebele";
  langMap["nso"] = "NorthernSotho";
  langMap["se\0"] = "NorthernSami";
  langMap["trv"] = "Taroko";
  langMap["guz"] = "Gusii";
  langMap["dav"] = "Taita";
  langMap["ff\0"] = "Fulah";
  langMap["ki\0"] = "Kikuyu";
  langMap["saq"] = "Samburu";
  langMap["seh"] = "Sena";
  langMap["nd\0"] = "NorthNdebele";
  langMap["rof"] = "Rombo";
  langMap["shi"] = "Tachelhit";
  langMap["kab"] = "Kabyle";
  langMap["nyn"] = "Nyankole";
  langMap["bez"] = "Bena";
  langMap["vun"] = "Vunjo";
  langMap["bm\0"] = "Bambara";
  langMap["ebu"] = "Embu";
  langMap["chr"] = "Cherokee";
  langMap["mfe"] = "Morisyen";
  langMap["kde"] = "Makonde";
  langMap["lag"] = "Langi";
  langMap["lg\0"] = "Ganda";
  langMap["bem"] = "Bemba";
  langMap["kea"] = "Kabuverdianu";
  langMap["mer"] = "Meru";
  langMap["kln"] = "Kalenjin";
  langMap["naq"] = "Nama";
  langMap["jmc"] = "Machame";
  langMap["ksh"] = "Colognian";
  langMap["mas"] = "Masai";
  langMap["xog"] = "Soga";
  langMap["luy"] = "Luyia";
  langMap["asa"] = "Asu";
  langMap["teo"] = "Teso";
  langMap["ssy"] = "Saho";
  langMap["khq"] = "KoyraChiini";
  langMap["rwk"] = "Rwa";
  langMap["luo"] = "Luo";
  langMap["cgg"] = "Chiga";
  langMap["tzm"] = "CentralMoroccoTamazight";
  langMap["ses"] = "KoyraboroSenni";
  langMap["ksb"] = "Shambala";

  ui->localeCombo->addItem("English", "en");
  QDir current = QDir(QCoreApplication::applicationDirPath());
  current.setFilter(QDir::Files);
  QStringList filters;
  filters << "dda-*.qm";
  current.setNameFilters(filters);
  QStringList entries = current.entryList();
  int currentIndex = 0;
  int size = entries.size();
  for(int i = 0; i < size; i++)
  {
    QString file = entries[i];
    int p1 = file.indexOf('-');
    int p2 = file.indexOf('.');
    QString lang = file.mid(p1 + 1, p2 - p1 - 1);
    //qDebug() << file << p1 << p2 << lang;
    ui->localeCombo->addItem(langMap[lang], lang);
    if(settings.localeName == lang)
      currentIndex = i + 1;
  }
  ui->localeCombo->setCurrentIndex(currentIndex);
  ui->databaseEdit->setText(settings.databaseFileName);
}
/*----------------------------------------------------------------------------*/
OptionsDialog::~OptionsDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void OptionsDialog::onOpenFile()
{
  QString fileName =  QFileDialog::getSaveFileName(this,
    tr("Select database file"),
    QString(),
    tr("Databases (*.db);;All files (*)"),
    0,
    QFileDialog::DontConfirmOverwrite
  );
  if(!fileName.isEmpty())
    ui->databaseEdit->setText(fileName);
}
/*----------------------------------------------------------------------------*/
void OptionsDialog::onAccept()
{
  DDASettings settings = config->settings();
  DDASettings oldSettings = settings;
  int langIndex = ui->localeCombo->currentIndex();
  QString lang = ui->localeCombo->model()->data( ui->localeCombo->model()->index(langIndex, 0), Qt::UserRole).toString();

  settings.localeName = lang;
  settings.databaseFileName = ui->databaseEdit->text();

  DDADatabase *db = NULL;

  if(settings.databaseFileName != oldSettings.databaseFileName)
  {
    db = new DDADatabase(qApp, settings.databaseFileName);
    if(db->isError())
    {
      QMessageBox::critical(NULL, QObject::tr("Error open database"), database->message());
      delete db;
      return;
    }
  }

  if(settings.localeName != oldSettings.localeName)
  {
    QTranslator *trans = new QTranslator(qApp);
    if(settings.localeName != "en")
    {
      if(!trans->load("dda-" + settings.localeName)
        && !trans->load("dda-" + settings.localeName, QCoreApplication::applicationDirPath()))
      {
        QString error = QString("Error load language file for `%1` locale").arg(settings.localeName);
        QMessageBox::critical(NULL, QObject::tr("Error translation"), error);
        delete trans;
        return;
      }
    }

    if(translator != NULL)
      QCoreApplication::removeTranslator(translator);
    QCoreApplication::installTranslator(trans);
    translator = trans;
  }

  if(db != NULL)
  {
    delete database;
    database = db;
  }

  config->setSettings(settings);
  accept();
}
/*----------------------------------------------------------------------------*/
