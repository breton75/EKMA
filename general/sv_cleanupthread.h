#ifndef SV_CLEANUPTHREAD_H
#define SV_CLEANUPTHREAD_H

#include <QDebug>
#include <QThread>
#include <QObject>
#include <QTime>
#include <QFile>
#include <QDir>
#include "time.h"
#include <QDataStream>

#include "../../svlib/sv_sqlite.h"

class SvCleanupThread : public QThread
{
    Q_OBJECT
  public:
    explicit SvCleanupThread(SvSQLITE *sqlite, int cleanup_period, QObject *parent = 0);
    
    ~SvCleanupThread() { deleteLater(); }
    
    
    bool isPlaying() { return _playing; }
//    bool isFinished() { return _finished; }

  private:
    SvSQLITE *_sqlite = nullptr;
    int _cleanup_period;
    
    bool _playing = false;
    bool _finished = false;
    
  protected:
    void run() Q_DECL_OVERRIDE;
    
  public slots:
    void stop() { _playing = false; }

    
};


#endif // SV_CLEANUPTHREAD_H
