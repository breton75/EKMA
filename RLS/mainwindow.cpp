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
  
  
  
  int w = AppParams::readParam(this, "RLS", "Width", 640).toInt();
  int h = AppParams::readParam(this, "RLS", "Height", 640).toInt();
  int r = AppParams::readParam(this, "RLS", "Resolution", 360).toInt();
//  AppParams::readParam(this, "RLS", "Draw radius", MAX_LINE_SIZE).toInt();

  
  _buffer = malloc(AZIMUTHS_COUNT * MAX_LINE_SIZE);
  memset(_buffer, char(0), AZIMUTHS_COUNT * MAX_LINE_SIZE);
  
  /**  разбираем параметры  **/
  SvRlsWidgetParams p;
  
  p.source = parser.isSet("source") ? (parser.value("source").toLower() == "archive" ? 1 : 0)
                                    :  AppParams::readParam(this, "PARAMS", "source", QVariant(0)).toInt();
  
  p.ip = parser.isSet("ip") ? parser.value("ip").toUInt()
                            : AppParams::readParam(this, "PARAMS", "ip", QHostAddress("127.0.0.1").toIPv4Address()).toUInt();
  
  p.port = parser.isSet("port") ? parser.value("port").toUInt()
                                : AppParams::readParam(this, "PARAMS", "port", QVariant(8000)).toUInt();
  
  p.fromdate = parser.isSet("fromdate") ? QDate::fromString(parser.value("fromdate"), "ddMMyyyy")
                                        : AppParams::readParam(this, "PARAMS", "fromdate", QVariant(QDate::currentDate())).toDate();
  
  p.fromtime = parser.isSet("fromtime") ? QTime::fromString(parser.value("fromtime"), "hhmmss")
                                        : AppParams::readParam(this, "PARAMS", "fromtime", QVariant(QTime::currentTime())).toTime();
  
  p.painter_bkgnd_color = parser.isSet("bcolor") ? QColor::fromRgb(parser.value("bcolor").toInt(nullptr, 16))
                                                 : AppParams::readParam(this, "PARAMS", "painter_bkgnd_color", QVariant("000000")).toString().toInt(nullptr, 16);
  
  p.painter_data_color = parser.isSet("dcolor") ? QColor::fromRgb(parser.value("dcolor").toInt(nullptr, 16))
                                                : AppParams::readParam(this, "PARAMS", "painter_data_color", QVariant("FFFF00")).toString().toInt(nullptr, 16);
  
  p.radius = parser.isSet("radius") ? parser.value("radius").toUInt() : AppParams::readParam(this, "PARAMS", "radius", QVariant(600)).toUInt();
  p.img_resolution = parser.isSet("imgresolution") ? parser.value("imgresolution").toUInt() : AppParams::readParam(this, "PARAMS", "img_resolution", QVariant(600)).toUInt();
  p.radius = parser.isSet("pointperline") ? parser.value("pointperline").toUInt() : AppParams::readParam(this, "PARAMS", "draw_points_per_line", QVariant(600)).toUInt();
  
  p.autostart = parser.isSet("autostart") ? true : AppParams::readParam(this, "PARAMS", "autostart", true).toBool();
  p.no_controls = parser.isSet("nocontrols") ? true : AppParams::readParam(this, "PARAMS", "nocontrols", false).toBool();
  qDebug() << p.no_controls;

  _rls_widget = new SvRlsWidget(_buffer, p);
  _rls_widget->setParent(this);
  _rls_widget->resize(w, h);
  ui->vlayMain->addWidget(_rls_widget);
  
  connect(this, SIGNAL(thread_started()), _rls_widget, SLOT(started()));
  connect(this, SIGNAL(thread_stopped()), _rls_widget, SLOT(stopped()));
  
  connect(_rls_widget, SIGNAL(start_stop_clicked(quint32,quint16)), this, SLOT(_start_stop_thread(quint32,quint16)));
  
  if(p.autostart)
    _start_stop_thread(p.ip, p.port);
    
  
  
} 

MainWindow::~MainWindow()
{
  if(_rls_thread) {
    _rls_thread->stop();
    while(!_rls_thread->isFinished()) QApplication::processEvents();
  }
  
  /* сохраняем парметры программы */
  AppParams::saveWindowParams(this, this->size(), this->pos(), this->windowState());
  
  delete ui;
}

void MainWindow::_start_stop_thread(quint32 ip, quint16 port)
{
  if(_rls_thread) {
    
    _rls_thread->stop();
    while(!_rls_thread->isFinished()) QApplication::processEvents();
    delete _rls_thread;
    
    _rls_thread = nullptr;
    
    emit thread_stopped();
    
  }
  else {
    
    _rls_thread = new SvRls2bitThread(_buffer, ip, port, this);
  //  connect(_rls_thread, QThread::finished, _rls_thread, &QObject::deleteLater);
    connect(_rls_thread, SIGNAL(lineUpdated(int)), _rls_widget, SLOT(drawLine(int)));
    
    _rls_thread->start();
    
    emit thread_started();
    
  }
}
