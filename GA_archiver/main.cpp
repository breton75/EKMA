#include <QCoreApplication>
#include <QCommandLineParser>
#include "stdio.h"
#include <QTextCodec>
#include <QProcess>
#include <QTimer>

#include "../general/sv_gathread.h"
#include "../general/sv_cleanupthread.h"
#include "../../svlib/sv_sqlite.h"

extern SvSQLITE *SQLITE;

SvGAArchiverParams params;

QTimer cleanupTimer();

void setParams(QCommandLineParser *parser);
void cleanup();

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
  
  parser.setApplicationDescription(QString("\nАрхивация и ретрансляция гидроакустических данных по протоколу UDP v.%1")
                                   .arg(APP_VERSION));
  
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

  clo << QCommandLineOption ("autostart", QCoreApplication::translate("main", "Запуск архивации при запуске программы.\n"));
  
  clo << QCommandLineOption ("cleanup",
                             QCoreApplication::translate("main", "Удалять файлы архива старше [n] дней. По умолчанию 7.\n"),
                             "7",
                             "7");
  
  QCommandLineOption nocleanupOption("nocleanup", QCoreApplication::translate("main", "Не удалять устаревшие файлы из архива.\n"));
  clo << nocleanupOption;
  
  
  for(QCommandLineOption o: clo)
    parser.addOption(o);
  
  parser.addPositionalArgument("ip", "ip адрес источника данных.", "-ip <source ip>");
  parser.addPositionalArgument("out_ip", "ip адрес для ретрансляции данных.", "-out_ip <out ip>");

  
  if (!parser.parse(a.arguments())) {
      qtout << parser.errorText() << endl << parser.helpText() << endl;
      return -1;
  }

  if (parser.isSet(versionOption)) {
    qtout << APP_VERSION << endl;
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

  /* если не стоит флажок 'не удалять старые файлы', то готовимся к работе с БД */
  SvCleanupThread *cleanup_thr = nullptr;
  SQLITE = nullptr;
  
  if(!parser.isSet(nocleanupOption)) {
  
    /* проверяем наличие БД */
    QString dbFileName = QString("%1/%2.sqlite")
                         .arg(QCoreApplication::applicationDirPath())
                         .arg(QCoreApplication::applicationName());
    
    QFile fbd(dbFileName);
    if(!fbd.exists()) {
      qtout << QString("Файл базы данных не найден. Создаем новый.\n").toUtf8();
      
      QString cmd = QString("sqlite3 %1 "
                            "\"create table files (\n"
                            "     id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n"
                            "     text filename,\n"
                            "     text date_begin,\n"
                            "     text date_end);\"\n")
                    .arg(dbFileName);
      
      qtout << cmd << '\n';
    
      QProcess* p = new QProcess();
      p->start(QString(cmd));
      
      if(!p->waitForStarted()) {
        p->close();
        delete p;
        qtout << QString("Не удалось выполнить команду.\n").toUtf8() << '\n';
        return -1;
      }
      
      if(!p->waitForFinished(1000)) {
        QByteArray errb = p->readAllStandardError();  
        p->close();
        delete p;
        qtout << errb << '\n';
        return -1;
      }
    }
    
    SQLITE = new SvSQLITE(0, dbFileName);
    QSqlError err = SQLITE->connectToDB();
    
    if(err.type() != QSqlError::NoError) {
      qtout << err.databaseText();
      return -1;
    }
    
    int cleanup_period = parser.value("cleanup").toInt();
    cleanup_thr = new SvCleanupThread(SQLITE, cleanup_period);
    cleanup_thr->start();
    
  }
  
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  if(QSqlError::NoError != SQLITE->execSQL(QString("select * from files"), q).type())
  {
    qtout << q->lastError().databaseText();
    q->finish();
    return -1;
  }
  
  
  setParams(&parser);
  
  SvGAArchiver *ga_archiver = nullptr;
  
  if(parser.isSet("autostart")) {
    ga_archiver = new SvGAArchiver(&params, SQLITE, 0);
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
        
        ga_archiver = new SvGAArchiver(&params, SQLITE, 0);
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

void cleanup()
{
  
}
