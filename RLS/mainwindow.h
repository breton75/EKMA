#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sv_rlswidget.h"

#define PRLI_PORT 8000

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private:
  Ui::MainWindow *ui;
  
  SvRlsWidget *_rls_widget;
  
  SvRls2bitThread *_rls_thread = nullptr;
  
  void *_buffer;
  
};

#endif // MAINWINDOW_H
