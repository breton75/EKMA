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
  
  SvRls2bitThread *_rls_udp_thread = nullptr;
  SvRlsExtractThread *_rls_archive_thread = nullptr;
  
  void *_buffer;
  
signals:
  void thread_udp_started();
  void thread_udp_stopped();
  void thread_archive_started();
  void thread_archive_stopped();  
  
private slots:
  void _start_stop_udp_thread(quint32 ip, quint16 port);
  void _start_stop_archive_thread(QStringList *files);
  
};

#endif // MAINWINDOW_H
