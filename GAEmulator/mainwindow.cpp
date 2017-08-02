#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace svlog;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  setWindowIcon(QIcon(":/appicons/icons/Sound.ico"));
  setWindowTitle(QString("Эмулятор гидроакустических устройств v.").arg(APP_VERSION));
  
  /* параметры главного окна */
  AppParams::WindowParams window_params = AppParams::readWindowParams(this);
  this->resize(window_params.size);
  this->move(window_params.position);
  this->setWindowState(window_params.state);
  
  /* параметры программы */
  ui->editIp->setText(AppParams::readParam(this, "PARAMS", "ip", "127.0.0.1").toString());
  ui->spinPort->setValue(AppParams::readParam(this, "PARAMS", "port", 9000).toUInt());
  _dir.setPath(AppParams::readParam(this, "PARAMS", "archive_path", "./pcmd").toString());
  ui->checkCycle->setChecked(AppParams::readParam(this, "PARAMS", "cycle", true).toBool()); 
  
  
  log = SvLog(ui->textEdit, this);
 
  connect(this, SIGNAL(thread_udp_started(bool)), this, SLOT(_start_stop_udp_thread(bool)));
  
  _lblStatus = new QLabel(statusBar());
  
  statusBar()->addWidget(_lblStatus);
  
}

MainWindow::~MainWindow()
{
  /* сохраняем парметры программы */
  AppParams::saveWindowParams(this, this->size(), this->pos(), this->windowState());
  
  AppParams::saveParam(this, "PARAMS", "ip", ui->editIp->text());
  AppParams::saveParam(this, "PARAMS", "port", ui->spinPort->value());
  AppParams::saveParam(this, "PARAMS", "archive_path", _dir.path());
  AppParams::saveParam(this, "PARAMS", "cycle", ui->checkCycle->isChecked());
  
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
  _files = QFileDialog::getOpenFileNames(this, tr("Выбрать файлы"), _dir.path(), "Файлы pcmd (*.pcmd);;Все файлы (*.*)");
  
  if(_files.count()) {
    QFileInfo fi(_files.first());
    _dir.setPath(fi.filePath());
  }
  
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
