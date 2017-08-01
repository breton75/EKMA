/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SV_GAWIDGET_H
#define SV_GAWIDGET_H

#include <QtCore/QTimer>
#include <QWidget>
#include <QMutex>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QSpinBox>
#include <QtCore/QTime>
#include <QtCore/QDebug>

#include "qcustomplot.h"

#include "sv_graph.h"

#include "sv_gathread.h"
#include "../../svlib/sv_settings.h"

namespace svgawdg {
  enum {
    udp = 0,
    archive
  };

  struct SvGAWidgetParams {
    int source = udp;
    qint32 ip = 0;
    qint16 port = 0;
    QColor painter_bkgnd_color = QColor();
    QColor painter_data_color = QColor();
    
    bool start_on_create = true;
    
    int display_point_count = 640;
    int buffer_point_count = MAX_BUFFER_POINT_COUNT;
    
    bool no_controls = false;
    bool autostart = true;
    
//    int grid_line_count = 12;
    
    QString archive_path = "";
    
    int x_range = 300;
    int x_tick_count = 26;
    qreal y_range = 1.0;
//    int y_tick_count = 11;
    bool y_autoscale = false;
    
  };

  struct GRAPH {
    QCPGraph* graph;
    svgraph::GraphParams params;
  };
  
  
  class SvGAPainter;
  class SvGAWidget;
  
}


class svgawdg::SvGAPainter: public QWidget
{
    Q_OBJECT

public:
    SvGAPainter(void *buffer, svgawdg::SvGAWidgetParams *params, QWidget *parent = 0);
    ~SvGAPainter() { close(); deleteLater(); }
    
    QCustomPlot *customplot() { return _customplot; }
    
    void setActualYRange();
    
    void setMaxMinY(qreal y) { if(y > _y_max) _y_max = y * 1.01; 
                               if(y < _y_min) _y_min = y * 1.01; }
    
    void setYRange(qreal lower, qreal upper) { _y_max = upper; _y_min = lower;
                                               _customplot->yAxis->setRange(lower, upper); }
   
    
    void addGraph(int graph_id, svgraph::GraphParams &graphParams);
    
    void removeGraph(int graph_id);
    
    bool findGraph(int graph_id) { return _graphs.find(graph_id) != _graphs.end(); }
    
    void setGraphParams(int graph_id, svgraph::GraphParams &graphParams);
    
    QList<int> graphList() { return _graphs.keys(); }
    
    int graphCount() { return _customplot->graphCount(); }
    
    void appendData(int graph_id, double y);
    
    void insertData(int graph_id, QCPData xy);
    
    svgraph::GraphParams graphParams(int graph_id) { return _graphs.value(graph_id)->params; }
    
    int pointCount() { return _customplot->graph()->data()->count(); }
    
    QMutex mutex;
    
    void setBuffer(void *buffer) { _buffer = buffer; }
    
//    void clear() { _img.fill(_params->painter_bkgnd_color); }
    
public slots:
    void setBuffersXY(int bufsize);
    void drawData(quint32 pointCount);
    
private:
    void *_buffer = nullptr;
   
    QTimer _timer;
    
    QCustomPlot *_customplot;
    QMap<int, svgawdg::GRAPH*> _graphs;
    svgawdg::SvGAWidgetParams *_params;
    
    qreal _y_max = -1000000000;
    qreal _y_min =  1000000000;
    
    QVector <double> x_data, y_data;
    
    /** виджеты **/
    void setupUi();
    
    QVBoxLayout *vlayMain;
    QHBoxLayout *hlay1;
    QPushButton *bnResetChart;
    QFrame *frameXRange;
    QHBoxLayout *hlayXRange;
    QSpacerItem *hspacer1;
    QPushButton *bnXRangeDown;
    QSpacerItem *hspacer2;
    QPushButton *bnXRangeActual;
    QSpacerItem *hspacer3;
    QSpinBox *spinXRange;
    QPushButton *bnXSetRange;
    QSpacerItem *hspacer4;
    QPushButton *bnXRangeUp;
    QSpacerItem *hspacer5;
    QHBoxLayout *hlay2;
    QFrame *frameYRange;
    QVBoxLayout *vlayYRange;
    QPushButton *bnYAutoscale;
    QSpacerItem *vspacer1;
    QPushButton *bnYRangeDown;
    QSpacerItem *vspacer2;
    QPushButton *bnYRangeActual;
    QSpacerItem *vspacer3;
    QPushButton *bnYRangeUp;
    QSpacerItem *vspacer4;
    
private slots:
    void on_bnXRangeUp_clicked();
    void on_bnXRangeDown_clicked();
    void on_bnXRangeActual_clicked();
    void on_bnXSetRange_clicked();
    void on_bnYRangeUp_clicked();
    void on_bnYRangeDown_clicked();
    void on_bnYRangeActual_clicked();
    void on_bnResetChart_clicked();
    void on_bnYAutoscale_clicked(bool checked);
    
    void replot();

};


class svgawdg::SvGAWidget: public QWidget
{
  Q_OBJECT
  
public:
  explicit SvGAWidget(void *buffer, svgawdg::SvGAWidgetParams &params);
  
  ~SvGAWidget() { deleteLater(); }
  
  svgawdg::SvGAPainter *painter() { return _ga_painter; }
  svgawdg::SvGAWidgetParams params() { return _params; }
  
  QSize painterSize() { return QSize(_ga_painter->width(), _ga_painter->height()); }
  
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
  void on__sliderLinePointCount_valueChanged(int val);
  
  void on__cbPaintBkgndColor_currentIndexChanged(int index) { 
    _params.painter_bkgnd_color = QColor(_cbPaintBkgndColor->itemText(index)); 
    _ga_painter->customplot()->setBackground(QBrush(_params.painter_bkgnd_color)); }
  
  void on__cbPaintDataColor_currentIndexChanged(int index) { 
    _params.painter_data_color = QColor(_cbPaintDataColor->itemText(index));  
    _ga_painter->customplot()->graph()->setPen(QPen(_params.painter_data_color));
    _ga_painter->customplot()->xAxis->setTickLabelColor(_params.painter_data_color);
    _ga_painter->customplot()->yAxis->setTickLabelColor(_params.painter_data_color);
  }
  
  void on__cbDataSource_currentIndexChanged(int index);
  
  void on__bnSelectArchiveFiles_clicked();
  
private:
  svgawdg::SvGAWidgetParams _params;
  void *_buffer = nullptr;
  
  QString _caption;

  QStringList _arch_files = QStringList();
  
  void _setupUI();
  
  /** *********  Элементы управления *********** **/
  QVBoxLayout *vlayMain;
  QHBoxLayout *_hlayMain;

  svgawdg::SvGAPainter *_ga_painter;
  
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
  QLabel *_labelDivider;
  QSlider *_sliderBufferPointCount;
  QHBoxLayout *_hlayPaintBkgndColor;
  QLabel *_labelPaintBkgndColor;
  QComboBox *_cbPaintBkgndColor;
  QHBoxLayout *_hlayPaintDataColor;
  QLabel *_labelPaintDataColor;
  QComboBox *_cbPaintDataColor;
  QSpacerItem *_verticalSpacer;
  
  
};


#endif /* SV_GAWIDGET_H */
