#include <QCoreApplication>
#include <QCommandLineParser>
#include "stdio.h"

#include "../general/sv_rls2bitthread.h"

SvRlsArchiverParams params;

void setParams(QCommandLineParser *parser);


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
  QCommandLineParser parser;
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
  parser.setApplicationDescription("Архивация и ретрансляция информации от РЛС");
  parser.addHelpOption();
  parser.addVersionOption();
  
  QList<QCommandLineOption> clo;
  
  clo << QCommandLineOption ("ip",
                             QCoreApplication::translate("main", "IP адрес для прослушивания данных."),
                             QCoreApplication::translate("main", "127.0.0.1"));
  
  clo << QCommandLineOption ("port",
                             QCoreApplication::translate("main", "Порт для приема данных по протоколу UDP."),
                             QCoreApplication::translate("main", "8000"));

  clo << QCommandLineOption (QStringList() << "path" << "path_to_save",
                             QCoreApplication::translate("main", "Каталог для сохранения файлов. Допустимо использование выражений {DEVICE}, {DATETIME}, {DATE}, {TIME}"),
                             QCoreApplication::translate("main", "archive"));
  
  clo << QCommandLineOption (QStringList() << "dev" << "device_name",
                             QCoreApplication::translate("main", "Имя устройства. По умолчанию 'rls'"),
                             QCoreApplication::translate("main", "rls"));
  
  clo << QCommandLineOption (QStringList() << "fd" << "file_duration",
                             QCoreApplication::translate("main", "Длительность одного файла архивных данных в формате 'HHMMSS'. По умолчанию 1 час."),
                             QCoreApplication::translate("main", "010000"));
   
  clo << QCommandLineOption (QStringList() << "td" << "total_duration",
                             QCoreApplication::translate("main", "Общая длительность записи архивных данных в формате 'HHMMSS'. По умолчанию длительность не ограничена."),
                             QCoreApplication::translate("main", "000000"));
  
  clo << QCommandLineOption (QStringList() << "dtfmt" << "date_time_format",
                             QCoreApplication::translate("main", "Формат даты времени для подстановки в имя файла. По умолчанию 'ddMMyy_hhmmss'"),
                             QCoreApplication::translate("main", "ddMMyy_hhmmss"));

  clo << QCommandLineOption (QStringList() << "fntmp" << "file_name_template",
                             QCoreApplication::translate("main", "Шаблон имени файла. По умолчанию '{DEVICE}_{DATETIME}. Допустимо использование выражений {DEVICE}, {DATETIME}, {DATE}, {TIME}'"),
                             QCoreApplication::translate("main", "{DEVICE}_{DATETIME}"));

  clo << QCommandLineOption ("out_ip",
                             QCoreApplication::translate("main", "IP адрес для ретрансляции данных. По умолчанию 127.0.0.1"),
                             QCoreApplication::translate("main", "127.0.0.1"));  
  
  clo << QCommandLineOption ("out_port",
                             QCoreApplication::translate("main", "Порт для ретрансляции данных. По умолчанию 8100"),
                             QCoreApplication::translate("main", "8001"));  

  
  for(QCommandLineOption o: clo)
    parser.addOption(o);
  
  if(!parser.parse(a.arguments())) 
    qInfo() << parser.errorText() << '\n' << 'n' << parser.helpText();
  
  setParams(&parser);
  
  SvRlsArchiver *rls_archiver = new SvRlsArchiver(&params, 0);
  rls_archiver->start();
  
  bool wait_cmd = true;
  QTextStream qtin(stdin);
  QTextStream qtout(stdout);
  QString cmd;
  while(wait_cmd) {

    qtin >> cmd; // = qtin->readLine();
    
    if(cmd == "stop") {
//      wait_cmd = false;
      rls_archiver->stop();
      while(!rls_archiver->isFinished()) QCoreApplication::processEvents();
      rls_archiver = nullptr;
      
      qtout << "Archiving stoppped" << endl;
               
    }
    else if (cmd == "start" && !rls_archiver) {
      if(!rls_archiver) {
        
        qtout << "Starting archiving" << endl;
        
        rls_archiver = new SvRlsArchiver(&params, 0);
        rls_archiver->start();
      }
      
    }
    else if(cmd == "quit") {
      wait_cmd = false;
      if(rls_archiver) {
        rls_archiver->stop();
        while(!rls_archiver->isFinished()) QCoreApplication::processEvents();
      }
    }
  }
    
  return a.exec();
  qtout << "Closing" << endl;
  if(rls_archiver) {
    rls_archiver->stop();
    while(!rls_archiver->isFinished()) QCoreApplication::processEvents();
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
