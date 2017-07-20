#include "sv_gathread.h"

#include "../../Common/sv_fnt.h"

/** *******  данные от РЛС  ************ **/
SvGAThread::SvGAThread(void *buffer,
                       quint32 ip,
                       quint16 port,
                       QObject *parent)
{
  setParent(parent);
  
  _ip = ip;
  _port = port;
  
  _buffer = buffer;
  
}

void SvGAThread::run()
{
  _playing = true;
  _finished = false;
  
  socket = new QUdpSocket();
  socket->bind(QHostAddress(_ip), _port);
  
  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
  qint64 datagram_size;
  
  while(_playing)
  {
    /** Note: This function may fail randomly on Windows!!! **/
    while (socket->waitForReadyRead(1)) // да! именно 1 мс.
    {
      while(socket->hasPendingDatagrams())
      {
        datagram_size = socket->pendingDatagramSize();

        if(datagram_size <= 0 || datagram_size > 0xFFFF) continue;
        
//        memset(_buffer, 0, dataSize);
        socket->readDatagram((char*)(_buffer), datagram_size);
        
        emit dataUpdated(quint64(datagram_size / sizeof(float)));
          
      }
    }
  }
  
  free(datagram);
  
  socket->close();
  _finished = true;  
  qDebug() << "udp finished";  
  
}

/** **********  АРХИВАЦИЯ ДАННЫХ РЛС  ********** **/
SvGAArchiver::SvGAArchiver(SvGAArchiverParams *params, 
                             QObject *parent) 
{
  setParent(parent);
  
  _params = params;
  
  _file_duration_sec = params->file_duration.hour() * 3600 + 
                             params->file_duration.minute() * 60 + 
                             params->file_duration.second();
  
  _total_duration_sec = params->total_duration.hour() * 3600 + 
                             params->total_duration.minute() * 60 + 
                             params->total_duration.second();
  
}

void SvGAArchiver::run()
{
  _playing = true;
  time_t fileTimeWatcher;
  time_t totalTimeWatcher;
  
  _socket = new QUdpSocket();
  _socket->bind(_params->port);
  
  _socket_out = new QUdpSocket();

  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
  _new_file = true;

  totalTimeWatcher = time(NULL);
  fileTimeWatcher = totalTimeWatcher;
  
  qint64 datagram_size;
  
  /* для формирования имени файла */
  svfnt::SvRE re;
  re.relist << qMakePair(svfnt::RE_DEVICE, _params->device_name);

  while(_playing)
  {
    /**** проверяем, задано ли полное время записи *****/
    if(_total_duration_sec)
    {
      /* если задано полное время записи, то           *
      * проверяем не истекло ли полное время записи        *
      * если истекло, то выходим и закрываем текущий файл  */
   
      if(time(NULL) > totalTimeWatcher + _total_duration_sec)
      {
        qDebug() << QString("Total record time %1 elapsed")
                            .arg(_params->total_duration.toString("hh:mm:ss"));
        emit TotalTimeElapsed();
        _playing = false;
        break;
      }
    }
    
    /****** проверяем не истекло ли время записи текущего файла *****/
    if(time(NULL) > fileTimeWatcher + _file_duration_sec)
    {
      /* close output */
      if(_file) _file->close();
      if(_out) free(_out);

      _new_file = true;
      emit FileTimeElapsed();
    }
    
    /******** открываем новый файл для записи **********/
    if(_new_file)
    {      
      re.date_time = QDateTime::currentDateTime();
      
      QString file_name = svfnt::get_file_path(_params->path, _params->file_name_template, re);
      file_name += _ext;
      
      _file = new QFile(file_name);
      if(!_file->open(QIODevice::WriteOnly))
      {
        qDebug() << "Cant open file for writing" << file_name;
        _playing = false;
        break;
      }
      _out = new QDataStream(_file);
      
      fileTimeWatcher = time(NULL);
      
      emit NewFile();
      
      _new_file = false;
      _new_header = true;
      
      qDebug() << "Writing to file: " << file_name;
      
    }
    
    while (_socket->waitForReadyRead(1))
    {
      while(_socket->hasPendingDatagrams())
      {
        datagram_size = _socket->pendingDatagramSize();

        if(datagram_size <= 0) continue;
  
        _socket->readDatagram((char*)(datagram), datagram_size);
        
        _out->writeRawData((char*)(datagram), datagram_size);
        _socket_out->writeDatagram((char*)(datagram), datagram_size, QHostAddress(_params->out_ip), _params->out_port);
        
      }
    }
  }
  
  _file->close();
  _socket->close();  
  
  free(datagram);
  free(_out);
  
  _playing = false;  
  
  qDebug() << "archive finished";  
}


/** **********  ВОСПРОИЗВЕДЕНИЕ ДАННЫХ ИЗ АРХИВА ************** **/
SvGAExtractThread::SvGAExtractThread(void *buffer,
                                       QStringList *file_names,
                                       bool repeat,
                                       QObject *parent) 
{
  setParent(parent);
  
  _buffer = buffer;
  _repeat = repeat;
  _file_names = *file_names;
  
}

void SvGAExtractThread::run()
{
  _playing = true;
  _finished = false;
  
  QFile* file = new QFile();
  
  _playing = true;
  while(_playing)
  {
    for(QString file_name: _file_names)
    {
      
      file->setFileName(file_name);
      if(!file->open(QIODevice::ReadOnly))
      {
        emit fileReadError(file_name, file->errorString());
        _playing = false;
        break;
      }
      
      emit fileReaded(file_name);
      
      /** формируем датаграмму **/
      file->seek(0);
      while(!file->atEnd() && _playing)
      {
        memset(_buffer, 0, buferPointCount * sizeof(float));
        file->read((char*)(_buffer), buferPointCount * sizeof(float));
        
        emit dataUpdated();
        
        msleep(quint64(1000 / (dataSampling / buferPointCount) * sizeof(float)));
        
      }
      
      if(file->isOpen()) file->close();
    
    }
    
    if(!_repeat) {
      _playing = false;
      break;
    }
  }
  
  if(file->isOpen()) file->close();
  delete file;
  
  _finished = true;  
  qDebug() << "recovery finished";  
  
}


/** имитатор гидроакустики **/
SvGAFile2Udp::SvGAFile2Udp(QStringList *file_names,
                           bool repeat,
                           QString ip,
                           quint16 port,
                           QObject *parent)
{
  setParent(parent);
  
  _ip = ip;
  _port = port;
  _repeat = repeat;
  _file_names = *file_names;
  
}

void SvGAFile2Udp::run()
{
  QUdpSocket* socket = new QUdpSocket();
  
  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
  QFile* file = new QFile();
  qint64 datagram_size;
  
  _playing = true;
  while(_playing)
  {
    for(QString file_name: _file_names)
    {
      
      file->setFileName(file_name);
      if(!file->open(QIODevice::ReadOnly))
      {
        emit fileReadError(file_name, file->errorString());
        _playing = false;
  //        qDebug() << file->errorString();
        break;
      }
      
//      qDebug() << file_name;
      emit fileReaded(file_name);
      
      /** формируем датаграмму **/
      file->seek(0);
      while(!file->atEnd() && _playing)
      {
        // вычитываем заголовок
        datagram_size = file->read((char*)(datagram), buferPointCount * sizeof(float));
        
        socket->writeDatagram((const char*)(datagram), datagram_size, QHostAddress(_ip), _port);
        
        msleep(quint64(1000 / (dataSampling / buferPointCount) * sizeof(float) * 2));
        
      }
      
      if(file->isOpen()) file->close();
      
      if(!_playing) break;
    
    }
    
    if(!_repeat) {
      _playing = false;
      break;
    }
  }
  
  if(file->isOpen()) file->close();
  delete file;
  
  free(datagram);
  
  _finished = true;  
  qDebug() << "translation finished"; 
  
}

