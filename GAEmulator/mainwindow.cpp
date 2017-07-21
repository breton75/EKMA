#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace svlog;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  log = SvLog(ui->textEdit, this);
 
  connect(this, SIGNAL(thread_udp_started(bool)), this, SLOT(_start_stop_udp_thread(bool)));
  
  _lblStatus = new QLabel(statusBar());
  
  statusBar()->addWidget(_lblStatus);
  
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_bnStart_clicked()
{
  if(_files.isEmpty()) {
    QMessageBox::critical(this, "Error", "Необходимо выбрать файлы", QMessageBox::Ok);
    ui->bnSelectFiles->setFocus();
    return;
  }
    
    
  _ga_udp_thread = new SvGAFile2Udp(&_files, ui->checkCycle->isChecked(),
                                    ui->editIp->text(), ui->spinPort->value(), this);
  
  connect(_ga_udp_thread, SIGNAL(fileReadError(QString,QString)), this, SLOT(fileReadError(QString, QString)));
  connect(_ga_udp_thread, SIGNAL(fileReaded(QString)), this, SLOT(fileReaded(QString)));
  _ga_udp_thread->start();
  
  emit thread_udp_started(true);
  
}

void MainWindow::on_bnStop_clicked()
{
  _ga_udp_thread->stop();
  while(!_ga_udp_thread->isFinished()) QApplication::processEvents();
  delete _ga_udp_thread;
  
  _ga_udp_thread = nullptr;
  
  emit thread_udp_started(false);
}

void MainWindow::fileReadError(QString filename, QString error)
{
  on_bnStop_clicked();
  
  QFileInfo fi(filename);
  log << svlog::Time << svlog::Critical << QString("Ошибка чтения файла %1\n%2").arg(fi.fileName()).arg(error) << svlog::endl;
  
}

void MainWindow::on_bnSelectFiles_clicked()
{
  QDir dir("D:/pcm/30122016/pcm"); // QDir::currentPath());
  
  _files = QFileDialog::getOpenFileNames(this, tr("Выбрать файлы"), dir.path(), "Файлы pcmd (*.pcmd);;Все файлы (*.*)");
  
  qSort(_files);

  ui->textEdit_2->clear();
  for(QString file_name: _files) {
    QFileInfo fi(file_name);
    ui->textEdit_2->append(fi.fileName());
  }
}

void MainWindow::_start_stop_udp_thread(bool started)
{
    ui->editIp->setEnabled(!started);
    ui->spinPort->setEnabled(!started);
    ui->bnStart->setEnabled(!started);
    ui->bnSelectFiles->setEnabled(!started);
    ui->textEdit_2->setEnabled(!started);
        
    ui->bnStop->setEnabled(started);
    
    log << svlog::Time << svlog::Info
        << QString("Трансляция %1").arg(started ? "запущена" : "остановлена")
        << svlog::endl;
    
    QApplication::processEvents();

}

void MainWindow::fileReaded(QString file_name)
{
  QFileInfo fi(file_name);
  _lblStatus->setText(QString("Текущий файл: %1  Начало: %2").arg(fi.fileName()).arg(QTime::currentTime().toString("hh:mm:ss")));
}
