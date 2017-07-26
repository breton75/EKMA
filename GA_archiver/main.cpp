#include <QCoreApplication>
#include <QCommandLineParser>
#include "stdio.h"
#include <QTextCodec>

#include "../general/sv_gathread.h"

SvGAArchiverParams params;

void setParams(QCommandLineParser *parser);


int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN32
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
#endif
 
#ifdef Q_OS_LINUX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif
    
  QTextStream qtin(stdin);
  QTextStream qtout(stdout);
  
  
  QCoreApplication a(argc, argv);
  
  QCommandLineParser parser;
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
  parser.setApplicationDescription("\nАрхивация и ретрансляция гидроакустических данных по протоколу UDP.");
  
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();
  
  QList<QCommandLineOption> clo;
  
  
  QCommandLineOption ipOption = QCommandLineOption (QStringList() << "ip" << "source_ip",
                                 QCoreApplication::translate("main", "IP адрес источника данных.\n"),
                                 "127.0.0.1",
                                 "127.0.0.1");
  clo << ipOption;

  
  clo << QCommandLineOption (QStringList() << "port" << "source_port",
                             QCoreApplication::translate("main", "Порт для приема данных. По умолчанию 8000.\n"),
                             "8000",
                             "8000");

  clo << QCommandLineOption (QStringList() << "path" << "path_to_archive",
                             QCoreApplication::translate("main", "Путь к каталогу архива. Допустимо использование выражений {DEVICE}, {DATETIME}, {DATE}, {TIME}.\n"),
                             "./archive",
                             "./archive");
  
  clo << QCommandLineOption (QStringList() << "dev" << "device_name",
                             QCoreApplication::translate("main", "Имя устройства. По умолчанию 'rls'.\n"),
                             "rls",
                             "rls");
  
  clo << QCommandLineOption (QStringList() << "fd" << "file_duration",
                             QCoreApplication::translate("main", "Длительность одного файла архивных данных в формате 'HHMMSS'. По умолчанию 1 час.\n"),
                             "010000",
                             "010000");
   
  clo << QCommandLineOption (QStringList() << "td" << "total_duration",
                             QCoreApplication::translate("main", "Общая длительность записи архивных данных в формате 'HHMMSS'. По умолчанию длительность не ограничена.'\n"),
                             "000000",
                             "000000");
  
  clo << QCommandLineOption (QStringList() << "dtfmt" << "date_time_format",
                             QCoreApplication::translate("main", "Формат даты времени для подстановки в имя файла. По умолчанию 'ddMMyy_hhmmss.'\n"),
                             "ddMMyy_hhmmss",
                             "ddMMyy_hhmmss");

  clo << QCommandLineOption (QStringList() << "fntmp" << "file_name_template",
                             QCoreApplication::translate("main", "Шаблон имени файла. Допустимо использование выражений {DEVICE}, {DATETIME}, {DATE}, {TIME}. По умолчанию '{DEVICE}_{DATETIME}.'\n"),
                             "{DEVICE}_{DATETIME}",
                             "{DEVICE}_{DATETIME}");

  QCommandLineOption outipOption = QCommandLineOption ("out_ip",
                             QCoreApplication::translate("main", "IP адрес для ретрансляции данных. По умолчанию 127.0.0.1.\n"),
                             "127.0.0.1",
                             "127.0.0.1"); 
  clo << outipOption;
  
  clo << QCommandLineOption ("out_port",
                             QCoreApplication::translate("main", "Порт для ретрансляции данных. По умолчанию 8001.\n"),
                             "8001",
                             "8001");  

  clo << QCommandLineOption ("start", QCoreApplication::translate("main", "Запуск архивации при запуске программы.\n"));
  
  
  for(QCommandLineOption o: clo)
    parser.addOption(o);
  
  parser.addPositionalArgument("ip", "ip адрес источника данных.", "-ip <source ip>");
  parser.addPositionalArgument("out_ip", "ip адрес для ретрансляции данных.", "-out_ip <out ip>");

  
  if (!parser.parse(a.arguments())) {
      qtout << parser.errorText() << endl << parser.helpText() << endl;
      return -1;
  }

  if (parser.isSet(versionOption)) {
    qtout << QCoreApplication::applicationVersion() << endl;
    return 0;
}

  if (parser.isSet(helpOption)) {
    qtout << parser.helpText() << endl;
    return 0;
}

  if (QHostAddress(parser.value(ipOption)).isNull()) {
    qtout << QString("Неверный ip адрес источника данных: %1").arg(parser.value(ipOption)).toUtf8() << endl;
    return -1;
  }
  
  if (QHostAddress(parser.value(outipOption)).isNull()) {
    qtout << QString("Неверный ip адрес для ретрансляции данных: %1").arg(parser.value(ipOption)).toUtf8() << endl;
    return -1;
  }

  setParams(&parser);
  
  SvGAArchiver *ga_archiver = nullptr;
  
  if(parser.isSet("autostart")) {
    ga_archiver = new SvGAArchiver(&params, 0);
    ga_archiver->start();
  }
  
  qtout << QString("IP источника данных:\t").toUtf8() << params.ip << endl
        << QString("Порт источника данных:\t").toUtf8() << params.port << endl
        << QString("IP приемника данных:\t").toUtf8() << params.out_ip << endl
        << QString("Порт приемника данных:\t").toUtf8() << params.out_port << endl
        << QString("Каталог архивации:\t").toUtf8() << params.path << endl
        << QString("Имя устройства:\t\t").toUtf8() << params.device_name  << endl
        << QString("Длительность файла:\t").toUtf8() << params.file_duration.toString("hh:mm:ss")  << endl
        << QString("Общая длительность:\t").toUtf8() << params.total_duration.toString("hh:mm:ss")  << endl
        << QString("Формат даты\\времени:\t").toUtf8() << params.date_time_format  << endl   
        << QString("Шаблон имени файла:\t").toUtf8() << params.file_name_template  << endl << endl;

  bool wait_cmd = true;
  QString cmd;
  while(wait_cmd) {

    qtin >> cmd;
    
    if(cmd == "stop") {
      ga_archiver->stop();
      while(!ga_archiver->isFinished()) QCoreApplication::processEvents();
      ga_archiver = nullptr;
      
      qtout << "Archiving stoppped" << endl;
               
    }
    else if (cmd == "start" && !ga_archiver) {
      if(!ga_archiver) {
        
        qtout << "Starting archiving" << endl;
        
        ga_archiver = new SvGAArchiver(&params, 0);
        ga_archiver->start();
      }
      
    }
    else if(cmd == "quit") {
      wait_cmd = false;
      if(ga_archiver) {
        ga_archiver->stop();
        while(!ga_archiver->isFinished()) QCoreApplication::processEvents();
      }
    }
  }
    
  return a.exec();
  qtout << "Closing" << endl;
  if(ga_archiver) {
    ga_archiver->stop();
    while(!ga_archiver->isFinished()) QCoreApplication::processEvents();
  }
  
}

void setParams(QCommandLineParser *parser)
{
  if(parser->isSet("ip")) params.ip = parser->value("ip");
  if(parser->isSet("port")) params.port = parser->value("port").toUInt();
  if(parser->isSet("device_name")) params.device_name = parser->value("device_name");
  if(parser->isSet("path")) params.path = parser->value("path");
  if(parser->isSet("total_duration")) params.total_duration = QTime::fromString(parser->value("total_duration"), "hhmmss");
  if(parser->isSet("file_duration")) params.file_duration = QTime::fromString(parser->value("file_duration"), "hhmmss");
  if(parser->isSet("date_time_format")) params.date_time_format = parser->value("date_time_format");
  if(parser->isSet("file_name_template")) params.file_name_template = parser->value("file_name_template");
  if(parser->isSet("out_ip")) params.out_ip = parser->value("out_ip");
  if(parser->isSet("out_port")) params.out_port = parser->value("out_port").toUInt();
//  params. = parser->value("");

   
}
