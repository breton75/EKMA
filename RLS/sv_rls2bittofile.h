#ifndef SVRL2BITSTOFILE_H
#define SVRL2BITSTOFILE_H

#include <QWidget>
#include <QObject>
#include <QUdpSocket>
#include <QIODevice>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QThread>
#include <QDateTime>
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QTime>
//#include <QSlider>
#include <QVBoxLayout>
#include <QDialog>
#include <qmath.h>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

#define Header2bitSize 13
#pragma pack(push,1)
struct Header2bit {
   quint8 dollarChar;  // символ '$' (0x24 = 36)
      int latitude;    // широта места установки радара
      int longitude;   // долгота места установки радара
  quint16 linesCount;    
  quint16 reserved; 
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

class SvRlsDataToFile : public QThread
{
    Q_OBJECT
  public:
    explicit SvRlsDataToFile(quint16 port,
                             int rotation_speed,
                             QString path_to_save_file,
                             QString device_name = "",
                             QString user_name = "",
                             QTime file_duaration = QTime(1, 0, 0), // один час
                             QTime total_duration = QTime(0, 0, 0),
                             QString datetime_format = "ddMMyy_hhmmss",
                             QString file_name_format = "<DATETIME>_<DEVICE_NAME>_<USER_NAME>",
                             QObject *parent = 0,
                             QString out_ip = "169.254.1.2",
                             quint16 out_port = 8000);
    
    ~SvRlsDataToFile();
    
    QDateTime cur_file_date_time;
    Header2bit cur_file_datagram_header;   
    QString file_name;
    
    bool isPlaying() { return _playing; }

  private:
    quint16 port;
        int rotation_speed;
    int linesCount;
    bool _playing = false;
    
    QString user_name;
//    QString password;
    QString path_to_save_file;
    QString device_name;
    QString datetime_format;
    QString file_name_format;
    
    QTime   file_duaration;
    QTime   total_duaration;
    qint32 file_duration_sec;
    qint32 total_duration_sec;
    
    quint16 out_port;
    QString out_ip;
    
    QFile* file = NULL;
    QDataStream* out = NULL;
    bool new_file;
    bool new_header;
    
  protected:
    void run() Q_DECL_OVERRIDE;
    
  signals:
//    void lineUpdated(int lineNum);
    void NewFile();
    void NewHeader();
    void TotalTimeElapsed();
    void TimeElapsed();
    void finished();
    
  public slots:
    void stop() { _playing = false; }
    
  private slots:
//    void readPendingDatagrams();

    
};

class SvRls2bitFile2Udp : public QThread
{
  Q_OBJECT
    
  public:
    SvRls2bitFile2Udp(QString ip,
                      quint16 port,
                      QDateTime begin_date_time,
                      QDateTime end_date_time,
                      int device_id);
    
    ~SvRls2bitFile2Udp();

    bool isPlaying() { return _playing; }
    QString CurrentFileName() { return current_file_name; }
    
  private:
    void run() Q_DECL_OVERRIDE;
    
    QSqlQuery* query;
    
    QString ip;
    quint16 port;
    QDateTime begin_date_time;
    QDateTime end_date_time;
    int device_id;
    
    QString current_file_name;
    bool _playing = false;
    
  signals:
    void NoRecords();
    void NewFile();
    void WrongFile();
    
  public slots:
    void stop() { _playing = false; }
};

#endif // SVRL2BITSTOFILE_H
