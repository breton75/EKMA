#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCommandLineParser>
#include <QColor>

#include "sv_rlswidget.h"

#define PRLI_PORT 8000

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QCommandLineParser &parser, QWidget *parent = 0);
  ~MainWindow();
  
private:
  Ui::MainWindow *ui;
  
  SvRlsWidget *_rls_widget;
  
  SvRls2bitThread *_rls_thread = nullptr;
  
  void *_buffer;
  
signals:
  void thread_started();
  void thread_stopped();
  
private slots:
  void _start_stop_thread(quint32 ip, quint16 port);
  
};

#endif // MAINWINDOW_H
