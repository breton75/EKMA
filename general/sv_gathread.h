#ifndef SV_GATHREAD_H
#define SV_GATHREAD_H

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

#define buferPointCount 1000
#define dataSampling 48000

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


#define MAX_LINE_POINT_COUNT 48000

struct SvGAArchiverParams {
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

class SvGAThread : public QThread
{
    Q_OBJECT
  public:
    explicit SvGAThread(void* buffer,
                             quint32 ip,
                                quint16 port,
                                QObject *parent = 0);
    
    ~SvGAThread() { deleteLater(); }
    
    
    bool isPlaying() { return _playing; }
//    bool isFinished() { return _finished; }

  private:
    QUdpSocket *socket;
    
    void* _buffer;
    
    quint32 _ip;
    quint16 _port;

    bool _playing = false;
    bool _finished = false;
    
  protected:
    void run() Q_DECL_OVERRIDE;
    
  signals:
    void dataUpdated(quint64 pointCount);
    
  public slots:
    void stop() { _playing = false; }

    
};



class SvGAArchiver : public QThread
{
    Q_OBJECT
  public:
    explicit SvGAArchiver(SvGAArchiverParams *params, QObject *parent);
    
    ~SvGAArchiver() { deleteLater(); }
    
    bool isPlaying() { return _playing; }

  private:
    
    QUdpSocket* _socket;
    QUdpSocket* _socket_out;
    
    SvGAArchiverParams *_params;
    
    bool _playing = false;
        
    QFile* _file = nullptr;
    QDataStream* _out = nullptr;
    
    qint32 _file_duration_sec;
    qint32 _total_duration_sec;
    
    bool _new_file;
    bool _new_header;
    
    QString _ext = ".ga";
    
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


class SvGAExtractThread : public QThread
{
    Q_OBJECT
  public:
    explicit SvGAExtractThread(void* buffer,
                                QStringList *file_names,
                                bool repeat,
                                QObject *parent = 0);
    
    ~SvGAExtractThread() { deleteLater(); }
    
    
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
    void dataUpdated();
    void fileReaded(QString filename);
    void fileReadError(QString filename, QString error);
    
  public slots:
    void stop() { _playing = false; }

    
};


class SvGAFile2Udp : public QThread
{
  Q_OBJECT
    
  public:
    SvGAFile2Udp(QStringList *file_names,
                 bool repeat,
                 QString ip,
                 quint16 port,
                 QObject *parent = 0);
    
    ~SvGAFile2Udp() { deleteLater(); }

    bool isPlaying() { return _playing; }
    
  private:
    void run() Q_DECL_OVERRIDE;
    
    QString _ip;
    quint16 _port;
    QStringList _file_names;
    bool _repeat;
    
    bool _playing = false;
    bool _finished = false;
    
  signals:
    void fileReaded(QString filename);
    void fileReadError(QString filename, QString error);
    
  public slots:
    void stop() { _playing = false; }
    
};


#endif // SV_GATHREAD_H