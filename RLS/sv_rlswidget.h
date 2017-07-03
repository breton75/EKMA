#ifndef SVRLWIDGET_H
#define SVRLWIDGET_H

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
//#include <QSqlQuery>
//#include <QSqlError>

#include <QVBoxLayout>
#include <QDialog>
#include <qmath.h>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>

#include "sv_rls2bitthread.h"
#include "../../Common/sv_settings.h"

#define AZIMUTHS_COUNT 4096

class SvRlsPainter;

class SvRlsWidget: public QWidget
{
  Q_OBJECT
  
public:
  explicit SvRlsWidget(void *buffer,
                       QString caption,
                       int window_width = 640,
                       int window_height = 640,
                       int img_resolution = 640,
                       int draw_points_per_line = MAX_LINE_SIZE);
  
  ~SvRlsWidget();
  
public slots:
  void drawLine(int lineNum);
  
private:
  QHBoxLayout *_hLayMain;
  SvRlsPainter *_rls_painter;
  QSlider *_vSlider;
  
  void *_buffer;
  QImage _img;
  
  QString _caption;
  float _angle;
  int _draw_points_per_line;

  int _img_resolution;
  
};


class SvRlsPainter : public QWidget
{
  Q_OBJECT
  
  public:
    
    SvRlsPainter(QString caption,
                    QImage *img,
                    int draw_points_per_line = MAX_LINE_SIZE);
    
    ~SvRlsPainter() { deleteLater(); }
    
    
  private:
    QImage *_img;
    
    QString _caption;
    QTimer _timer;
    float _angle;
    int _draw_points_per_line;

    int _img_resolution;
        
  protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    
  public slots:
    void setResolution(int resolution) { _draw_points_per_line = resolution; }
    
};

#endif // SVRLWIDGET_H
