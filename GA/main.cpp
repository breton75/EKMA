#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  
  
  QCommandLineParser parser;
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
  parser.setApplicationDescription(QString("Отображение информации от гидроакустических устройств v.%1")
                                   .arg(APP_VERSION));
  
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();
  
  QList<QCommandLineOption> clo;
  
  clo << QCommandLineOption (QStringList() << "s" << "source",
                        QCoreApplication::translate("main", "Источник данных для вывода на дисплей: 'udp', 'archive'. По умолчанию 'udp'."),
                        "udp",
                        "udp");

  clo << QCommandLineOption ("ip",
                             QCoreApplication::translate("main", "IP адрес для прослушивания данных."),
                             "127.0.0.1",
                             "127.0.0.1");
  
  clo << QCommandLineOption (QStringList() << "p" << "port",
                             QCoreApplication::translate("main", "Порт для приема данных по протоколу UDP."),
                             "9001",
                             "9001");
  
  clo << QCommandLineOption (QStringList() << "path" << "path_to_archive",
                             QCoreApplication::translate("main", "Путь к каталогу архива."),
                             "./archive",
                             "./archive");

  clo << QCommandLineOption (QStringList() << "bc" << "bcolor",
                             QCoreApplication::translate("main", "Цвет фона дисплея. По умолчанию #000000"),
                             "#000000",
                             "#000000");

  clo << QCommandLineOption (QStringList() << "dc" << "dcolor",
                             QCoreApplication::translate("main", "Цвет данных на дисплее. По умолчанию #FFFF00"),
                             "#FFFF00",
                             "#FFFF00");  
  
  clo << QCommandLineOption (QStringList() << "display" << "display_point_count",
                             QCoreApplication::translate("main", "Размер дисплея в точках. По умолчанию 640x640"),
                             "640",
                             "640");  
//  clo.last().setFlags(QCommandLineOption::HiddenFromHelp);
  
  clo << QCommandLineOption (QStringList() << "buffer" << "buffer_point_count",
                             QCoreApplication::translate("main", "Размер буфера отображения в точках. Возможные значения 48000, 24000, 16000, 12000, 8000, 6000, 4000, 2000. По умолчанию 48000"),
                             "48000",
                             "48000");  
  
  clo << QCommandLineOption ("autostart",
                             QCoreApplication::translate("main", "Автоматический запуск отображения при запуске."));  
  
  
  clo << QCommandLineOption ("autoscale",
                             QCoreApplication::translate("main", "Автомасштаб по оси y."));  
  
  clo << QCommandLineOption ("nocontrols",
                             QCoreApplication::translate("main", "Не отображать элементы управления."));  
  
  
  for(QCommandLineOption o: clo)
    parser.addOption(o);
  
  if(!parser.parse(a.arguments())) {
    qInfo() << parser.errorText() << '\n';
    return -1; 
  }
  
  if (parser.isSet(versionOption)) {
    qInfo() << APP_VERSION << endl;
    return 0;
}

  if (parser.isSet(helpOption)) {
    qInfo() << parser.helpText() << endl;
    return 0;
}
    
  MainWindow w(parser);
  w.show();
  
  return a.exec();
}
