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

  
  _buffer = malloc(MAX_LINE_POINT_COUNT * sizeof(float));
  memset(_buffer, char(0), MAX_LINE_POINT_COUNT * sizeof(float));
  
  /**  разбираем параметры  **/
  svgawdg::SvGAWidgetParams p;
  
  p.source = parser.isSet("source") ? (parser.value("source").toLower() == "archive" ? svgawdg::archive : svgawdg::udp)
                                    :  AppParams::readParam(this, "PARAMS", "source", svgawdg::udp).toInt();
  
  p.ip = parser.isSet("ip") ? parser.value("ip").toUInt()
                            : AppParams::readParam(this, "PARAMS", "ip", QHostAddress("127.0.0.1").toIPv4Address()).toUInt();
  
  p.port = parser.isSet("port") ? parser.value("port").toUInt()
                                : AppParams::readParam(this, "PARAMS", "port", 8001).toUInt();
  
  p.fromdate = parser.isSet("fromdate") ? QDate::fromString(parser.value("fromdate"), "ddMMyyyy")
                                        : AppParams::readParam(this, "PARAMS", "fromdate", QDate::currentDate()).toDate();
  
  p.fromtime = parser.isSet("fromtime") ? QTime::fromString(parser.value("fromtime"), "hhmmss")
                                        : AppParams::readParam(this, "PARAMS", "fromtime", QTime::currentTime()).toTime();
  
  p.painter_bkgnd_color = parser.isSet("bcolor") ? QColor::fromRgb(parser.value("bcolor").toInt(nullptr, 16))
                                                 : QColor(AppParams::readParam(this, "PARAMS", "painter_bkgnd_color", "#000000").toString()) ; //.toInt(nullptr, 16));
  
  p.painter_data_color = parser.isSet("dcolor") ? QColor::fromRgb(parser.value("dcolor").toInt(nullptr, 16))
                                                : QColor(AppParams::readParam(this, "PARAMS", "painter_data_color", "#FFFF00").toString());
  
//  p.radius = parser.isSet("radius") ? parser.value("radius").toUInt() : AppParams::readParam(this, "PARAMS", "radius", QVariant(600)).toUInt();
  p.display_point_count = parser.isSet("displaypc") ? parser.value("displaypc").toUInt() : AppParams::readParam(this, "PARAMS", "display_point_count", 640).toUInt();
  p.line_point_count = parser.isSet("linepc") ? parser.value("linepc").toUInt() : AppParams::readParam(this, "PARAMS", "line_point_count", 1000).toUInt();
  
  p.autostart = parser.isSet("autostart") ? true : AppParams::readParam(this, "PARAMS", "autostart", true).toBool();
  p.no_controls = parser.isSet("nocontrols") ? true : AppParams::readParam(this, "PARAMS", "nocontrols", false).toBool();

  p.archive_path = parser.isSet("archive_path") ? parser.value("archive_path")
                                      : AppParams::readParam(this, "PARAMS", "archive_path", "archive").toString();
  
  p.y_autoscale = true;
  p.x_range = p.line_point_count;
  
  _ga_widget = new svgawdg::SvGAWidget(_buffer, p);
  _ga_widget->setParent(this);
  
  svgraph::GraphParams gp;
  gp.line_color = p.painter_data_color;
  gp.line_width = 2;
  _ga_widget->painter()->addGraph(0, gp);
//  _ga_widget->painter()->appendData(0, -50000);
//  _ga_widget->painter()->appendData(0, 50000);
    
  ui->vlayMain->addWidget(_ga_widget);
  
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
  AppParams::saveParam(this, "PARAMS", "fromdate", p.fromdate);
  AppParams::saveParam(this, "PARAMS", "fromtime", p.fromtime);
  AppParams::saveParam(this, "PARAMS", "painter_bkgnd_color", p.painter_bkgnd_color.name());
  AppParams::saveParam(this, "PARAMS", "painter_data_color",  p.painter_data_color.name());
  AppParams::saveParam(this, "PARAMS", "display_point_count", p.display_point_count);
  AppParams::saveParam(this, "PARAMS", "line_point_count", p.line_point_count);
  AppParams::saveParam(this, "PARAMS", "autostart", p.autostart);
  
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
    connect(_ga_udp_thread, SIGNAL(dataUpdated(quint64)), _ga_widget->painter(), SLOT(drawData(quint64)));
    
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
    
    connect(_ga_archive_thread, SIGNAL(dataUpdated()), _ga_widget->painter(), SLOT(drawData()));
    
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
