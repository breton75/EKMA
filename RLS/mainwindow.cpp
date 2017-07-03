#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  int w = AppParams::readParam(this, "RLS", "Width", 640).toInt();
  int h = AppParams::readParam(this, "RLS", "Height", 640).toInt();
  int r = AppParams::readParam(this, "RLS", "Resolution", 360).toInt();
//  AppParams::readParam(this, "RLS", "Draw radius", MAX_LINE_SIZE).toInt();

  
  _buffer = malloc(AZIMUTHS_COUNT * MAX_LINE_SIZE);
  memset(_buffer, char(0), AZIMUTHS_COUNT * MAX_LINE_SIZE);
  
  
  _rls_widget = new SvRlsWidget(_buffer, "RLS");
  _rls_widget->setParent(this);
//  _rls_widget->resize(w, h);
  ui->vlayMain->addWidget(_rls_widget);
  
  QPushButton *bn = new QPushButton("stop", this);
  
  
  this->resize(r + 25, r + 54);
  
  _rls_thread = new SvRls2bitThread(_buffer, PRLI_PORT, this);
//  connect(_rls_thread, QThread::finished, _rls_thread, &QObject::deleteLater);
  connect(_rls_thread, SIGNAL(lineUpdated(int)), _rls_widget, SLOT(drawLine(int)));
  connect(bn, SIGNAL(pressed()), _rls_thread, SLOT(stop()));
  _rls_thread->start();
  
} 

MainWindow::~MainWindow()
{

  //  App
  
  delete ui;
}
