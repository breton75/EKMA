#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  
  
  QCommandLineParser parser;
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
  parser.setApplicationDescription("Отображение информации от РЛС");
  parser.addHelpOption();
  parser.addVersionOption();
  
  QList<QCommandLineOption> clo;
  parser.addPositionalArgument("source", QCoreApplication::translate("main", "Источник данных для вывода на экран: udp, archive."));
  parser.addPositionalArgument("port", QCoreApplication::translate("main", "Порт для приема данных по протоколу UDP."));
  
  
  clo << QCommandLineOption (QStringList() << "s" << "source",
                        QCoreApplication::translate("main", "Источник данных для вывода на экран: udp, archive."),
                        QCoreApplication::translate("main", "udp"));

  clo << QCommandLineOption (QStringList() << "p" << "port",
                             QCoreApplication::translate("main", "Порт для приема данных по протоколу UDP."),
                             QCoreApplication::translate("main", "8000"));
  
  clo << QCommandLineOption ("ip",
                             QCoreApplication::translate("main", "IP адрес для прослушивания данных."),
                             QCoreApplication::translate("main", "127.0.0.1"));

  clo << QCommandLineOption (QStringList() << "fd" << "fromdate",
                             QCoreApplication::translate("main", "Дата начала просмотра архивных данных в формате 'DDMMYYYY'."),
                             QCoreApplication::translate("main", "current"));

  clo << QCommandLineOption (QStringList() << "ft" << "fromtime",
                             QCoreApplication::translate("main", "Время начала просмотра архивных данных в формате 'HHMMSS'."),
                             QCoreApplication::translate("main", "current"));

  clo << QCommandLineOption (QStringList() << "r" << "radius",
                             QCoreApplication::translate("main", "Количество отображаемых точек."),
                             QCoreApplication::translate("main", "600"));

  clo << QCommandLineOption (QStringList() << "bc" << "bcolor",
                             QCoreApplication::translate("main", "Цвет фона монитора."),
                             QCoreApplication::translate("main", "0"));

  clo << QCommandLineOption (QStringList() << "dc" << "dcolor",
                             QCoreApplication::translate("main", "Цвет данных на мониторе."),
                             QCoreApplication::translate("main", "#FFFF96"));  
  
  clo << QCommandLineOption ("autostart",
                             QCoreApplication::translate("main", "Автоматический запуск отображения при запуске."));  
  
  clo << QCommandLineOption (QStringList() << "ir" << "imgresolution",
                             QCoreApplication::translate("main", "То же, что radius"),
                             QCoreApplication::translate("main", "600"));  
  
  clo << QCommandLineOption (QStringList() << "ppl" << "pointperline",
                             QCoreApplication::translate("main", "То же, что radius"),
                             QCoreApplication::translate("main", "600"));  
  
  clo << QCommandLineOption ("nocontrols",
                             QCoreApplication::translate("main", "Не отображать элементы управления."));  

  
  for(QCommandLineOption o: clo)
    parser.addOption(o);
  
  if(!parser.parse(a.arguments())) 
    qInfo() << parser.errorText() << '\n' << 'n' << parser.helpText();
    
  MainWindow w(parser);
  w.show();
  
  return a.exec();
}
