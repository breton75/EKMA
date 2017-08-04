#include "sv_cleanupthread.h"

//#include "../../svlib/sv_fnt.h"

extern SvSQLITE *SQLITE;

/** *******  удаление устаревших файлов  ************ **/
SvCleanupThread::SvCleanupThread(SvSQLITE *sqlite, int cleanup_period, QObject *parent)
{
  setParent(parent);
  _sqlite = sqlite;
  _cleanup_period = cleanup_period;
}

void SvCleanupThread::run()
{
  _playing = true;
  _finished = false;

  QSqlQuery* q = new QSqlQuery(_sqlite->db);
  
  while(_playing)
  {
    QString query1 = QString("select filename from files where date(dateend) < date('now', '-%1 days')")
                    .arg(_cleanup_period);
    
    if(QSqlError::NoError != _sqlite->execSQL(query1, q).type())
    {
      qInfo() << q->lastError().databaseText();
      q->finish();
      continue;
    }
    
    while(q->next()) {
      QString filename = q->value("filename").toString();
      
      QFile f(filename);
      if(!f.remove())
        qInfo() << f.errorString();
      
    }
    
    QString query2 = QString("delete from files where date(dateend) < date('now', '-%1 days')")
                    .arg(_cleanup_period);
    
    if(QSqlError::NoError != _sqlite->execSQL(query2).type())
    {
      qInfo() << _sqlite->db.lastError().databaseText();
      continue;
    }
    
    msleep(1000);
    
  }

  delete q;
  _finished = true;  
  qDebug() << "cleanup finished";  
  
}
