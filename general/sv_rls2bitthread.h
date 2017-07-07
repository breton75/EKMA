#ifndef SV_RLS2BITTHREAD_H
#define SV_RLS2BITTHREAD_H

#include <QDebug>
#include <QThread>
#include <QUdpSocket>
#include <QObject>
#include <QTime>
#include <QFile>
#include <QDir>
#include "time.h"
#include <QDataStream>



#pragma pack(push,1)
struct StandartRLS {
      uchar dollarChar;  // символ '$' (0x24 = 36)
        int latitude;    // широта места установки радара
        int longitude;   // долгота места установки радара
    quint16 discret;    // цена одного дискрета в мм
    quint16 lineNum;    // номер линейки 0..4095
    quint16 lineLen;    // размер линейки 0..4104
      uchar reserve;     // всегда 0
      uchar checkSum1;   // контрольная сумма 1 (байты 0-15 с переполнением)
      uchar checkSum2;   // контрольная сумма 2 (байты 0-16 с переполнением
    quint64 lineDT;     // время прихода линейки, UTC
    // дальше - собственно линейка данных
};
#pragma pack(pop)

#define Header2bitSize 13
#pragma pack(push,1)
struct Header2bit {
   quint8 dollarChar;  // символ '$' (0x24 = 36)
      int latitude;    // широта места установки радара
      int longitude;   // долгота места установки радара
  quint32 reserved; 
};
#pragma pack(pop)

#define Line2bitHeaderSize 18
#pragma pack(push,1)
struct Line2bit
{
  quint32 lineLen;  
  quint32 lineNum;
  quint16 discret;
  quint64 lineDT;
};
#pragma pack(pop)


#define MAX_LINE_POINT_COUNT 1460
#define AZIMUTHS_COUNT 4096

struct SvRlsArchiverParams {
  QString ip = "127.0.0.1";
  quint16 port = 8000;
  
  int rotation_speed;
  
  QString path = "archive";
  QString device_name = "rls";
  QTime file_duration = QTime(1, 0, 0); // один час
  QTime total_duration = QTime(0, 0, 0);
  QString date_time_format = "ddMMyy_hhmmss";
  QString file_name_template = "{DEVICE}_{DATETIME}";

  QString out_ip = "127.0.0.1";
  quint16 out_port = 8001;
};

class SvRls2bitThread : public QThread
{
    Q_OBJECT
  public:
    explicit SvRls2bitThread(void* buffer,
                             quint32 ip,
                                quint16 port,
                                QObject *parent = 0);
    
    ~SvRls2bitThread() { deleteLater(); }
    
    void* buffer;
    
    bool isPlaying() { return _playing; }
//    bool isFinished() { return _finished; }

  private:
    QUdpSocket *socket;
    
    quint32 _ip;
    quint16 _port;

    bool _playing = false;
    bool _finished = false;
    
  protected:
    void run() Q_DECL_OVERRIDE;
    
  signals:
    void lineUpdated(int lineNum, quint16 discret);
    
  public slots:
    void stop() { _playing = false; }

    
};



class SvRlsArchiver : public QThread
{
    Q_OBJECT
  public:
    explicit SvRlsArchiver(SvRlsArchiverParams *params, QObject *parent);
    
    ~SvRlsArchiver() { deleteLater(); }
    
    bool isPlaying() { return _playing; }

  private:
    
    QUdpSocket* _socket;
    QUdpSocket* _socket_out;
    
    SvRlsArchiverParams *_params;
    
    bool _playing = false;
        
    QFile* _file = nullptr;
    QDataStream* _out = nullptr;
    
    qint32 _file_duration_sec;
    qint32 _total_duration_sec;
    
    bool _new_file;
    bool _new_header;
    
    QString _ext = ".bt2";
    
  protected:
    void run() Q_DECL_OVERRIDE;
    
  signals:
    void NewFile();
    void NewHeader();
    void TotalTimeElapsed();
    void FileTimeElapsed();
    
  public slots:
    void stop() { _playing = false; }

    
};


class SvRlsExtractThread : public QThread
{
    Q_OBJECT
  public:
    explicit SvRlsExtractThread(void* buffer,
                                QStringList *file_names,
                                bool repeat,
                                QObject *parent = 0);
    
    ~SvRlsExtractThread() { deleteLater(); }
    
    
    bool isPlaying() { return _playing; }

  private:
    
    void* _buffer;
    QStringList _file_names;
    bool _repeat;

    bool _playing = false;
    bool _finished = false;
    
  protected:
    void run() Q_DECL_OVERRIDE;
    
  signals:
    void lineUpdated(int lineNum, quint16 discret);
    void NewFile();
    void WrongFile();
    
  public slots:
    void stop() { _playing = false; }

    
};


#endif // SV_RLS2BITTHREAD_H
