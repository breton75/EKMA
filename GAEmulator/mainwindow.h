#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStringList>
#include <QApplication>
#include <QHBoxLayout>

#include "../../svlib/sv_log.h"
#include "../general/sv_gathread.h"
#include "../../svlib/sv_settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private slots:
  void on_bnStart_clicked();
  
  void on_bnStop_clicked();
  
  void on_bnSelectFiles_clicked();
  
private:
  Ui::MainWindow *ui;
  
  svlog::SvLog log;
  
  SvGAFile2Udp *_ga_udp_thread = nullptr;
  
  QStringList _files;
  
  QLabel *_lblStatus;
  
  QDir _dir;
  
signals:
  void thread_udp_started(bool started);
  
private slots:
  void _start_stop_udp_thread(bool started);
  void fileReaded(QString file_name);
  void fileReadError(QString filename, QString error);
};

#endif // MAINWINDOW_H
