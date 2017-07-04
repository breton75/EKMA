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
#include <qmath.h>

//#include <QSqlQuery>
//#include <QSqlError>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "sv_rls2bitthread.h"
#include "../../Common/sv_settings.h"

#define AZIMUTHS_COUNT 4096

struct SvRlsWidgetParams {
  int source = -1;
  qint32 ip = 0;
  qint16 port = 0;
  QDate fromdate = QDate();
  QTime fromtime = QTime();
  int radius = -1;
  QColor painter_bkgnd_color = QColor();
  QColor painter_data_color = QColor();
  
  bool start_on_create = true;
  
  int window_width = 640;
  int window_height = 640;
  int img_resolution = 640;
  int draw_points_per_line = MAX_LINE_SIZE;
  
  bool no_controls = false;
  bool autostart = true;
  
};

class SvRlsPainter;

class SvRlsWidget: public QWidget
{
  Q_OBJECT
  
public:
  explicit SvRlsWidget(void *buffer, SvRlsWidgetParams &params);
  
  ~SvRlsWidget();
  
public slots:
  void drawLine(int lineNum);
  void started();
  void stopped();
  
  
signals:
  void start_stop_clicked(quint32 ip, quint16 port);
//  void stop_clicked();
  
private slots:
  void on__bnStartStop_clicked();
  
private:
  SvRlsWidgetParams _params;
  
  void *_buffer;
  QImage _img;
  
  QString _caption;
  float _angle;
  int _draw_points_per_line;

  int _img_resolution;
  
  void _setupUI();
  
  /** *********  Элементы управления *********** **/
  QVBoxLayout *vlayMain;
  QHBoxLayout *_hlayMain;

  SvRlsPainter *_rls_painter;
  
  QVBoxLayout *_vlayControls;
  QPushButton *_bnStartStop;
  QGroupBox *_gbParams;
  QVBoxLayout *vlayParams;
  QHBoxLayout *_hlayDataSource;
  QLabel *_labelDataSource;
  QComboBox *_cbDataSource;
  QGroupBox *_gbNetworkParams;
  QVBoxLayout *_vlayNetworkParams;
  QHBoxLayout *_hlayIp;
  QLabel *_labelIp;
  QLineEdit *_editIp;
  QHBoxLayout *_hlayPort;
  QLabel *_labelPort;
  QSpinBox *_spinPort;
  QGroupBox *_gbArchParams;
  QVBoxLayout *_vlayArchParams;
  QHBoxLayout *_hlayArchDateBegin;
  QLabel *_labelArchDateBegin;
  QDateEdit *_dateArchBegin;
  QHBoxLayout *_hlayArchTimeBegin;
  QLabel *_labelArchTimeBegin;
  QTimeEdit *_timeArchBegin;
  QGroupBox *_gbPaintParams;
  QVBoxLayout *_vlayPaintParams;
  QHBoxLayout *_hlayPaintRadius;
  QLabel *_labelPaintRadius;
  QSlider *_sliderPaintRadius;
  QHBoxLayout *_hlayPaintBkgndColor;
  QLabel *_labelPaintBkgndColor;
  QComboBox *_cbPaintBkgndColor;
  QHBoxLayout *_hlayPaintDataColor;
  QLabel *_labelPaintDataColor;
  QComboBox *_cbPaintDataColor;
  QSpacerItem *_verticalSpacer;
  
  
};


class SvRlsPainter : public QWidget
{
  Q_OBJECT
  
  public:
    
    SvRlsPainter(QImage *img,
                 int draw_points_per_line = MAX_LINE_SIZE);
    
    ~SvRlsPainter() { _timer.stop(); deleteLater(); }
    
    
  private:
    QImage *_img;
    
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
