/*----------------------------------------------------------------------------*/
/**
* @pkg message_file
*/
/**
* Short description of message_file.
*
* Long description of message_file.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 08.11.2012 15:10:26<br>
* @pkgdoc message_file
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "message_file.h"
#include <QDomDocument>
#include <QFile>
#include <QStringList>
/*----------------------------------------------------------------------------*/
MessageFile :: MessageFile()
{
  langHash["\0"] = "Unused";
  langHash["\0"] = "C";
  langHash["ab\0"] = "Abkhazian";
  langHash["om\0"] = "Afan";
  langHash["aa\0"] = "Afar";
  langHash["af\0"] = "Afrikaans";
  langHash["sq\0"] = "Albanian";
  langHash["am\0"] = "Amharic";
  langHash["ar\0"] = "Arabic";
  langHash["hy\0"] = "Armenian";
  langHash["as\0"] = "Assamese";
  langHash["ay\0"] = "Aymara";
  langHash["az\0"] = "Azerbaijani";
  langHash["ba\0"] = "Bashkir";
  langHash["eu\0"] = "Basque";
  langHash["bn\0"] = "Bengali";
  langHash["dz\0"] = "Bhutani";
  langHash["bh\0"] = "Bihari";
  langHash["bi\0"] = "Bislama";
  langHash["br\0"] = "Breton";
  langHash["bg\0"] = "Bulgarian";
  langHash["my\0"] = "Burmese";
  langHash["be\0"] = "Byelorussian";
  langHash["km\0"] = "Cambodian";
  langHash["ca\0"] = "Catalan";
  langHash["zh\0"] = "Chinese";
  langHash["co\0"] = "Corsican";
  langHash["hr\0"] = "Croatian";
  langHash["cs\0"] = "Czech";
  langHash["da\0"] = "Danish";
  langHash["nl\0"] = "Dutch";
  langHash["en\0"] = "English";
  langHash["eo\0"] = "Esperanto";
  langHash["et\0"] = "Estonian";
  langHash["fo\0"] = "Faroese";
  langHash["fj\0"] = "Fiji";
  langHash["fi\0"] = "Finnish";
  langHash["fr\0"] = "French";
  langHash["fy\0"] = "Frisian";
  langHash["gd\0"] = "Gaelic";
  langHash["gl\0"] = "Galician";
  langHash["ka\0"] = "Georgian";
  langHash["de\0"] = "German";
  langHash["el\0"] = "Greek";
  langHash["kl\0"] = "Greenlandic";
  langHash["gn\0"] = "Guarani";
  langHash["gu\0"] = "Gujarati";
  langHash["ha\0"] = "Hausa";
  langHash["he\0"] = "Hebrew";
  langHash["hi\0"] = "Hindi";
  langHash["hu\0"] = "Hungarian";
  langHash["is\0"] = "Icelandic";
  langHash["id\0"] = "Indonesian";
  langHash["ia\0"] = "Interlingua";
  langHash["ie\0"] = "Interlingue";
  langHash["iu\0"] = "Inuktitut";
  langHash["ik\0"] = "Inupiak";
  langHash["ga\0"] = "Irish";
  langHash["it\0"] = "Italian";
  langHash["ja\0"] = "Japanese";
  langHash["jv\0"] = "Javanese";
  langHash["kn\0"] = "Kannada";
  langHash["ks\0"] = "Kashmiri";
  langHash["kk\0"] = "Kazakh";
  langHash["rw\0"] = "Kinyarwanda";
  langHash["ky\0"] = "Kirghiz";
  langHash["ko\0"] = "Korean";
  langHash["ku\0"] = "Kurdish";
  langHash["rn\0"] = "Kurundi";
  langHash["lo\0"] = "Laothian";
  langHash["la\0"] = "Latin";
  langHash["lv\0"] = "Latvian";
  langHash["ln\0"] = "Lingala";
  langHash["lt\0"] = "Lithuanian";
  langHash["mk\0"] = "Macedonian";
  langHash["mg\0"] = "Malagasy";
  langHash["ms\0"] = "Malay";
  langHash["ml\0"] = "Malayalam";
  langHash["mt\0"] = "Maltese";
  langHash["mi\0"] = "Maori";
  langHash["mr\0"] = "Marathi";
  langHash["mo\0"] = "Moldavian";
  langHash["mn\0"] = "Mongolian";
  langHash["na\0"] = "Nauru";
  langHash["ne\0"] = "Nepali";
  langHash["nb\0"] = "Norwegian";
  langHash["oc\0"] = "Occitan";
  langHash["or\0"] = "Oriya";
  langHash["ps\0"] = "Pashto";
  langHash["fa\0"] = "Persian";
  langHash["pl\0"] = "Polish";
  langHash["pt\0"] = "Portuguese";
  langHash["pa\0"] = "Punjabi";
  langHash["qu\0"] = "Quechua";
  langHash["rm\0"] = "RhaetoRomance";
  langHash["ro\0"] = "Romanian";
  langHash["ru\0"] = "Russian";
  langHash["sm\0"] = "Samoan";
  langHash["sg\0"] = "Sangho";
  langHash["sa\0"] = "Sanskrit";
  langHash["sr\0"] = "Serbian";
  langHash["sh\0"] = "SerboCroatian";
  langHash["st\0"] = "Sesotho";
  langHash["tn\0"] = "Setswana";
  langHash["sn\0"] = "Shona";
  langHash["sd\0"] = "Sindhi";
  langHash["si\0"] = "Singhalese";
  langHash["ss\0"] = "Siswati";
  langHash["sk\0"] = "Slovak";
  langHash["sl\0"] = "Slovenian";
  langHash["so\0"] = "Somali";
  langHash["es\0"] = "Spanish";
  langHash["su\0"] = "Sundanese";
  langHash["sw\0"] = "Swahili";
  langHash["sv\0"] = "Swedish";
  langHash["tl\0"] = "Tagalog";
  langHash["tg\0"] = "Tajik";
  langHash["ta\0"] = "Tamil";
  langHash["tt\0"] = "Tatar";
  langHash["te\0"] = "Telugu";
  langHash["th\0"] = "Thai";
  langHash["bo\0"] = "Tibetan";
  langHash["ti\0"] = "Tigrinya";
  langHash["to\0"] = "Tonga";
  langHash["ts\0"] = "Tsonga";
  langHash["tr\0"] = "Turkish";
  langHash["tk\0"] = "Turkmen";
  langHash["tw\0"] = "Twi";
  langHash["ug\0"] = "Uigur";
  langHash["uk\0"] = "Ukrainian";
  langHash["ur\0"] = "Urdu";
  langHash["uz\0"] = "Uzbek";
  langHash["vi\0"] = "Vietnamese";
  langHash["vo\0"] = "Volapuk";
  langHash["cy\0"] = "Welsh";
  langHash["wo\0"] = "Wolof";
  langHash["xh\0"] = "Xhosa";
  langHash["yi\0"] = "Yiddish";
  langHash["yo\0"] = "Yoruba";
  langHash["za\0"] = "Zhuang";
  langHash["zu\0"] = "Zulu";
  langHash["nn\0"] = "Nynorsk";
  langHash["bs\0"] = "Bosnian";
  langHash["dv\0"] = "Divehi";
  langHash["gv\0"] = "Manx";
  langHash["kw\0"] = "Cornish";
  langHash["ak\0"] = "Akan";
  langHash["kok"] = "Konkani";
  langHash["gaa"] = "Ga";
  langHash["ig\0"] = "Igbo";
  langHash["kam"] = "Kamba";
  langHash["syr"] = "Syriac";
  langHash["byn"] = "Blin";
  langHash["gez"] = "Geez";
  langHash["kfo"] = "Koro";
  langHash["sid"] = "Sidamo";
  langHash["cch"] = "Atsam";
  langHash["tig"] = "Tigre";
  langHash["kaj"] = "Jju";
  langHash["fur"] = "Friulian";
  langHash["ve\0"] = "Venda";
  langHash["ee\0"] = "Ewe";
  langHash["wal"] = "Walamo";
  langHash["haw"] = "Hawaiian";
  langHash["kcg"] = "Tyap";
  langHash["ny\0"] = "Chewa";
  langHash["fil"] = "Filipino";
  langHash["gsw"] = "SwissGerman";
  langHash["ii\0"] = "SichuanYi";
  langHash["kpe"] = "Kpelle";
  langHash["nds"] = "LowGerman";
  langHash["nr\0"] = "SouthNdebele";
  langHash["nso"] = "NorthernSotho";
  langHash["se\0"] = "NorthernSami";
  langHash["trv"] = "Taroko";
  langHash["guz"] = "Gusii";
  langHash["dav"] = "Taita";
  langHash["ff\0"] = "Fulah";
  langHash["ki\0"] = "Kikuyu";
  langHash["saq"] = "Samburu";
  langHash["seh"] = "Sena";
  langHash["nd\0"] = "NorthNdebele";
  langHash["rof"] = "Rombo";
  langHash["shi"] = "Tachelhit";
  langHash["kab"] = "Kabyle";
  langHash["nyn"] = "Nyankole";
  langHash["bez"] = "Bena";
  langHash["vun"] = "Vunjo";
  langHash["bm\0"] = "Bambara";
  langHash["ebu"] = "Embu";
  langHash["chr"] = "Cherokee";
  langHash["mfe"] = "Morisyen";
  langHash["kde"] = "Makonde";
  langHash["lag"] = "Langi";
  langHash["lg\0"] = "Ganda";
  langHash["bem"] = "Bemba";
  langHash["kea"] = "Kabuverdianu";
  langHash["mer"] = "Meru";
  langHash["kln"] = "Kalenjin";
  langHash["naq"] = "Nama";
  langHash["jmc"] = "Machame";
  langHash["ksh"] = "Colognian";
  langHash["mas"] = "Masai";
  langHash["xog"] = "Soga";
  langHash["luy"] = "Luyia";
  langHash["asa"] = "Asu";
  langHash["teo"] = "Teso";
  langHash["ssy"] = "Saho";
  langHash["khq"] = "KoyraChiini";
  langHash["rwk"] = "Rwa";
  langHash["luo"] = "Luo";
  langHash["cgg"] = "Chiga";
  langHash["tzm"] = "CentralMoroccoTamazight";
  langHash["ses"] = "KoyraboroSenni";
  langHash["ksb"] = "Shambala";

  countryHash["AF\0"] = "Afghanistan";
  countryHash["AL\0"] = "Albania";
  countryHash["DZ\0"] = "Algeria";
  countryHash["AS\0"] = "AmericanSamoa";
  countryHash["AD\0"] = "Andorra";
  countryHash["AO\0"] = "Angola";
  countryHash["AI\0"] = "Anguilla";
  countryHash["AQ\0"] = "Antarctica";
  countryHash["AG\0"] = "AntiguaAndBarbuda";
  countryHash["AR\0"] = "Argentina";
  countryHash["AM\0"] = "Armenia";
  countryHash["AW\0"] = "Aruba";
  countryHash["AU\0"] = "Australia";
  countryHash["AT\0"] = "Austria";
  countryHash["AZ\0"] = "Azerbaijan";
  countryHash["BS\0"] = "Bahamas";
  countryHash["BH\0"] = "Bahrain";
  countryHash["BD\0"] = "Bangladesh";
  countryHash["BB\0"] = "Barbados";
  countryHash["BY\0"] = "Belarus";
  countryHash["BE\0"] = "Belgium";
  countryHash["BZ\0"] = "Belize";
  countryHash["BJ\0"] = "Benin";
  countryHash["BM\0"] = "Bermuda";
  countryHash["BT\0"] = "Bhutan";
  countryHash["BO\0"] = "Bolivia";
  countryHash["BA\0"] = "BosniaAndHerzegowina";
  countryHash["BW\0"] = "Botswana";
  countryHash["BV\0"] = "BouvetIsland";
  countryHash["BR\0"] = "Brazil";
  countryHash["IO\0"] = "BritishIndianOceanTerritory";
  countryHash["BN\0"] = "BruneiDarussalam";
  countryHash["BG\0"] = "Bulgaria";
  countryHash["BF\0"] = "BurkinaFaso";
  countryHash["BI\0"] = "Burundi";
  countryHash["KH\0"] = "Cambodia";
  countryHash["CM\0"] = "Cameroon";
  countryHash["CA\0"] = "Canada";
  countryHash["CV\0"] = "CapeVerde";
  countryHash["KY\0"] = "CaymanIslands";
  countryHash["CF\0"] = "CentralAfricanRepublic";
  countryHash["TD\0"] = "Chad";
  countryHash["CL\0"] = "Chile";
  countryHash["CN\0"] = "China";
  countryHash["CX\0"] = "ChristmasIsland";
  countryHash["CC\0"] = "CocosIslands";
  countryHash["CO\0"] = "Colombia";
  countryHash["KM\0"] = "Comoros";
  countryHash["CD\0"] = "DemocraticRepublicOfCongo";
  countryHash["CG\0"] = "PeoplesRepublicOfCongo";
  countryHash["CK\0"] = "CookIslands";
  countryHash["CR\0"] = "CostaRica";
  countryHash["CI\0"] = "IvoryCoast";
  countryHash["HR\0"] = "Croatia";
  countryHash["CU\0"] = "Cuba";
  countryHash["CY\0"] = "Cyprus";
  countryHash["CZ\0"] = "CzechRepublic";
  countryHash["DK\0"] = "Denmark";
  countryHash["DJ\0"] = "Djibouti";
  countryHash["DM\0"] = "Dominica";
  countryHash["DO\0"] = "DominicanRepublic";
  countryHash["TL\0"] = "EastTimor";
  countryHash["EC\0"] = "Ecuador";
  countryHash["EG\0"] = "Egypt";
  countryHash["SV\0"] = "ElSalvador";
  countryHash["GQ\0"] = "EquatorialGuinea";
  countryHash["ER\0"] = "Eritrea";
  countryHash["EE\0"] = "Estonia";
  countryHash["ET\0"] = "Ethiopia";
  countryHash["FK\0"] = "FalklandIslands";
  countryHash["FO\0"] = "FaroeIslands";
  countryHash["FJ\0"] = "Fiji";
  countryHash["FI\0"] = "Finland";
  countryHash["FR\0"] = "France";
  countryHash["FX\0"] = "MetropolitanFrance";
  countryHash["GF\0"] = "FrenchGuiana";
  countryHash["PF\0"] = "FrenchPolynesia";
  countryHash["TF\0"] = "FrenchSouthernTerritories";
  countryHash["GA\0"] = "Gabon";
  countryHash["GM\0"] = "Gambia";
  countryHash["GE\0"] = "Georgia";
  countryHash["DE\0"] = "Germany";
  countryHash["GH\0"] = "Ghana";
  countryHash["GI\0"] = "Gibraltar";
  countryHash["GR\0"] = "Greece";
  countryHash["GL\0"] = "Greenland";
  countryHash["GD\0"] = "Grenada";
  countryHash["GP\0"] = "Guadeloupe";
  countryHash["GU\0"] = "Guam";
  countryHash["GT\0"] = "Guatemala";
  countryHash["GN\0"] = "Guinea";
  countryHash["GW\0"] = "GuineaBissau";
  countryHash["GY\0"] = "Guyana";
  countryHash["HT\0"] = "Haiti";
  countryHash["HM\0"] = "HeardAndMcDonaldIslands";
  countryHash["HN\0"] = "Honduras";
  countryHash["HK\0"] = "HongKong";
  countryHash["HU\0"] = "Hungary";
  countryHash["IS\0"] = "Iceland";
  countryHash["IN\0"] = "India";
  countryHash["ID\0"] = "Indonesia";
  countryHash["IR\0"] = "Iran";
  countryHash["IQ\0"] = "Iraq";
  countryHash["IE\0"] = "Ireland";
  countryHash["IL\0"] = "Israel";
  countryHash["IT\0"] = "Italy";
  countryHash["JM\0"] = "Jamaica";
  countryHash["JP\0"] = "Japan";
  countryHash["JO\0"] = "Jordan";
  countryHash["KZ\0"] = "Kazakhstan";
  countryHash["KE\0"] = "Kenya";
  countryHash["KI\0"] = "Kiribati";
  countryHash["KP\0"] = "DemocraticRepublicOfKorea";
  countryHash["KR\0"] = "RepublicOfKorea";
  countryHash["KW\0"] = "Kuwait";
  countryHash["KG\0"] = "Kyrgyzstan";
  countryHash["LA\0"] = "Lao";
  countryHash["LV\0"] = "Latvia";
  countryHash["LB\0"] = "Lebanon";
  countryHash["LS\0"] = "Lesotho";
  countryHash["LR\0"] = "Liberia";
  countryHash["LY\0"] = "LibyanArabJamahiriya";
  countryHash["LI\0"] = "Liechtenstein";
  countryHash["LT\0"] = "Lithuania";
  countryHash["LU\0"] = "Luxembourg";
  countryHash["MO\0"] = "Macau";
  countryHash["MK\0"] = "Macedonia";
  countryHash["MG\0"] = "Madagascar";
  countryHash["MW\0"] = "Malawi";
  countryHash["MY\0"] = "Malaysia";
  countryHash["MV\0"] = "Maldives";
  countryHash["ML\0"] = "Mali";
  countryHash["MT\0"] = "Malta";
  countryHash["MH\0"] = "MarshallIslands";
  countryHash["MQ\0"] = "Martinique";
  countryHash["MR\0"] = "Mauritania";
  countryHash["MU\0"] = "Mauritius";
  countryHash["YT\0"] = "Mayotte";
  countryHash["MX\0"] = "Mexico";
  countryHash["FM\0"] = "Micronesia";
  countryHash["MD\0"] = "Moldova";
  countryHash["MC\0"] = "Monaco";
  countryHash["MN\0"] = "Mongolia";
  countryHash["MS\0"] = "Montserrat";
  countryHash["MA\0"] = "Morocco";
  countryHash["MZ\0"] = "Mozambique";
  countryHash["MM\0"] = "Myanmar";
  countryHash["NA\0"] = "Namibia";
  countryHash["NR\0"] = "Nauru";
  countryHash["NP\0"] = "Nepal";
  countryHash["NL\0"] = "Netherlands";
  countryHash["AN\0"] = "NetherlandsAntilles";
  countryHash["NC\0"] = "NewCaledonia";
  countryHash["NZ\0"] = "NewZealand";
  countryHash["NI\0"] = "Nicaragua";
  countryHash["NE\0"] = "Niger";
  countryHash["NG\0"] = "Nigeria";
  countryHash["NU\0"] = "Niue";
  countryHash["NF\0"] = "NorfolkIsland";
  countryHash["MP\0"] = "NorthernMarianaIslands";
  countryHash["NO\0"] = "Norway";
  countryHash["OM\0"] = "Oman";
  countryHash["PK\0"] = "Pakistan";
  countryHash["PW\0"] = "Palau";
  countryHash["PS\0"] = "PalestinianTerritory";
  countryHash["PA\0"] = "Panama";
  countryHash["PG\0"] = "PapuaNewGuinea";
  countryHash["PY\0"] = "Paraguay";
  countryHash["PE\0"] = "Peru";
  countryHash["PH\0"] = "Philippines";
  countryHash["PN\0"] = "Pitcairn";
  countryHash["PL\0"] = "Poland";
  countryHash["PT\0"] = "Portugal";
  countryHash["PR\0"] = "PuertoRico";
  countryHash["QA\0"] = "Qatar";
  countryHash["RE\0"] = "Reunion";
  countryHash["RO\0"] = "Romania";
  countryHash["RU\0"] = "RussianFederation";
  countryHash["RW\0"] = "Rwanda";
  countryHash["KN\0"] = "SaintKittsAndNevis";
  countryHash["LC\0"] = "StLucia";
  countryHash["VC\0"] = "StVincentAndTheGrenadines";
  countryHash["WS\0"] = "Samoa";
  countryHash["SM\0"] = "SanMarino";
  countryHash["ST\0"] = "SaoTomeAndPrincipe";
  countryHash["SA\0"] = "SaudiArabia";
  countryHash["SN\0"] = "Senegal";
  countryHash["SC\0"] = "Seychelles";
  countryHash["SL\0"] = "SierraLeone";
  countryHash["SG\0"] = "Singapore";
  countryHash["SK\0"] = "Slovakia";
  countryHash["SI\0"] = "Slovenia";
  countryHash["SB\0"] = "SolomonIslands";
  countryHash["SO\0"] = "Somalia";
  countryHash["ZA\0"] = "SouthAfrica";
  countryHash["GS\0"] = "SouthGeorgiaAndTheSouthSandwichIslands";
  countryHash["ES\0"] = "Spain";
  countryHash["LK\0"] = "SriLanka";
  countryHash["SH\0"] = "StHelena";
  countryHash["PM\0"] = "StPierreAndMiquelon";
  countryHash["SD\0"] = "Sudan";
  countryHash["SR\0"] = "Suriname";
  countryHash["SJ\0"] = "SvalbardAndJanMayenIslands";
  countryHash["SZ\0"] = "Swaziland";
  countryHash["SE\0"] = "Sweden";
  countryHash["CH\0"] = "Switzerland";
  countryHash["SY\0"] = "SyrianArabRepublic";
  countryHash["TW\0"] = "Taiwan";
  countryHash["TJ\0"] = "Tajikistan";
  countryHash["TZ\0"] = "Tanzania";
  countryHash["TH\0"] = "Thailand";
  countryHash["TG\0"] = "Togo";
  countryHash["TK\0"] = "Tokelau";
  countryHash["TO\0"] = "Tonga";
  countryHash["TT\0"] = "TrinidadAndTobago";
  countryHash["TN\0"] = "Tunisia";
  countryHash["TR\0"] = "Turkey";
  countryHash["TM\0"] = "Turkmenistan";
  countryHash["TC\0"] = "TurksAndCaicosIslands";
  countryHash["TV\0"] = "Tuvalu";
  countryHash["UG\0"] = "Uganda";
  countryHash["UA\0"] = "Ukraine";
  countryHash["AE\0"] = "UnitedArabEmirates";
  countryHash["GB\0"] = "UnitedKingdom";
  countryHash["US\0"] = "UnitedStates";
  countryHash["UM\0"] = "UnitedStatesMinorOutlyingIslands";
  countryHash["UY\0"] = "Uruguay";
  countryHash["UZ\0"] = "Uzbekistan";
  countryHash["VU\0"] = "Vanuatu";
  countryHash["VA\0"] = "VaticanCityState";
  countryHash["VE\0"] = "Venezuela";
  countryHash["VN\0"] = "VietNam";
  countryHash["VG\0"] = "BritishVirginIslands";
  countryHash["VI\0"] = "USVirginIslands";
  countryHash["WF\0"] = "WallisAndFutunaIslands";
  countryHash["EH\0"] = "WesternSahara";
  countryHash["YE\0"] = "Yemen";
  countryHash["YU\0"] = "Yugoslavia";
  countryHash["ZM\0"] = "Zambia";
  countryHash["ZW\0"] = "Zimbabwe";
  countryHash["CS\0"] = "SerbiaAndMontenegro";
  countryHash["ME\0"] = "Montenegro";
  countryHash["RS\0"] = "Serbia";
  countryHash["BL\0"] = "SaintBarthelemy";
  countryHash["MF\0"] = "SaintMartin";
  countryHash["419"] = "LatinAmericaAndTheCaribbean";

  m_modified = false;
}
/*----------------------------------------------------------------------------*/
void MessageFile :: addLang(const QString& locale)
{
  if(!m_langList.contains(locale))
  {
    m_langList.append(locale);
    int p = locale.indexOf('_');
    QString lang = langHash[locale.left(p)];
    QString country = countryHash[locale.mid(p + 1)];
    addMessage(lang, lang, locale);
    addMessage(country, country, locale);
  }
}
/*----------------------------------------------------------------------------*/
bool MessageFile :: load(const QString& fileName)
{
  QFile confFile(fileName);

  if(!confFile.open(QIODevice::ReadOnly))
  {
    m_errorString = confFile.errorString();
    return false;
  }

  QDomDocument doc;

  QString errMsg;
  int errorLine, errorColumn;
  if(!doc.setContent(&confFile, &errMsg, &errorLine, &errorColumn))
  {
    m_errorString = QString("Error %1:%2:%3: %4").arg(confFile.fileName()).arg(errorLine).arg(errorColumn).arg(errMsg);
    return false;
  }

  QDomElement el = doc.firstChildElement("translator");
  for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    if(n.nodeName() == "message")
    {
      QString source;
      StringHash strings;
      for(QDomNode nn = n.firstChild(); !nn.isNull(); nn = nn.nextSibling())
      {
        if(nn.nodeName() == "source")
        {
          source = nn.toElement().text();
        }
        else
        {
          QString lang = nn.nodeName();
          QString txt = nn.toElement().text();
          strings[lang] = txt;
          addLang(lang);
        }
      }
      m_messageHash[source] = strings;
    }
  }

  m_modified = false;
  return true;
}
/*----------------------------------------------------------------------------*/
bool MessageFile :: save(const QString& fileName)
{
  QDomDocument doc;

  doc.appendChild(doc.createProcessingInstruction(
        "xml",
        "version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\""
        ));

  QDomElement root = doc.createElement("translator");
  doc.appendChild(root);

  QDomElement tag = doc.createElement("languages");

  int size = m_langList.size();
  for(int i = 0; i < size; i++)
  {
    QDomElement t = doc.createElement("language");
    t.appendChild(doc.createTextNode(m_langList[i]));
    tag.appendChild(t);
  }
  root.appendChild(tag);

  MessageHash::Iterator mend = m_messageHash.end();
  for(MessageHash::Iterator mi = m_messageHash.begin(); mi != mend; ++mi)
  {
    QString source = mi.key();
    QDomElement tag = doc.createElement("message");
    QDomElement t = doc.createElement("source");
    t.appendChild(doc.createTextNode(source));
    tag.appendChild(t);

    StringHash &strings = mi.value();
    StringHash::Iterator end = strings.end();
    for(StringHash::Iterator it = strings.begin(); it != end; ++it)
    {
      QDomElement t = doc.createElement(it.key());
      t.appendChild(doc.createTextNode(it.value()));
      tag.appendChild(t);
    }
    root.appendChild(tag);
  }

  QString xml = doc.toString();
  QFile confFile(fileName);

  if(!confFile.open(QIODevice::WriteOnly))
  {
    m_errorString = confFile.errorString();
    return false;
  }

  if(!confFile.write(xml.toUtf8()))
  {
    m_errorString = confFile.errorString();
    return false;
  }
  m_modified = false;
  return true;
}
/*----------------------------------------------------------------------------*/
void MessageFile :: setLang(const QString& lang)
{
  m_lang = lang;
  addLang(lang);
}
/*----------------------------------------------------------------------------*/
void MessageFile :: setMessage(const QString& source, const QString& msg, const QString& l)
{
  QString lang = l;
  if(l.isEmpty())
    lang = m_lang;
  addLang(lang);

  MessageHash::Iterator mi = m_messageHash.find(source);
  if(mi != m_messageHash.end())
  {
    StringHash &strings = mi.value();
    StringHash::Iterator it = strings.find(lang);
    if(it == strings.end())
    {
      strings.insert(lang, msg);
      m_modified = true;
    }
    else
    {
      m_modified = m_modified || it.value() != msg;
      it.value() = msg;
    }
  }
  else
  {
    StringHash strings;
    strings[lang] = msg;
    m_messageHash[source] = strings;
    m_modified = true;
  }
}
/*----------------------------------------------------------------------------*/
void MessageFile :: addMessage(const QString& source, const QString& msg, const QString& l)
{
  QString lang = l;
  if(l.isEmpty())
    lang = m_lang;
  addLang(lang);

  MessageHash::Iterator mi = m_messageHash.find(source);
  if(mi != m_messageHash.end())
  {
    StringHash &strings = mi.value();
    StringHash::Iterator it = strings.find(lang);
    if(it == strings.end())
    {
      strings.insert(lang, msg);
      m_modified = true;
    }
  }
  else
  {
    StringHash strings;
    strings[lang] = msg;
    m_messageHash[source] = strings;
    m_modified = true;
  }
}
/*----------------------------------------------------------------------------*/
QStringList MessageFile :: sourceList()
{
  QStringList lst;
  MessageHash::Iterator mi = m_messageHash.begin();
  while(mi != m_messageHash.end())
  {
    lst.append(mi.key());
    ++ mi;
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
QString MessageFile :: message(const QString& source, const QString& l)
{
  QString lang = l;
  if(l.isEmpty())
    lang = m_lang;

  MessageHash::Iterator mi = m_messageHash.find(source);
  if(mi != m_messageHash.end())
  {
    StringHash &strings = mi.value();
    StringHash::Iterator it = strings.find(lang);
    if(it != strings.end())
      return it.value();
  }
  return QString();
}
/*----------------------------------------------------------------------------*/
void MessageFile :: deleteSource(const QString& source)
{
  MessageHash::Iterator mi = m_messageHash.find(source);
  if(mi != m_messageHash.end())
  {
    m_messageHash.erase(mi);
    m_modified = true;
  }
}
/*----------------------------------------------------------------------------*/
void MessageFile :: addSource(const QString& source)
{
  MessageHash::Iterator mi = m_messageHash.find(source);
  if(mi == m_messageHash.end())
  {
    StringHash strings;
    m_messageHash[source] = strings;
    m_modified = true;
  }
}
/*----------------------------------------------------------------------------*/
void MessageFile :: deleteLang(const QString& lang)
{
  MessageHash::Iterator mi = m_messageHash.begin();
  while(mi != m_messageHash.end())
  {
    StringHash &strings = mi.value();
    StringHash::Iterator it = strings.find(lang);
    if(it != strings.end())
    {
      strings.erase(it);
      m_modified = true;
    }
    ++ mi;
  }
  int index = m_langList.indexOf(lang);
  if(index >= 0)
    m_langList.erase(m_langList.begin() + index);
}
/*----------------------------------------------------------------------------*/
QStringList MessageFile :: langsList()
{
  QStringList lst;
  StringHash::Iterator end = langHash.end();
  for(StringHash::Iterator it = langHash.begin(); it != end; ++it)
    lst.append(it.value());
  return lst;
}
/*----------------------------------------------------------------------------*/
QStringList MessageFile :: countryList()
{
  QStringList lst;
  StringHash::Iterator end = countryHash.end();
  for(StringHash::Iterator it = countryHash.begin(); it != end; ++it)
    lst.append(it.value());
  return lst;
}
/*----------------------------------------------------------------------------*/
QString MessageFile :: mkLocale(int langIndex, int countryIndex)
{
  QString lang, country;
  StringHash::Iterator end = langHash.end();
  int index = 0;
  for(StringHash::Iterator it = langHash.begin(); it != end; ++it, index ++)
    if(index == langIndex)
    {
      lang = it.key();
      break;
    }

  end = countryHash.end();
  index = 0;
  for(StringHash::Iterator it = countryHash.begin(); it != end; ++it, index ++)
    if(index == countryIndex)
    {
      country = it.key();
      break;
    }

  if(country.isEmpty())
    return lang;
  return QString("%1_%2").arg(lang).arg(country);
}
/*----------------------------------------------------------------------------*/
QString MessageFile :: locale(const QString& txt)
{
  int p = txt.indexOf('_');
  QString lang = txt.left(p);
  QString cntr = txt.mid(p + 1);

  if(cntr.isEmpty())
    return language(lang);
  if(lang.isEmpty())
    return country(cntr);
  return QString("%1 - %2").arg(language(lang)).arg(country(cntr));
}
/*----------------------------------------------------------------------------*/
QString MessageFile :: language(const QString& l)
{
  StringHash::Iterator it = langHash.find(l);
  if(it != langHash.end())
    return it.value();
  return "";
}
/*----------------------------------------------------------------------------*/
QString MessageFile :: country(const QString& c)
{
  StringHash::Iterator it = countryHash.find(c);
  if(it != countryHash.end())
    return it.value();
  return "";
}
/*----------------------------------------------------------------------------*/
