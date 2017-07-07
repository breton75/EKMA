#include "sv_rls2bitthread.h"

#include "../../Common/sv_fnt.h"

/** *******  данные от РЛС  ************ **/
SvRls2bitThread::SvRls2bitThread(void *buffer,
                                 quint32 ip,
                                 quint16 port,
                                 QObject *parent) 
{
  setParent(parent);
  
  _ip = ip;
  _port = port;
  
  this->buffer = buffer;
}

void SvRls2bitThread::run()
{
  _playing = true;
  _finished = false;
  
  socket = new QUdpSocket();
  socket->bind(QHostAddress(_ip), _port);
  
  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
  Header2bit* header = new Header2bit;
  Line2bit* line_head = new Line2bit;
  
  void* line_data;

  qint64 datagram_size;
  
  int offset = 0;
  while(_playing)
  {
    /** Note: This function may fail randomly on Windows!!! **/
    while (socket->waitForReadyRead(1)) // да! именно 1 мс.
    {
      while(socket->hasPendingDatagrams())
      {
        datagram_size = socket->pendingDatagramSize();
//        aval = socket->bytesAvailable();
        if(datagram_size <= 0 || datagram_size > 0xFFFF) continue;

        socket->readDatagram((char*)(datagram), datagram_size);
        
        memcpy(header, datagram, Header2bitSize);
        offset = Header2bitSize;
        
        while(offset < datagram_size)
        {
          memcpy(line_head, datagram + offset, Line2bitHeaderSize);
          offset += Line2bitHeaderSize;
          
          int line_data_size = line_head->lineLen - Line2bitHeaderSize;
          line_data = datagram + offset; // указатель на начало данных текущей линейки
          
          offset += line_data_size; // передвигаем указатель на следующую линейку
          
          line_head->lineNum = line_head->lineNum & 0xFFFF;
          
          int i = 0;
          int cnt = 0;
          void* data = buffer + line_head->lineNum * MAX_LINE_POINT_COUNT;
          while(i < line_data_size)
          {
            quint8* ch = (quint8*)(line_data + i);
            
            switch (*ch)
            {
              case 0x01:
                cnt += 1; // если ноль, то ничего не делаем. в массиве уже нули
                  i += 1;
//                  qDebug() << "case 0x01:" << "data + cnt * 4=" << cnt * 4;
                break;
                
              case 0x54:
              case 0xA9:
              case 0xFE:  
                *ch += 1;
                memset(data + cnt, *ch, 1);
                cnt += 1;
                  i += 1;
//                  qDebug() << "case 0x54:" << "data + cnt * 4=" << cnt * 4;
                break;
                
              case 0x00:
              case 0x55:
              case 0xAA:
              case 0xFF:
              {
                quint8* c = (quint8*)(line_data + i + 1);
                memset(data + cnt, *ch, *c);
                cnt += *c;
                  i += 2;
//                  qDebug() << "case 0x00:" << "c=" << int(*c) << "data + cnt * 4=" << cnt * 4;
                break;
              }
                
              default:
              {
                memset(data + cnt, *ch, 1);
                cnt += 1;
                  i += 1;
//                  qDebug() << "case dflt:" << "data + cnt * 4=" << cnt * 4 ;
                break;
              }
            }
          }
            
           emit lineUpdated(line_head->lineNum, line_head->discret);
          
        }
      }
    }
  }
  
  free(datagram);
  socket->close();
  _finished = true;  
  qDebug() << "udp finished";  
  
}

/** **********  АРХИВАЦИЯ ДАННЫХ РЛС  ********** **/
SvRlsArchiver::SvRlsArchiver(SvRlsArchiverParams *params,
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

void SvRlsArchiver::run()
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
        
        /* записываем в тело датаграммы, в поле Header2bit.reserved размер датаграммы,
         * для последующего восстановления данных из архива */
        Header2bit header;
        memcpy(&header, datagram, sizeof(Header2bit));
        header.reserved = datagram_size & 0xFFFF;
        memcpy(datagram, &header, sizeof(Header2bit));
        
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
SvRlsExtractThread::SvRlsExtractThread(void *buffer,
                                       QStringList *file_names,
                                       bool repeat,
                                       QObject *parent) 
{
  setParent(parent);
  
  _buffer = buffer;
  _repeat = repeat;
  _file_names = *file_names;
  
}

void SvRlsExtractThread::run()
{
  _playing = true;
  _finished = false;
  
  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
  Header2bit* header = new Header2bit;
  Line2bit* line_head = new Line2bit;
  
  void* line_data;

  quint64 rotation_speed = 8 * 1000000 / AZIMUTHS_COUNT;
  
  QFile* file = new QFile();
  
  qint64 offset = 0;
  qint64 datagram_size = 0;
  while(_playing)
  {
   
    for(QString file_name: _file_names)
    {
      
      file->setFileName(file_name);
      if(!file->open(QIODevice::ReadOnly))
      {
        emit WrongFile();
        break;
      }
      
      /** формируем датаграмму, как если бы считали ее по udp **/
      file->seek(0);
      while(!file->atEnd())
      {
        /* вычитываем заголовок, узнаем размер датаграммы и вычитываем всю датаграмму */
        file->read((char*)header, Header2bitSize);
        memcpy(datagram, header, Header2bitSize);
        datagram_size = header->reserved;
        
        file->read((char*)(datagram + Header2bitSize), datagram_size - Header2bitSize);
        
        /* задержка для синхронизации со скоростью вращения РЛС */
        usleep(rotation_speed);
        
        /* датаграмма считана. теперь разбираем ее как в SvRls2bitThread */
        offset = Header2bitSize;
        while(offset < datagram_size)
        {
          memcpy(line_head, datagram + offset, Line2bitHeaderSize);
          offset += Line2bitHeaderSize;
          
          int line_data_size = line_head->lineLen - Line2bitHeaderSize;
          line_data = datagram + offset; // указатель на начало данных текущей линейки
          
          offset += line_data_size; // передвигаем указатель на следующую линейку
          
          line_head->lineNum = line_head->lineNum & 0xFFFF;
          
          int i = 0;
          int cnt = 0;
          void* data = _buffer + line_head->lineNum * MAX_LINE_POINT_COUNT;
          while(i < line_data_size)
          {
            quint8* ch = (quint8*)(line_data + i);
            
            switch (*ch)
            {
              case 0x01:
                cnt += 1; // если ноль, то ничего не делаем. в массиве уже нули
                  i += 1;
    //                  qDebug() << "case 0x01:" << "data + cnt * 4=" << cnt * 4;
                break;
                
              case 0x54:
              case 0xA9:
              case 0xFE:  
                *ch += 1;
                memset(data + cnt, *ch, 1);
                cnt += 1;
                  i += 1;
    //                  qDebug() << "case 0x54:" << "data + cnt * 4=" << cnt * 4;
                break;
                
              case 0x00:
              case 0x55:
              case 0xAA:
              case 0xFF:
              {
                quint8* c = (quint8*)(line_data + i + 1);
                memset(data + cnt, *ch, *c);
                cnt += *c;
                  i += 2;
    //                  qDebug() << "case 0x00:" << "c=" << int(*c) << "data + cnt * 4=" << cnt * 4;
                break;
              }
                
              default:
              {
                memset(data + cnt, *ch, 1);
                cnt += 1;
                  i += 1;
    //                  qDebug() << "case dflt:" << "data + cnt * 4=" << cnt * 4 ;
                break;
              }
            }
          }
            
           emit lineUpdated(line_head->lineNum, line_head->discret);
          
        }
        
      }
      
    }
    
    if(!_repeat) {
      _playing = false;
      break;
    }
    
  }
  
  free(datagram);
  _finished = true;  
  qDebug() << "recovery finished";  
  
}
