#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QCommandLineParser &parser, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  setWindowIcon(QIcon(":/appicons/icons/Sound.ico"));
  setWindowTitle(tr("Отображение данных гидроакустики v.").arg(APP_VERSION));
  
  /* параметры главного окна */
  AppParams::WindowParams window_params = AppParams::readWindowParams(this);
  this->resize(window_params.size);
  this->move(window_params.position);
  this->setWindowState(window_params.state);
  
  
  _buffer = malloc(MAX_BUFFER_POINT_COUNT * sizeof(double));
  memset(_buffer, char(0), MAX_BUFFER_POINT_COUNT * sizeof(double));
  
  /**  разбираем параметры  **/
  svgawdg::SvGAWidgetParams p;
  
  /* если хоть один параметр задан в командной строке, то берем параметры из нее */
  if(parser.optionNames().count() > 0) {
  
    p.source = parser.value("source").toLower() == "archive" ? svgawdg::archive : svgawdg::udp;                                      
    p.ip = QHostAddress(parser.value("ip")).toIPv4Address();
    p.port = parser.value("port").toUInt();
    p.archive_path = parser.value("path");
    p.painter_bkgnd_color = QColor(parser.value("bcolor"));
    p.painter_data_color = QColor(parser.value("dcolor"));
    p.display_point_count = parser.value("display").toUInt();
    p.buffer_point_count = parser.value("buffer").toUInt();
    p.autostart = parser.isSet("autostart");
    p.no_controls = parser.isSet("nocontrols");  
    p.y_autoscale = parser.isSet("autoscale");
    
  }
  
  /* иначе берем параметры из инишника */
  else {  
    
    
    p.source = AppParams::readParam(this, "PARAMS", "source", svgawdg::udp).toInt();
    p.ip = AppParams::readParam(this, "PARAMS", "ip", QHostAddress("127.0.0.1").toIPv4Address()).toUInt();
    p.port = AppParams::readParam(this, "PARAMS", "port", 9001).toUInt();
    p.archive_path = AppParams::readParam(this, "PARAMS", "archive_path", "./archive").toString();
    p.painter_bkgnd_color = QColor(AppParams::readParam(this, "PARAMS", "painter_bkgnd_color", "#000000").toString());
    p.painter_data_color = QColor(AppParams::readParam(this, "PARAMS", "painter_data_color", "#FFFF00").toString());
    p.display_point_count = AppParams::readParam(this, "PARAMS", "display_point_count", 640).toUInt();
    p.buffer_point_count = AppParams::readParam(this, "PARAMS", "buffer_point_count", 48000).toUInt();
    p.autostart = AppParams::readParam(this, "PARAMS", "autostart", false).toBool();
    p.no_controls = AppParams::readParam(this, "PARAMS", "nocontrols", false).toBool();
    AppParams::readParam(this, "PARAMS", "y_autoscale", true).toBool();
    
  }
  
  _ga_widget = new svgawdg::SvGAWidget(_buffer, p);
  _ga_widget->setParent(this);
      
  ui->vlayMain->addWidget(_ga_widget);
  
  
  if(p.no_controls)
    resize(_ga_widget->painterSize().width() + 40, height()); // _ga_widget->painterSize().height() + 72);
  
  else
    resize(_ga_widget->painterSize().width() + 40 + 250, height()); //_ga_widget->painterSize().height() + 72);
  
  connect(this, SIGNAL(thread_udp_started()), _ga_widget, SLOT(startedUdp()));
  connect(this, SIGNAL(thread_udp_stopped()), _ga_widget, SLOT(stoppedUdp()));
  connect(this, SIGNAL(thread_archive_started()), _ga_widget, SLOT(startedArchive()));
  connect(this, SIGNAL(thread_archive_stopped()), _ga_widget, SLOT(stoppedArchive()));
  
  connect(_ga_widget, SIGNAL(start_stop_udp_clicked(quint32,quint16)), this, SLOT(_start_stop_udp_thread(quint32,quint16)));
  connect(_ga_widget, SIGNAL(start_stop_archive_clicked(QStringList*)), this, SLOT(_start_stop_archive_thread(QStringList*)));

  if(p.autostart && (p.source == svgawdg::udp))
    _start_stop_udp_thread(p.ip, p.port);
    
  
  
} 

MainWindow::~MainWindow()
{
  if(_ga_udp_thread) {
    _ga_udp_thread->stop();
    while(!_ga_udp_thread->isFinished()) QApplication::processEvents();
  }
  
  if(_ga_archive_thread) {
    _ga_archive_thread->stop();
    while(!_ga_archive_thread->isFinished()) QApplication::processEvents();
  }
  
  /* сохраняем парметры программы */
  AppParams::saveWindowParams(this, this->size(), this->pos(), this->windowState());
  
  svgawdg::SvGAWidgetParams p = _ga_widget->params();
  AppParams::saveParam(this, "PARAMS", "source", p.source);
  AppParams::saveParam(this, "PARAMS", "ip", p.ip);
  AppParams::saveParam(this, "PARAMS", "port", quint16(p.port));
  AppParams::saveParam(this, "PARAMS", "painter_bkgnd_color", p.painter_bkgnd_color.name());
  AppParams::saveParam(this, "PARAMS", "painter_data_color",  p.painter_data_color.name());
  AppParams::saveParam(this, "PARAMS", "buffer_point_count", p.buffer_point_count);
  AppParams::saveParam(this, "PARAMS", "y_autoscale", p.y_autoscale);
  
  delete ui;
}

void MainWindow::_start_stop_udp_thread(quint32 ip, quint16 port)
{
  if(_ga_udp_thread) {
    
    _ga_udp_thread->stop();
    while(!_ga_udp_thread->isFinished()) QApplication::processEvents();
    delete _ga_udp_thread;
    
    _ga_udp_thread = nullptr;
    
    emit thread_udp_stopped();
    
  }
  else {
    
    _ga_udp_thread = new SvGAThread(_buffer, ip, port, this);
  //  connect(_chart_thread, QThread::finished, _chart_thread, &QObject::deleteLater);
    connect(_ga_udp_thread, SIGNAL(dataUpdated(quint32)), _ga_widget->painter(), SLOT(drawData(quint32)));
    
    _ga_udp_thread->start();
    
    emit thread_udp_started();
    
  }
}

void MainWindow::_start_stop_archive_thread(QStringList *files)
{
  if(_ga_archive_thread) {
    
    _ga_archive_thread->stop();
    while(!_ga_archive_thread->isFinished()) QApplication::processEvents();
    delete _ga_archive_thread;
    
    _ga_archive_thread = nullptr;
    
    emit thread_archive_stopped();
    
  }
  else {
    
    _ga_archive_thread = new SvGAExtractThread(_buffer, files, true, this);
  //  connect(_chart_thread, QThread::finished, _chart_thread, &QObject::deleteLater);
    
    connect(_ga_archive_thread, SIGNAL(dataUpdated(quint32)), _ga_widget->painter(), SLOT(drawData(quint32)));
    
    connect(_ga_archive_thread, SIGNAL(fileReaded(QString)), _ga_widget, SLOT(fileReaded(QString)));
    connect(_ga_archive_thread, SIGNAL(fileReadError(QString, QString)), this, SLOT(fileReadError(QString, QString)));
    
    _ga_archive_thread->start();
    
    emit thread_archive_started();
    
  }
}

void MainWindow::fileReadError(QString filename, QString error)
{
  QFileInfo fi(filename);
  QMessageBox::critical(this, "Error", QString("Ошибка чтения файла %1\n%2").arg(fi.fileName()).arg(error), QMessageBox::Ok);
  
  _ga_archive_thread->stop();
  while(!_ga_archive_thread->isFinished()) QApplication::processEvents();
  delete _ga_archive_thread;
  
  _ga_archive_thread = nullptr;
  
  emit thread_archive_stopped();
  
}
