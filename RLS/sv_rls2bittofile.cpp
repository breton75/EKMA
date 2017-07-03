#include "sv_rls2bittofile.h"
#include <time.h>

#include "./COMMON/sv_settings.h"
#include "../demo_1/COMMON/sv_pgdb.h"

extern SvPGDB* PGDB;
  
#define AZIMUTHS_COUNT 4096

SvRlsDataToFile::SvRlsDataToFile(quint16 port,
                                 int rotation_speed,
                                 QString path_to_save_file,
                                 QString device_name,
                                 QString user_name,
                                 QTime file_duaration, // один час
                                 QTime total_duration,
                                 QString datetime_format,
                                 QString file_name_format,
                                 QObject *parent,
                                 QString out_ip,
                                 quint16 out_port) 
{
  this->setParent(parent);
//  this->ip = ip;
  this->port = port;
  this->rotation_speed = rotation_speed;
  this->path_to_save_file = path_to_save_file;
  this->device_name = device_name;
  this->user_name = user_name; 
  this->datetime_format = datetime_format;
  this->file_name_format = file_name_format;
  
  this->file_duaration = file_duaration;
  this->total_duaration = total_duration;
  
  this->file_duration_sec = file_duaration.hour() * 3600 + 
                             file_duaration.minute() * 60 + 
                             file_duaration.second();
  
  this->total_duration_sec = total_duration.hour() * 3600 + 
                             total_duration.minute() * 60 + 
                             total_duration.second();
  
  this->out_ip = out_ip;
  this->out_port = out_port;
  
}

SvRlsDataToFile::~SvRlsDataToFile()
{
  this->deleteLater();
}

void SvRlsDataToFile::run()
{
  _playing = true;
  time_t fileTimeWatcher;
  time_t totalTimeWatcher;
  
  QUdpSocket* socket = new QUdpSocket();
  socket->bind(port);
  
  QUdpSocket* socket_out = new QUdpSocket();
//  socket_out->

  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
//  Header2bit* header = new Header2bit;
  Line2bit* line_head = new Line2bit;
  
  void* line_data;

  this->new_file = true;
//  timeWatcher.start();
  totalTimeWatcher = time(NULL);
  fileTimeWatcher = totalTimeWatcher;
  
  
  
  qint64 datagram_size;
  qint64 aval;
  int offset = 0;

  while(_playing)
  {
    /**** проверяем, задано ли полное время записи *****/
    if(this->total_duration_sec)
    {
      /* если задано полное время записи, то           *
      * проверяем не истекло ли полное время записи        *
      * если истекло, то выходим и закрываем текущий файл  */
   
      if(time(NULL) > totalTimeWatcher + this->total_duration_sec)
      {
        qDebug() << QString("Total time %1 spent")
                            .arg(this->total_duaration.toString("hh:mm:ss"));
        emit TotalTimeElapsed();
        break;
      }
    }
    
    /****** проверяем не истекло ли время записи текущего файла *****/
    if(time(NULL) > fileTimeWatcher + this->file_duration_sec)
    {
      /* close output */
      if(file) file->close();
      if(out) free(out);

      this->new_file = true;
      emit TimeElapsed();
    }
    
    /******** открываем новый файл для записи **********/
    if(this->new_file)
    {      
      cur_file_date_time = QDateTime::currentDateTime();
      
      file_name = QDir(this->path_to_save_file).canonicalPath() + "/" + this->file_name_format;
      file_name = file_name.replace("<DATETIME>", cur_file_date_time.toString(this->datetime_format));
      file_name = file_name.replace("<DEVICE_NAME>", this->device_name);
      file_name = file_name.replace("<USER_NAME>", this->user_name);
      file_name = file_name + ".bt2";
      
      file = new QFile(file_name);
      if(!file->open(QIODevice::WriteOnly))
      {
        qDebug() << "Не удалось открыть файл " << file_name;
        break;
      }
      out = new QDataStream(file);
      
      fileTimeWatcher = time(NULL);
      
      emit NewFile();
      new_file = false;
      new_header = true;
      qDebug() << file_name;
    }
    
    while (socket->waitForReadyRead(1))
    {
      while(socket->hasPendingDatagrams())
      {
        datagram_size = socket->pendingDatagramSize();
//        aval = socket->bytesAvailable();
        if(datagram_size <= 0) continue;
  
        socket->readDatagram((char*)(datagram), datagram_size);
        
        memcpy(&cur_file_datagram_header, datagram, Header2bitSize);
        offset = Header2bitSize;
        
        if(new_header) emit NewHeader();
        
        out->writeRawData((char*)(datagram), datagram_size);

        socket_out->writeDatagram((char*)(datagram), datagram_size, out_ip, out_port);
        
      }
    }
  }
  
  file->close();
  socket->close();  
  free(datagram);
  free(out);
  _playing = false;  
  
  emit finished();
  qDebug() << "finished";  
}


/*******************************************************/

SvRls2bitFile2Udp::SvRls2bitFile2Udp(QString ip,
                                     quint16 port,
                                     QDateTime begin_date_time,
                                     QDateTime end_date_time,
                                     int device_id)
{
  this->ip = ip;
  this->port = port;
  this->begin_date_time = begin_date_time;
  this->end_date_time = end_date_time;
  this->device_id = device_id;

}

SvRls2bitFile2Udp::~SvRls2bitFile2Udp()
{
  this->deleteLater();
}

void SvRls2bitFile2Udp::run()
{
  query = new QSqlQuery(PGDB->db);

  QSqlError err = PGDB->execSQL(QString("SELECT id, record_begin, "
                                        "record_end, device_id, "
                                        "file_name, rotation_speed FROM arch_rls "
                                        "where device_id=%1 and record_begin >= '%2' "
                                        "ORDER BY record_begin ASC")
                                .arg(device_id)
                                .arg(begin_date_time.toString("dd/MM/yyyy hh:mm:ss")), query);
  qDebug() << "query->size()" << query->size();
  if(!query->size())
  {
    emit NoRecords();
    return;
  }
//  return;
  QUdpSocket* socket = new QUdpSocket();
//  socket->open(QUdpSocket::WriteOnly);
  
  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
  Header2bit header;
  Line2bit line_header;
  
  QFile* file = new QFile();
  
  _playing = true;
  
  while(_playing  & query->next())
  {
    current_file_name = query->value("file_name").toString();
    int rotation_speed = query->value("rotation_speed").toInt();
    
    qDebug()<< current_file_name;
    
    file->setFileName(current_file_name);
    if(!file->open(QIODevice::ReadOnly))
    {
      emit WrongFile();
      break;
    }
    
//    emit NewFile();
    
    file->seek(0);
    
    while(!file->atEnd())
    {
      // вычитываем заголовок
      file->read((char*)(&header), Header2bitSize);
      memcpy(datagram, &header, Header2bitSize);
//      int slp = 0;
      int offset = Header2bitSize;
      for(int i = 0; i < header.linesCount; i++)
      {
        // вычитываем данные линейки
        file->read((char*)(&line_header), Line2bitHeaderSize);
        memcpy(datagram + offset, &line_header, Line2bitHeaderSize);
        offset += Line2bitHeaderSize;
//        qDebug() << "line_num=" << line_header.lineNum << "line_size=" << line_header.lineLen;
        
        file->read((char*)(datagram + offset), line_header.lineLen - Line2bitHeaderSize);
        offset += line_header.lineLen - Line2bitHeaderSize;
        
        usleep(rotation_speed * 1000000 / AZIMUTHS_COUNT);
        
      }
      
      socket->writeDatagram((const char*)(datagram), offset, QHostAddress(ip), port);
//      qDebug() << "slp" << slp;
//      usleep(header.linesCount * rotation_speed * 1000000 / AZIMUTHS_COUNT);
      if(!_playing) break;
      
    }
    file->close();
  }

  query->finish();
  free(query);
  
}

