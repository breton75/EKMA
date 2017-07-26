#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QCommandLineParser &parser, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  /* параметры главного окна */
  AppParams::WindowParams window_params = AppParams::readWindowParams(this);
  this->resize(window_params.size);
  this->move(window_params.position);
  this->setWindowState(window_params.state);
  
  
  
//  int w = AppParams::readParam(this, "RLS", "Width", 640).toInt();
//  int h = AppParams::readParam(this, "RLS", "Height", 640).toInt();
//  int r = AppParams::readParam(this, "RLS", "Resolution", 360).toInt();
//  AppParams::readParam(this, "RLS", "Draw radius", MAX_LINE_SIZE).toInt();

  
  _buffer = malloc(AZIMUTHS_COUNT * MAX_LINE_POINT_COUNT);
  memset(_buffer, char(0), AZIMUTHS_COUNT * MAX_LINE_POINT_COUNT);
  
  /**  разбираем параметры  **/
  SvRlsWidgetParams p;
  
  /* если хоть один параметр задан в командной строке, то берем параметры из нее */
  if(parser.optionNames().count() > 0) {
  
    p.source = parser.value("source").toLower() == "archive" ? svrlswdg::archive : svrlswdg::udp;                                      
    p.ip = QHostAddress(parser.value("ip")).toIPv4Address();
    p.port = parser.value("port").toUInt();
    p.painter_bkgnd_color = QColor(parser.value("bcolor"));
    p.painter_data_color = QColor(parser.value("dcolor"));
    p.display_point_count = parser.value("display").toUInt();
    p.line_point_count = parser.value("line").toUInt();
    p.autostart = parser.isSet("autostart");
    p.no_controls = parser.isSet("nocontrols");  
    p.archive_path = parser.value("path");
    
  }
  
  /* иначе берем параметры из инишника */
  else {  
    
    p.source = AppParams::readParam(this, "PARAMS", "source", svrlswdg::udp).toInt();
    p.ip = AppParams::readParam(this, "PARAMS", "ip", QHostAddress("127.0.0.1").toIPv4Address()).toUInt();
    p.port = AppParams::readParam(this, "PARAMS", "port", 8001).toUInt();
    p.painter_bkgnd_color = QColor(AppParams::readParam(this, "PARAMS", "painter_bkgnd_color", "#000000").toString());
    p.painter_data_color = QColor(AppParams::readParam(this, "PARAMS", "painter_data_color", "#FFFF00").toString());
    p.display_point_count = AppParams::readParam(this, "PARAMS", "display_point_count", 640).toUInt();
    p.line_point_count = AppParams::readParam(this, "PARAMS", "line_point_count", 1200).toUInt();
    p.autostart = AppParams::readParam(this, "PARAMS", "autostart", false).toBool();
    p.no_controls = AppParams::readParam(this, "PARAMS", "nocontrols", false).toBool();
    p.archive_path = AppParams::readParam(this, "PARAMS", "archive_path", "archive").toString();
    
  }
  
  
  _rls_widget = new SvRlsWidget(_buffer, p);
  _rls_widget->setParent(this);
  
  ui->vlayMain->addWidget(_rls_widget);
  
  connect(this, SIGNAL(thread_udp_started()), _rls_widget, SLOT(startedUdp()));
  connect(this, SIGNAL(thread_udp_stopped()), _rls_widget, SLOT(stoppedUdp()));
  connect(this, SIGNAL(thread_archive_started()), _rls_widget, SLOT(startedArchive()));
  connect(this, SIGNAL(thread_archive_stopped()), _rls_widget, SLOT(stoppedArchive()));
  
  connect(_rls_widget, SIGNAL(start_stop_udp_clicked(quint32,quint16)), this, SLOT(_start_stop_udp_thread(quint32,quint16)));
  connect(_rls_widget, SIGNAL(start_stop_archive_clicked(QStringList*)), this, SLOT(_start_stop_archive_thread(QStringList*)));

  if(p.autostart && (p.source == svrlswdg::udp))
    _start_stop_udp_thread(p.ip, p.port);
  
} 

MainWindow::~MainWindow()
{
  if(_rls_udp_thread) {
    _rls_udp_thread->stop();
    while(!_rls_udp_thread->isFinished()) QApplication::processEvents();
  }
  
  if(_rls_archive_thread) {
    _rls_archive_thread->stop();
    while(!_rls_archive_thread->isFinished()) QApplication::processEvents();
  }
  
  /* сохраняем парметры программы */
  AppParams::saveWindowParams(this, this->size(), this->pos(), this->windowState());
  
  SvRlsWidgetParams p = _rls_widget->params();
  AppParams::saveParam(this, "PARAMS", "source", p.source);
  AppParams::saveParam(this, "PARAMS", "ip", p.ip);
  AppParams::saveParam(this, "PARAMS", "port", p.port);
  AppParams::saveParam(this, "PARAMS", "fromdate", p.fromdate);
  AppParams::saveParam(this, "PARAMS", "fromtime", p.fromtime);
  AppParams::saveParam(this, "PARAMS", "painter_bkgnd_color", p.painter_bkgnd_color.name());
  AppParams::saveParam(this, "PARAMS", "painter_data_color",  p.painter_data_color.name());
  AppParams::saveParam(this, "PARAMS", "display_point_count", p.display_point_count);
  AppParams::saveParam(this, "PARAMS", "line_point_count", p.line_point_count);
  
  delete ui;
}

void MainWindow::_start_stop_udp_thread(quint32 ip, quint16 port)
{
  if(_rls_udp_thread) {
    
    _rls_udp_thread->stop();
    while(!_rls_udp_thread->isFinished()) QApplication::processEvents();
    delete _rls_udp_thread;
    
    _rls_udp_thread = nullptr;
    
    emit thread_udp_stopped();
    
  }
  else {
    
    _rls_udp_thread = new SvRls2bitThread(_buffer, ip, port, this);
  //  connect(_rls_thread, QThread::finished, _rls_thread, &QObject::deleteLater);
    connect(_rls_udp_thread, SIGNAL(lineUpdated(int, quint16)), _rls_widget->painter(), SLOT(drawLine(int, quint16)));
    
    _rls_udp_thread->start();
    
    emit thread_udp_started();
    
  }
}

void MainWindow::_start_stop_archive_thread(QStringList *files)
{
  if(_rls_archive_thread) {
    
    _rls_archive_thread->stop();
    while(!_rls_archive_thread->isFinished()) QApplication::processEvents();
    delete _rls_archive_thread;
    
    _rls_archive_thread = nullptr;
    
    emit thread_archive_stopped();
    
  }
  else {
    
    _rls_archive_thread = new SvRlsExtractThread(_buffer, files, this);
  //  connect(_rls_thread, QThread::finished, _rls_thread, &QObject::deleteLater);
    connect(_rls_archive_thread, SIGNAL(lineUpdated(int, quint16)), _rls_widget->painter(), SLOT(drawLine(int, quint16)));
    connect(_rls_archive_thread, SIGNAL(fileReaded(QString)), _rls_widget, SLOT(fileReaded(QString)));
    connect(_rls_archive_thread, SIGNAL(fileReadError(QString, QString)), this, SLOT(fileReadError(QString, QString)));
    
    _rls_archive_thread->start();
    
    emit thread_archive_started();
    
  }
}

void MainWindow::fileReadError(QString filename, QString error)
{
  QFileInfo fi(filename);
  QMessageBox::critical(this, "Error", QString("Ошибка чтения файла %1\n%2").arg(fi.fileName()).arg(error), QMessageBox::Ok);
  
  _rls_archive_thread->stop();
  while(!_rls_archive_thread->isFinished()) QApplication::processEvents();
  delete _rls_archive_thread;
  
  _rls_archive_thread = nullptr;
  
  emit thread_archive_stopped();
  
}
