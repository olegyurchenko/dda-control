#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QDir>
#include <QCoreApplication>
#include <configuration.h>
#include <QtDebug>
#include <QMessageBox>
#include <translator.h>
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

  countryMap["AF\0"] = "Afghanistan";
  countryMap["AL\0"] = "Albania";
  countryMap["DZ\0"] = "Algeria";
  countryMap["AS\0"] = "AmericanSamoa";
  countryMap["AD\0"] = "Andorra";
  countryMap["AO\0"] = "Angola";
  countryMap["AI\0"] = "Anguilla";
  countryMap["AQ\0"] = "Antarctica";
  countryMap["AG\0"] = "AntiguaAndBarbuda";
  countryMap["AR\0"] = "Argentina";
  countryMap["AM\0"] = "Armenia";
  countryMap["AW\0"] = "Aruba";
  countryMap["AU\0"] = "Australia";
  countryMap["AT\0"] = "Austria";
  countryMap["AZ\0"] = "Azerbaijan";
  countryMap["BS\0"] = "Bahamas";
  countryMap["BH\0"] = "Bahrain";
  countryMap["BD\0"] = "Bangladesh";
  countryMap["BB\0"] = "Barbados";
  countryMap["BY\0"] = "Belarus";
  countryMap["BE\0"] = "Belgium";
  countryMap["BZ\0"] = "Belize";
  countryMap["BJ\0"] = "Benin";
  countryMap["BM\0"] = "Bermuda";
  countryMap["BT\0"] = "Bhutan";
  countryMap["BO\0"] = "Bolivia";
  countryMap["BA\0"] = "BosniaAndHerzegowina";
  countryMap["BW\0"] = "Botswana";
  countryMap["BV\0"] = "BouvetIsland";
  countryMap["BR\0"] = "Brazil";
  countryMap["IO\0"] = "BritishIndianOceanTerritory";
  countryMap["BN\0"] = "BruneiDarussalam";
  countryMap["BG\0"] = "Bulgaria";
  countryMap["BF\0"] = "BurkinaFaso";
  countryMap["BI\0"] = "Burundi";
  countryMap["KH\0"] = "Cambodia";
  countryMap["CM\0"] = "Cameroon";
  countryMap["CA\0"] = "Canada";
  countryMap["CV\0"] = "CapeVerde";
  countryMap["KY\0"] = "CaymanIslands";
  countryMap["CF\0"] = "CentralAfricanRepublic";
  countryMap["TD\0"] = "Chad";
  countryMap["CL\0"] = "Chile";
  countryMap["CN\0"] = "China";
  countryMap["CX\0"] = "ChristmasIsland";
  countryMap["CC\0"] = "CocosIslands";
  countryMap["CO\0"] = "Colombia";
  countryMap["KM\0"] = "Comoros";
  countryMap["CD\0"] = "DemocraticRepublicOfCongo";
  countryMap["CG\0"] = "PeoplesRepublicOfCongo";
  countryMap["CK\0"] = "CookIslands";
  countryMap["CR\0"] = "CostaRica";
  countryMap["CI\0"] = "IvoryCoast";
  countryMap["HR\0"] = "Croatia";
  countryMap["CU\0"] = "Cuba";
  countryMap["CY\0"] = "Cyprus";
  countryMap["CZ\0"] = "CzechRepublic";
  countryMap["DK\0"] = "Denmark";
  countryMap["DJ\0"] = "Djibouti";
  countryMap["DM\0"] = "Dominica";
  countryMap["DO\0"] = "DominicanRepublic";
  countryMap["TL\0"] = "EastTimor";
  countryMap["EC\0"] = "Ecuador";
  countryMap["EG\0"] = "Egypt";
  countryMap["SV\0"] = "ElSalvador";
  countryMap["GQ\0"] = "EquatorialGuinea";
  countryMap["ER\0"] = "Eritrea";
  countryMap["EE\0"] = "Estonia";
  countryMap["ET\0"] = "Ethiopia";
  countryMap["FK\0"] = "FalklandIslands";
  countryMap["FO\0"] = "FaroeIslands";
  countryMap["FJ\0"] = "Fiji";
  countryMap["FI\0"] = "Finland";
  countryMap["FR\0"] = "France";
  countryMap["FX\0"] = "MetropolitanFrance";
  countryMap["GF\0"] = "FrenchGuiana";
  countryMap["PF\0"] = "FrenchPolynesia";
  countryMap["TF\0"] = "FrenchSouthernTerritories";
  countryMap["GA\0"] = "Gabon";
  countryMap["GM\0"] = "Gambia";
  countryMap["GE\0"] = "Georgia";
  countryMap["DE\0"] = "Germany";
  countryMap["GH\0"] = "Ghana";
  countryMap["GI\0"] = "Gibraltar";
  countryMap["GR\0"] = "Greece";
  countryMap["GL\0"] = "Greenland";
  countryMap["GD\0"] = "Grenada";
  countryMap["GP\0"] = "Guadeloupe";
  countryMap["GU\0"] = "Guam";
  countryMap["GT\0"] = "Guatemala";
  countryMap["GN\0"] = "Guinea";
  countryMap["GW\0"] = "GuineaBissau";
  countryMap["GY\0"] = "Guyana";
  countryMap["HT\0"] = "Haiti";
  countryMap["HM\0"] = "HeardAndMcDonaldIslands";
  countryMap["HN\0"] = "Honduras";
  countryMap["HK\0"] = "HongKong";
  countryMap["HU\0"] = "Hungary";
  countryMap["IS\0"] = "Iceland";
  countryMap["IN\0"] = "India";
  countryMap["ID\0"] = "Indonesia";
  countryMap["IR\0"] = "Iran";
  countryMap["IQ\0"] = "Iraq";
  countryMap["IE\0"] = "Ireland";
  countryMap["IL\0"] = "Israel";
  countryMap["IT\0"] = "Italy";
  countryMap["JM\0"] = "Jamaica";
  countryMap["JP\0"] = "Japan";
  countryMap["JO\0"] = "Jordan";
  countryMap["KZ\0"] = "Kazakhstan";
  countryMap["KE\0"] = "Kenya";
  countryMap["KI\0"] = "Kiribati";
  countryMap["KP\0"] = "DemocraticRepublicOfKorea";
  countryMap["KR\0"] = "RepublicOfKorea";
  countryMap["KW\0"] = "Kuwait";
  countryMap["KG\0"] = "Kyrgyzstan";
  countryMap["LA\0"] = "Lao";
  countryMap["LV\0"] = "Latvia";
  countryMap["LB\0"] = "Lebanon";
  countryMap["LS\0"] = "Lesotho";
  countryMap["LR\0"] = "Liberia";
  countryMap["LY\0"] = "LibyanArabJamahiriya";
  countryMap["LI\0"] = "Liechtenstein";
  countryMap["LT\0"] = "Lithuania";
  countryMap["LU\0"] = "Luxembourg";
  countryMap["MO\0"] = "Macau";
  countryMap["MK\0"] = "Macedonia";
  countryMap["MG\0"] = "Madagascar";
  countryMap["MW\0"] = "Malawi";
  countryMap["MY\0"] = "Malaysia";
  countryMap["MV\0"] = "Maldives";
  countryMap["ML\0"] = "Mali";
  countryMap["MT\0"] = "Malta";
  countryMap["MH\0"] = "MarshallIslands";
  countryMap["MQ\0"] = "Martinique";
  countryMap["MR\0"] = "Mauritania";
  countryMap["MU\0"] = "Mauritius";
  countryMap["YT\0"] = "Mayotte";
  countryMap["MX\0"] = "Mexico";
  countryMap["FM\0"] = "Micronesia";
  countryMap["MD\0"] = "Moldova";
  countryMap["MC\0"] = "Monaco";
  countryMap["MN\0"] = "Mongolia";
  countryMap["MS\0"] = "Montserrat";
  countryMap["MA\0"] = "Morocco";
  countryMap["MZ\0"] = "Mozambique";
  countryMap["MM\0"] = "Myanmar";
  countryMap["NA\0"] = "Namibia";
  countryMap["NR\0"] = "Nauru";
  countryMap["NP\0"] = "Nepal";
  countryMap["NL\0"] = "Netherlands";
  countryMap["AN\0"] = "NetherlandsAntilles";
  countryMap["NC\0"] = "NewCaledonia";
  countryMap["NZ\0"] = "NewZealand";
  countryMap["NI\0"] = "Nicaragua";
  countryMap["NE\0"] = "Niger";
  countryMap["NG\0"] = "Nigeria";
  countryMap["NU\0"] = "Niue";
  countryMap["NF\0"] = "NorfolkIsland";
  countryMap["MP\0"] = "NorthernMarianaIslands";
  countryMap["NO\0"] = "Norway";
  countryMap["OM\0"] = "Oman";
  countryMap["PK\0"] = "Pakistan";
  countryMap["PW\0"] = "Palau";
  countryMap["PS\0"] = "PalestinianTerritory";
  countryMap["PA\0"] = "Panama";
  countryMap["PG\0"] = "PapuaNewGuinea";
  countryMap["PY\0"] = "Paraguay";
  countryMap["PE\0"] = "Peru";
  countryMap["PH\0"] = "Philippines";
  countryMap["PN\0"] = "Pitcairn";
  countryMap["PL\0"] = "Poland";
  countryMap["PT\0"] = "Portugal";
  countryMap["PR\0"] = "PuertoRico";
  countryMap["QA\0"] = "Qatar";
  countryMap["RE\0"] = "Reunion";
  countryMap["RO\0"] = "Romania";
  countryMap["RU\0"] = "RussianFederation";
  countryMap["RW\0"] = "Rwanda";
  countryMap["KN\0"] = "SaintKittsAndNevis";
  countryMap["LC\0"] = "StLucia";
  countryMap["VC\0"] = "StVincentAndTheGrenadines";
  countryMap["WS\0"] = "Samoa";
  countryMap["SM\0"] = "SanMarino";
  countryMap["ST\0"] = "SaoTomeAndPrincipe";
  countryMap["SA\0"] = "SaudiArabia";
  countryMap["SN\0"] = "Senegal";
  countryMap["SC\0"] = "Seychelles";
  countryMap["SL\0"] = "SierraLeone";
  countryMap["SG\0"] = "Singapore";
  countryMap["SK\0"] = "Slovakia";
  countryMap["SI\0"] = "Slovenia";
  countryMap["SB\0"] = "SolomonIslands";
  countryMap["SO\0"] = "Somalia";
  countryMap["ZA\0"] = "SouthAfrica";
  countryMap["GS\0"] = "SouthGeorgiaAndTheSouthSandwichIslands";
  countryMap["ES\0"] = "Spain";
  countryMap["LK\0"] = "SriLanka";
  countryMap["SH\0"] = "StHelena";
  countryMap["PM\0"] = "StPierreAndMiquelon";
  countryMap["SD\0"] = "Sudan";
  countryMap["SR\0"] = "Suriname";
  countryMap["SJ\0"] = "SvalbardAndJanMayenIslands";
  countryMap["SZ\0"] = "Swaziland";
  countryMap["SE\0"] = "Sweden";
  countryMap["CH\0"] = "Switzerland";
  countryMap["SY\0"] = "SyrianArabRepublic";
  countryMap["TW\0"] = "Taiwan";
  countryMap["TJ\0"] = "Tajikistan";
  countryMap["TZ\0"] = "Tanzania";
  countryMap["TH\0"] = "Thailand";
  countryMap["TG\0"] = "Togo";
  countryMap["TK\0"] = "Tokelau";
  countryMap["TO\0"] = "Tonga";
  countryMap["TT\0"] = "TrinidadAndTobago";
  countryMap["TN\0"] = "Tunisia";
  countryMap["TR\0"] = "Turkey";
  countryMap["TM\0"] = "Turkmenistan";
  countryMap["TC\0"] = "TurksAndCaicosIslands";
  countryMap["TV\0"] = "Tuvalu";
  countryMap["UG\0"] = "Uganda";
  countryMap["UA\0"] = "Ukraine";
  countryMap["AE\0"] = "UnitedArabEmirates";
  countryMap["GB\0"] = "UnitedKingdom";
  countryMap["US\0"] = "UnitedStates";
  countryMap["UM\0"] = "UnitedStatesMinorOutlyingIslands";
  countryMap["UY\0"] = "Uruguay";
  countryMap["UZ\0"] = "Uzbekistan";
  countryMap["VU\0"] = "Vanuatu";
  countryMap["VA\0"] = "VaticanCityState";
  countryMap["VE\0"] = "Venezuela";
  countryMap["VN\0"] = "VietNam";
  countryMap["VG\0"] = "BritishVirginIslands";
  countryMap["VI\0"] = "USVirginIslands";
  countryMap["WF\0"] = "WallisAndFutunaIslands";
  countryMap["EH\0"] = "WesternSahara";
  countryMap["YE\0"] = "Yemen";
  countryMap["YU\0"] = "Yugoslavia";
  countryMap["ZM\0"] = "Zambia";
  countryMap["ZW\0"] = "Zimbabwe";
  countryMap["CS\0"] = "SerbiaAndMontenegro";
  countryMap["ME\0"] = "Montenegro";
  countryMap["RS\0"] = "Serbia";
  countryMap["BL\0"] = "SaintBarthelemy";
  countryMap["MF\0"] = "SaintMartin";
  countryMap["419"] = "LatinAmericaAndTheCaribbean";

  ui->localeCombo->addItem("No translate", "C");

  QStringList langs = translator->languages();
  qDebug() << langs;

  int currentIndex = 0;
  int size = langs.size();
  for(int i = 0; i < size; i++)
  {
    QString txt = langs[i];
    int p = txt.indexOf('_');
    QString lang = txt.left(p);
    QString country = txt.mid(p + 1);

    qDebug() << lang << country;
    QString descr = QString("%1 - %2").arg(tr(langMap[lang].toUtf8().constData())).arg(tr(countryMap[country].toUtf8().constData()));

    ui->localeCombo->addItem(descr, txt);
    if(settings.localeName == txt || settings.localeName == lang)
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
  QString locale = ui->localeCombo->model()->data( ui->localeCombo->model()->index(langIndex, 0), Qt::UserRole).toString();

  settings.localeName = locale;
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
    if(!translator->setLang(settings.localeName))
    {
      QString error = QString("Error sel language: %1").arg(translator->errorString());
      QMessageBox::critical(NULL, "Error translation", error);
      return;
    }
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
