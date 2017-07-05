#ifndef SV_RLS2BITTHREAD_H
#define SV_RLS2BITTHREAD_H

#include <QDebug>
#include <QThread>
#include <QUdpSocket>
#include <QObject>
#include <QApplication>

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

#define MAX_LINE_POINT_COUNT 1460

#pragma pack(push,1)
struct Line2bit
{
  quint32 lineLen;  
  quint32 lineNum;
  quint16 discret;
  quint64 lineDT;
};
#pragma pack(pop)


class SvRls2bitThread : public QThread
{
    Q_OBJECT
  public:
    explicit SvRls2bitThread(void* buffer,
                             quint32 ip,
                                quint16 port,
                                QObject *parent = 0);
    
    ~SvRls2bitThread();
    
    void* buffer;
    int* firstLineNum;
    int* lastLineNum;
    
    bool isPlaying() { return _playing; }
//    bool isFinished() { return _finished; }

  private:
    QUdpSocket *socket;
    
    quint32 _ip;
    quint16 _port;
//    QByteArray *datagram;
    int _linesCount;
    int _pointPerLine;
    int _bitPerPoint;
    bool _playing = false;
    bool _finished = false;
    
  protected:
    void run() Q_DECL_OVERRIDE;
    
  signals:
    void lineUpdated(int lineNum, quint16 discret);
    
  public slots:
    void stop();
    
//  private slots:
//    void readPendingDatagrams();

    
};

#endif // SV_RLS2BITTHREAD_H
