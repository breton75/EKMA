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
#include <QConicalGradient>
#include <QMessageBox>
#include <QPair>
#include <QFileDialog>

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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QWidget>

#include "sv_rls2bitthread.h"
#include "../../svlib/sv_settings.h"

namespace svrlswdg {

  enum {
    udp = 0,
    archive
  };

}

struct SvRlsWidgetParams {
  int source = -1;
  qint32 ip = 0;
  qint16 port = 0;
  QDate fromdate = QDate();
  QTime fromtime = QTime();
//  int radius = -1;
  QColor painter_bkgnd_color = QColor();
  QColor painter_data_color = QColor();
  
  bool start_on_create = true;
  
//  int window_width = 640;
//  int window_height = 640;
  int display_point_count = 640;
  int line_point_count = MAX_LINE_POINT_COUNT;
  
  bool no_controls = false;
  bool autostart = true;
  
  int grid_line_count = 12;
  
  QString archive_path = "";
  
};

//class SvRlsPainter;


class SvRlsPainter : public QWidget
{
  Q_OBJECT
  
  public:
    
    SvRlsPainter(void *buffer, const SvRlsWidgetParams *params);
    
    ~SvRlsPainter() { _timer.stop(); deleteLater(); }
    
    void setBuffer(void *buffer) { _buffer = buffer; }
    
    void clear() { _img.fill(_params->painter_bkgnd_color); }
    
public slots:
    void drawLine(int lineNum, quint16 discret);
    
  private:
    void *_buffer = nullptr;
    QImage _img;
   
    QTimer _timer;
    
    qreal _radius;
    
    const SvRlsWidgetParams *_params;
    
    quint16 _current_discret = 0;
    
    qreal _data_angle_step;
    qreal _indicator_angle_step;
    
    QPointF _indicator_center;
    
    QConicalGradient _indicator;
    
        
  protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    
  
};


class SvRlsWidget: public QWidget
{
  Q_OBJECT
  
public:
  explicit SvRlsWidget(void *buffer, SvRlsWidgetParams &params);
  
  ~SvRlsWidget() { deleteLater(); }
  
  SvRlsPainter *painter() { return _rls_painter; }
  SvRlsWidgetParams params() { return _params; }
  
public slots:
  void startedUdp();
  void stoppedUdp();
  void startedArchive();
  void stoppedArchive();
  void fileReaded(QString filename);
  
signals:
  void start_stop_udp_clicked(quint32 ip, quint16 port);
  void start_stop_archive_clicked(QStringList *files);
//  void stop_clicked();
  
private slots:
  void on__bnStartStopUDP_clicked();
  void on__bnStartStopArchive_clicked();
  void on__sliderLinePointCount_valueChanged(int line_point_count) { _params.line_point_count = line_point_count; _rls_painter->clear(); }
  
  void on__cbPaintBkgndColor_currentIndexChanged(int index) { _params.painter_bkgnd_color = QColor(_cbPaintBkgndColor->itemText(index)); _rls_painter->clear(); }
  void on__cbPaintDataColor_currentIndexChanged(int index) { _params.painter_data_color = QColor(_cbPaintDataColor->itemText(index));   _rls_painter->clear(); }
  
  void on__cbDataSource_currentIndexChanged(int index);
  
  void on__bnSelectArchiveFiles_clicked();
  
private:
  SvRlsWidgetParams _params;
  void *_buffer = nullptr;
  
  QString _caption;
  qreal _angle_step;
  qreal _current_angle = 0;
  int _current_line_num = 0;
  
  int _draw_points_per_line;

  QStringList _arch_files = QStringList();
  
  void _setupUI();
  
  /** *********  Элементы управления *********** **/
  QVBoxLayout *vlayMain;
  QHBoxLayout *_hlayMain;

  SvRlsPainter *_rls_painter;
  
  QVBoxLayout *_vlayControls;
  QPushButton *_bnStartStopUDP;
  QGroupBox *_gbParams;
  QVBoxLayout *_vlayParams;
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
  QPushButton *_bnSelectArchiveFiles;
  QHBoxLayout *_hlayCurrentArchiveFile;
  QLabel *_labelCurrentArchiveFile;
  QLineEdit *_editCurrentArchiveFile;
  QCheckBox *_checkRepeatArchiveFiles;
  QPushButton *_bnStartStopArchive;
  QGroupBox *_gbPaintParams;
  QVBoxLayout *_vlayPaintParams;
  QHBoxLayout *_hlayLinePointCount;
  QLabel *_labelLinePointCount;
  QSlider *_sliderLinePointCount;
  QHBoxLayout *_hlayPaintBkgndColor;
  QLabel *_labelPaintBkgndColor;
  QComboBox *_cbPaintBkgndColor;
  QHBoxLayout *_hlayPaintDataColor;
  QLabel *_labelPaintDataColor;
  QComboBox *_cbPaintDataColor;
  QSpacerItem *_verticalSpacer;
  
  
};



#endif // SVRLWIDGET_H
