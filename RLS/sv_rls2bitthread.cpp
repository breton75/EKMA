#include "sv_rls2bitthread.h"

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

SvRls2bitThread::~SvRls2bitThread()
{
//  stop();
  deleteLater();
}

void SvRls2bitThread::stop() 
{
  _playing = false;
}

void SvRls2bitThread::run()
{
  _playing = true;
  _finished = false;
  
//  QUdpSocket* 
  socket = new QUdpSocket();
  socket->bind(QHostAddress(_ip), _port);
  
  void* datagram = malloc(0xFFFF); // максимальный возможный размер датаграммы
  
  Header2bit* header = new Header2bit;
  Line2bit* line_head = new Line2bit;
  
  void* line_data;

//  QFile file("e:/out2.rls");
//  if (!file.open(QIODevice::WriteOnly))
//    return;
//  QDataStream out(&file);

  qint64 datagram_size;
  qint64 aval;
  
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
//          line_head->discret
//          qDebug() << "line_len=" << line_head->lineLen
//                   << "line_data_size=" << line_data_size;
          
          int i = 0;
          int cnt = 0;
          void* data = buffer + line_head->lineNum * MAX_LINE_SIZE;
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
            
           emit lineUpdated(line_head->lineNum);
          
        }
      }
    }
  }
  
  free(datagram);
  socket->close();
  _finished = true;  
  qDebug() << "finished";  
  
}
