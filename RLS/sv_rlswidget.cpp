#include "sv_rlswidget.h"


//SvSingleRlsWindow* SINGLERLS_UI;
//SvSelectRlsArchWindow* SELECT_RSL_ARCH_UI;

SvRlsWidget::SvRlsWidget(void *buffer, SvRlsWidgetParams &params)
{
  _params = params;
  _buffer = buffer;
  
  resize(_params.display_point_count, _params.display_point_count);
  
  _setupUI();  
  

  QStringList colorNames = QColor::colorNames();

  for (int i = 0; i < colorNames.size(); ++i) {
      QColor color(colorNames[i]);

      _cbPaintBkgndColor->insertItem(i, colorNames[i]);
      _cbPaintBkgndColor->setItemData(i, color, Qt::DecorationRole);
      
      _cbPaintDataColor->insertItem(i, colorNames[i]);
      _cbPaintDataColor->setItemData(i, color, Qt::DecorationRole);
  }
  
  _cbDataSource->setCurrentIndex(_params.source);
  _editIp->setText(QHostAddress(_params.ip).toString());
  _spinPort->setValue(_params.port);
  _dateArchBegin->setDate(_params.fromdate);
  _timeArchBegin->setTime(_params.fromtime);
  _sliderLinePointCount->setValue(_params.line_point_count);

  _cbPaintBkgndColor->setCurrentIndex(_cbPaintBkgndColor->findData(_params.painter_bkgnd_color, Qt::DecorationRole));
  _cbPaintDataColor->setCurrentIndex(_cbPaintDataColor->findData(_params.painter_data_color, Qt::DecorationRole));
   
  _gbParams->setVisible(!_params.no_controls);
  
//  connect(_sliderLinePointCount, SIGNAL(valueChanged(int)), this, SLOT(setLinePointCount(int)));
  
  QMetaObject::connectSlotsByName(this);
  
}

SvRlsWidget::~SvRlsWidget()
{
  deleteLater();
}

void SvRlsWidget::_setupUI()
{
  this->setObjectName(tr("SvRlsWidget"));

  vlayMain = new QVBoxLayout(this);
  vlayMain->setObjectName(QStringLiteral("vlayMain"));
  _hlayMain = new QHBoxLayout();
  _hlayMain->setObjectName(QStringLiteral("_hlayMain"));
  
  _rls_painter = new SvRlsPainter(_buffer, &_params);
  _rls_painter->setParent(this);
  _rls_painter->setObjectName(QStringLiteral("_rls_painter"));
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(_rls_painter->sizePolicy().hasHeightForWidth());
  _rls_painter->setSizePolicy(sizePolicy);

  _hlayMain->addWidget(_rls_painter);

  _vlayControls = new QVBoxLayout();
  _vlayControls->setObjectName(QStringLiteral("_vlayControls"));
//  _bnStartStop = new QPushButton(this);
//  _bnStartStop->setObjectName(QStringLiteral("_bnStartStop"));
//  QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
//  sizePolicy1.setHorizontalStretch(0);
//  sizePolicy1.setVerticalStretch(0);
//  sizePolicy1.setHeightForWidth(_bnStartStop->sizePolicy().hasHeightForWidth());
//  _bnStartStop->setSizePolicy(sizePolicy1);

//  _vlayControls->addWidget(_bnStartStop);

  _gbParams = new QGroupBox(this);
  _gbParams->setObjectName(QStringLiteral("_gbParams"));
  QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(_gbParams->sizePolicy().hasHeightForWidth());
  _gbParams->setSizePolicy(sizePolicy2);
  vlayParams = new QVBoxLayout(_gbParams);
  vlayParams->setObjectName(QStringLiteral("vlayParams"));
  _hlayDataSource = new QHBoxLayout();
  _hlayDataSource->setObjectName(QStringLiteral("_hlayDataSource"));
  _labelDataSource = new QLabel(_gbParams);
  _labelDataSource->setObjectName(QStringLiteral("_labelDataSource"));
  sizePolicy2.setHeightForWidth(_labelDataSource->sizePolicy().hasHeightForWidth());
  _labelDataSource->setSizePolicy(sizePolicy2);
  _labelDataSource->setMinimumSize(QSize(65, 0));
  _labelDataSource->setMaximumSize(QSize(65, 16777215));
  _labelDataSource->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayDataSource->addWidget(_labelDataSource);

  _cbDataSource = new QComboBox(_gbParams);
  _cbDataSource->setObjectName(QStringLiteral("_cbDataSource"));

  _hlayDataSource->addWidget(_cbDataSource);


  vlayParams->addLayout(_hlayDataSource);

  _gbNetworkParams = new QGroupBox(_gbParams);
  _gbNetworkParams->setObjectName(QStringLiteral("_gbNetworkParams"));
  _vlayNetworkParams = new QVBoxLayout(_gbNetworkParams);
  _vlayNetworkParams->setObjectName(QStringLiteral("_vlayNetworkParams"));
  _hlayIp = new QHBoxLayout();
  _hlayIp->setObjectName(QStringLiteral("_hlayIp"));
  _labelIp = new QLabel(_gbNetworkParams);
  _labelIp->setObjectName(QStringLiteral("_labelIp"));
  sizePolicy2.setHeightForWidth(_labelIp->sizePolicy().hasHeightForWidth());
  _labelIp->setSizePolicy(sizePolicy2);
  _labelIp->setMinimumSize(QSize(65, 0));
  _labelIp->setMaximumSize(QSize(65, 16777215));
  _labelIp->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayIp->addWidget(_labelIp);

  _editIp = new QLineEdit(_gbNetworkParams);
  _editIp->setObjectName(QStringLiteral("_editIp"));

  _hlayIp->addWidget(_editIp);


  _vlayNetworkParams->addLayout(_hlayIp);

  _hlayPort = new QHBoxLayout();
  _hlayPort->setObjectName(QStringLiteral("_hlayPort"));
  _labelPort = new QLabel(_gbNetworkParams);
  _labelPort->setObjectName(QStringLiteral("_labelPort"));
  sizePolicy2.setHeightForWidth(_labelPort->sizePolicy().hasHeightForWidth());
  _labelPort->setSizePolicy(sizePolicy2);
  _labelPort->setMinimumSize(QSize(65, 0));
  _labelPort->setMaximumSize(QSize(65, 16777215));
  _labelPort->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayPort->addWidget(_labelPort);

  _spinPort = new QSpinBox(_gbNetworkParams);
  _spinPort->setObjectName(QStringLiteral("_spinPort"));
  _spinPort->setMinimum(1);
  _spinPort->setMaximum(32565);
  _spinPort->setValue(8000);

  _hlayPort->addWidget(_spinPort);


  _vlayNetworkParams->addLayout(_hlayPort);


  vlayParams->addWidget(_gbNetworkParams);

  _gbArchParams = new QGroupBox(_gbParams);
  _gbArchParams->setObjectName(QStringLiteral("_gbArchParams"));
  _vlayArchParams = new QVBoxLayout(_gbArchParams);
  _vlayArchParams->setObjectName(QStringLiteral("_vlayArchParams"));
  _hlayArchDateBegin = new QHBoxLayout();
  _hlayArchDateBegin->setObjectName(QStringLiteral("_hlayArchDateBegin"));
  _labelArchDateBegin = new QLabel(_gbArchParams);
  _labelArchDateBegin->setObjectName(QStringLiteral("_labelArchDateBegin"));
  sizePolicy2.setHeightForWidth(_labelArchDateBegin->sizePolicy().hasHeightForWidth());
  _labelArchDateBegin->setSizePolicy(sizePolicy2);
  _labelArchDateBegin->setMinimumSize(QSize(65, 0));
  _labelArchDateBegin->setMaximumSize(QSize(65, 16777215));
  _labelArchDateBegin->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayArchDateBegin->addWidget(_labelArchDateBegin);

  _dateArchBegin = new QDateEdit(_gbArchParams);
  _dateArchBegin->setObjectName(QStringLiteral("_dateArchBegin"));

  _hlayArchDateBegin->addWidget(_dateArchBegin);


  _vlayArchParams->addLayout(_hlayArchDateBegin);

  _hlayArchTimeBegin = new QHBoxLayout();
  _hlayArchTimeBegin->setObjectName(QStringLiteral("_hlayArchTimeBegin"));
  _labelArchTimeBegin = new QLabel(_gbArchParams);
  _labelArchTimeBegin->setObjectName(QStringLiteral("_labelArchTimeBegin"));
  sizePolicy2.setHeightForWidth(_labelArchTimeBegin->sizePolicy().hasHeightForWidth());
  _labelArchTimeBegin->setSizePolicy(sizePolicy2);
  _labelArchTimeBegin->setMinimumSize(QSize(65, 0));
  _labelArchTimeBegin->setMaximumSize(QSize(65, 16777215));
  _labelArchTimeBegin->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayArchTimeBegin->addWidget(_labelArchTimeBegin);

  _timeArchBegin = new QTimeEdit(_gbArchParams);
  _timeArchBegin->setObjectName(QStringLiteral("_timeArchBegin"));

  _hlayArchTimeBegin->addWidget(_timeArchBegin);


  _vlayArchParams->addLayout(_hlayArchTimeBegin);


  vlayParams->addWidget(_gbArchParams);

  _gbPaintParams = new QGroupBox(_gbParams);
  _gbPaintParams->setObjectName(QStringLiteral("_gbPaintParams"));
  _vlayPaintParams = new QVBoxLayout(_gbPaintParams);
  _vlayPaintParams->setObjectName(QStringLiteral("_vlayPaintParams"));
  _hlayLinePointCount = new QHBoxLayout();
  _hlayLinePointCount->setObjectName(QStringLiteral("_hlayLinePointCount"));
  _labelLinePointCount = new QLabel(_gbPaintParams);
  _labelLinePointCount->setObjectName(QStringLiteral("_labelLinePointCount"));
  sizePolicy2.setHeightForWidth(_labelLinePointCount->sizePolicy().hasHeightForWidth());
  _labelLinePointCount->setSizePolicy(sizePolicy2);
  _labelLinePointCount->setMinimumSize(QSize(65, 0));
  _labelLinePointCount->setMaximumSize(QSize(65, 16777215));
  _labelLinePointCount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayLinePointCount->addWidget(_labelLinePointCount);

  _sliderLinePointCount = new QSlider(_gbPaintParams);
  _sliderLinePointCount->setObjectName(QStringLiteral("_sliderLinePointCount"));
  _sliderLinePointCount->setMinimum(300);
  _sliderLinePointCount->setMaximum(1200);
  _sliderLinePointCount->setSingleStep(100);
  _sliderLinePointCount->setPageStep(100);
  _sliderLinePointCount->setValue(600);
  _sliderLinePointCount->setOrientation(Qt::Horizontal);
  _sliderLinePointCount->setTickPosition(QSlider::TicksBelow);
  _sliderLinePointCount->setTickInterval(100);
    
  _hlayLinePointCount->addWidget(_sliderLinePointCount);


  _vlayPaintParams->addLayout(_hlayLinePointCount);

  _hlayPaintBkgndColor = new QHBoxLayout();
  _hlayPaintBkgndColor->setObjectName(QStringLiteral("_hlayPaintBkgndColor"));
  _labelPaintBkgndColor = new QLabel(_gbPaintParams);
  _labelPaintBkgndColor->setObjectName(QStringLiteral("_labelPaintBkgndColor"));
  sizePolicy2.setHeightForWidth(_labelPaintBkgndColor->sizePolicy().hasHeightForWidth());
  _labelPaintBkgndColor->setSizePolicy(sizePolicy2);
  _labelPaintBkgndColor->setMinimumSize(QSize(65, 0));
  _labelPaintBkgndColor->setMaximumSize(QSize(65, 16777215));
  _labelPaintBkgndColor->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayPaintBkgndColor->addWidget(_labelPaintBkgndColor);

  _cbPaintBkgndColor = new QComboBox(_gbPaintParams);
  _cbPaintBkgndColor->setObjectName(QStringLiteral("_cbPaintBkgndColor"));

  _hlayPaintBkgndColor->addWidget(_cbPaintBkgndColor);


  _vlayPaintParams->addLayout(_hlayPaintBkgndColor);

  _hlayPaintDataColor = new QHBoxLayout();
  _hlayPaintDataColor->setObjectName(QStringLiteral("_hlayPaintDataColor"));
  _labelPaintDataColor = new QLabel(_gbPaintParams);
  _labelPaintDataColor->setObjectName(QStringLiteral("_labelPaintDataColor"));
  sizePolicy2.setHeightForWidth(_labelPaintDataColor->sizePolicy().hasHeightForWidth());
  _labelPaintDataColor->setSizePolicy(sizePolicy2);
  _labelPaintDataColor->setMinimumSize(QSize(65, 0));
  _labelPaintDataColor->setMaximumSize(QSize(65, 16777215));
  _labelPaintDataColor->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  _hlayPaintDataColor->addWidget(_labelPaintDataColor);

  _cbPaintDataColor = new QComboBox(_gbPaintParams);
  _cbPaintDataColor->setObjectName(QStringLiteral("_cbPaintDataColor"));

  _hlayPaintDataColor->addWidget(_cbPaintDataColor);


  _vlayPaintParams->addLayout(_hlayPaintDataColor);


  vlayParams->addWidget(_gbPaintParams);
  
  _bnStartStop = new QPushButton(this);
  _bnStartStop->setObjectName(QStringLiteral("_bnStartStop"));
  QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(_bnStartStop->sizePolicy().hasHeightForWidth());
  _bnStartStop->setSizePolicy(sizePolicy1);
  
  vlayParams->addWidget(_bnStartStop);
  
  _verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  vlayParams->addItem(_verticalSpacer);

  _vlayControls->addWidget(_gbParams);

  _hlayMain->addLayout(_vlayControls);

  vlayMain->addLayout(_hlayMain);


  // retranslateUi
  _bnStartStop->setText(QApplication::translate("Form", "\320\241\321\202\320\260\321\200\321\202", Q_NULLPTR));
  _gbParams->setTitle(QString());
  _labelDataSource->setText(QApplication::translate("Form", "\320\230\321\201\321\202.\320\264\320\260\320\275\321\213\321\205", Q_NULLPTR));
  
  _cbDataSource->clear();
  _cbDataSource->insertItems(0, QStringList()
   << QApplication::translate("Form", "\320\241\320\265\321\202\321\214", Q_NULLPTR)
   << QApplication::translate("Form", "\320\220\321\200\321\205\320\270\320\262", Q_NULLPTR)
  );
  
  _gbNetworkParams->setTitle(QApplication::translate("Form", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \321\201\320\265\321\202\320\270", Q_NULLPTR));
  _labelIp->setText(QApplication::translate("Form", "\320\220\320\264\321\200\320\265\321\201", Q_NULLPTR));
  _editIp->setText(QApplication::translate("Form", "127.0.0.1", Q_NULLPTR));
  _editIp->setPlaceholderText(QApplication::translate("Form", "0.0.0.0", Q_NULLPTR));
  _labelPort->setText(QApplication::translate("Form", "\320\237\320\276\321\200\321\202", Q_NULLPTR));
  
  _gbArchParams->setTitle(QApplication::translate("Form", "\320\220\321\200\321\205\320\270\320\262", Q_NULLPTR));
  _labelArchDateBegin->setText(QApplication::translate("Form", "\320\224\320\260\321\202\320\260", Q_NULLPTR));
  _labelArchTimeBegin->setText(QApplication::translate("Form", "\320\222\321\200\320\265\320\274\321\217", Q_NULLPTR));
  _gbPaintParams->setTitle(QApplication::translate("Form", "\320\236\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\320\265", Q_NULLPTR));
  _labelLinePointCount->setText(QApplication::translate("Form", "\320\224\320\273\320\270\320\275\320\260 \320\273\321\203\321\207\320\260", Q_NULLPTR));
  _labelPaintBkgndColor->setText(QApplication::translate("Form", "\320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260", Q_NULLPTR));
  _labelPaintDataColor->setText(QApplication::translate("Form", "\320\246\320\262\320\265\321\202 \320\237\320\240\320\233\320\230", Q_NULLPTR));
 // retranslateUi

  
}

void SvRlsWidget::on__bnStartStop_clicked()
{
  _bnStartStop->setEnabled(false);
  emit start_stop_clicked(_params.ip, _params.port);
}

void SvRlsWidget::started()
{
  _bnStartStop->setStyleSheet("background-color: tomato");
  _bnStartStop->setText("Стоп");  
  _bnStartStop->setEnabled(true);
}

void SvRlsWidget::stopped()
{
   _bnStartStop->setStyleSheet("");
   _bnStartStop->setText("Старт");
   _bnStartStop->setEnabled(true);
}

/** *************************************** **/

SvRlsPainter::SvRlsPainter(void *buffer, const SvRlsWidgetParams *params)
{
  _params = params;
  _buffer = buffer;
  
  _data_angle_step = double(2 * M_PI) / double(AZIMUTHS_COUNT);
  _indicator_angle_step = 360.0 / AZIMUTHS_COUNT; // 16 * 360
  
  _img = QImage(_params->display_point_count, _params->display_point_count, QImage::Format_RGB888);
  _img.fill(_params->painter_bkgnd_color);
  
  
  _radius = qreal(_params->display_point_count)/2;
  _indicator_center = QPointF(_radius, _radius);

  _indicator.setCenter(_indicator_center);
  _indicator.setColorAt(0.95, QColor(0, 0, 0, 0));
  _indicator.setColorAt(1, _params->painter_data_color);
  
  _timer.setInterval(20);
  _timer.setSingleShot(false);
  connect(&_timer, SIGNAL(timeout()), this, SLOT(repaint()));
  _timer.start();

}

void SvRlsPainter::drawLine(int lineNum, quint16 discret)
{
  if(!_buffer) return;
  
  void* d;
  d = _buffer + lineNum * MAX_LINE_POINT_COUNT;
  
  // угол 
  qreal a = double(0.5 * M_PI) + _data_angle_step * double(lineNum);
  
  for(int i = 0; i < _params->line_point_count; i++)
  {
    int x = _params->display_point_count / 2 - double(i * _params->display_point_count) / _params->line_point_count / 2 * cos(a);
    int y = _params->display_point_count / 2 - double(i * _params->display_point_count) / _params->line_point_count / 2 * sin(a);
    
    quint8* v = (quint8*)(d + i);
 
    if(*v == 0) _img.setPixel(x, y, _params->painter_bkgnd_color.rgb());
    else {
      QColor color = _params->painter_data_color;
      color.setAlpha(*v);
      _img.setPixel(x, y, color.rgba());
    }
  }
  
  /** индикатор **/
  a = _indicator_angle_step * lineNum - 90;
  
  _indicator.setAngle(-a);
  
  /** Цена одного дискрета дальности в миллиметрах **/
  _current_discret = discret;
  
  
}


void SvRlsPainter::paintEvent(QPaintEvent *event)
{
//  if(!_rls_thread->isPlaying()) return;

  QPainter painter(this);
  painter.drawImage(0, 0, _img);
  
  QPen pen(QColor::fromRgba(qRgba(255, 255, 150, 50)));
  pen.setStyle(Qt::DashLine);
  painter.setPen(pen);
  
  /* отображаемая дистанция */
  qint64 dist = _params->line_point_count * _current_discret / 1000;
  
//  qreal i = step;

  /* окружности */
//  qreal lc = qreal(_params->grid_line_count) / 2;
  qreal step =  _radius / _params->grid_line_count;
  for(qreal i = 1; i <= _params->grid_line_count; i++) {
    qreal p = i * step;
    painter.drawEllipse(_radius - p, _radius - p, 2*p, 2*p);
  }
  
  /* диагонали */
  step =  2 * M_PI / _params->grid_line_count;
  for(int i = 0; i < _params->grid_line_count/2; i++) {

    qreal x = _radius * (1 - cos(i * step));
    qreal y = _radius * (1 - sin(i * step));
    
    qreal x1 = _radius * (1 + cos(i * step));
    qreal y1 = _radius * (1 + sin(i * step));
    
    painter.drawLine(_indicator_center, QPointF(x, y));
    painter.drawLine(_indicator_center, QPointF(x1, y1));
    
//    if(x)
//      painter.drawLine(QPointF(x, y), QPointF(x1, y));
    
//    if(y)
//      painter.drawLine(QPointF(x, y), QPointF(x, y1));
    
  }
  
  /* сетка */
//  step =  _params->display_point_count / _params->grid_line_count;
//  for(int i = 0; i < _params->grid_line_count; i++) {
//    qreal p = (i + 1) * step;
//    painter.drawLine(QPointF(_indicator_center.x() - p, 0), QPointF(_indicator_center.x() - p, _params->display_point_count));
//    painter.drawLine(QPointF(0, _indicator_center.y() - p), QPointF(_params->display_point_count, _indicator_center.y() - p));
    
//      painter.drawLine(QPointF(_indicator_center.x() + p, 0), QPointF(_indicator_center.x() + p, _params->display_point_count));
//      painter.drawLine(QPointF(0, _indicator_center.y() + p), QPointF(_params->display_point_count, _indicator_center.y() + p));
//  }


  painter.fillRect(0, 0, _params->display_point_count, _params->display_point_count, _indicator);
  
  pen.setColor(QColor::fromRgba(qRgba(255, 255, 150, 255)));
  pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);
  
  painter.drawText(10,20, QString("Радиус: %1 метров").arg(dist));
  
  painter.drawText(10, 40, QString("Шаг сетки: %1 метров").arg(dist / _params->grid_line_count));
  painter.drawLine(10, 50, 10, 55);
  painter.drawLine(10, 55, _radius / _params->grid_line_count, 55);
  painter.drawLine(_radius / _params->grid_line_count, 55, _radius / _params->grid_line_count, 50);
  
}
