#include "sv_rlswidget.h"


//SvSingleRlsWindow* SINGLERLS_UI;
//SvSelectRlsArchWindow* SELECT_RSL_ARCH_UI;

SvRlsWidget::SvRlsWidget(void *buffer, SvRlsWidgetParams &params)
{
  _params = params;
  _buffer = buffer;
  
  resize(_params.display_point_count, _params.display_point_count);
  
  _setupUI();  
  
  _cbDataSource->addItem("UDP", QVariant(svrlswdg::udp));
  _cbDataSource->addItem("\320\220\321\200\321\205\320\270\320\262", QVariant(svrlswdg::archive));

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
//  _dateArchBegin->setDate(_params.fromdate);
//  _timeArchBegin->setTime(_params.fromtime);
  _sliderLinePointCount->setValue(_params.line_point_count);

  _cbPaintBkgndColor->setCurrentIndex(_cbPaintBkgndColor->findData(_params.painter_bkgnd_color, Qt::DecorationRole));
  _cbPaintDataColor->setCurrentIndex(_cbPaintDataColor->findData(_params.painter_data_color, Qt::DecorationRole));
   
  
//  connect(_sliderLinePointCount, SIGNAL(valueChanged(int)), this, SLOT(setLinePointCount(int)));
  
  QMetaObject::connectSlotsByName(this);
  
  
  on__cbDataSource_currentIndexChanged(_params.source);
  
  _gbParams->setVisible(!_params.no_controls);
  
}

void SvRlsWidget::_setupUI()
{
  this->setObjectName(tr("SvRlsWidget"));

  _hlayMain = new QHBoxLayout(this);
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
  
          _gbParams = new QGroupBox(this);
          _gbParams->setObjectName(QStringLiteral("_gbParams"));
          QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
          sizePolicy1.setHorizontalStretch(0);
          sizePolicy1.setVerticalStretch(0);
          sizePolicy1.setHeightForWidth(_gbParams->sizePolicy().hasHeightForWidth());
          _gbParams->setSizePolicy(sizePolicy1);
          _vlayParams = new QVBoxLayout(_gbParams); 
          _vlayParams->setObjectName(QStringLiteral("_vlayParams"));
          _hlayDataSource = new QHBoxLayout();
          _hlayDataSource->setObjectName(QStringLiteral("_hlayDataSource"));
          _labelDataSource = new QLabel(_gbParams);
          _labelDataSource->setObjectName(QStringLiteral("_labelDataSource"));
          sizePolicy1.setHeightForWidth(_labelDataSource->sizePolicy().hasHeightForWidth());
          _labelDataSource->setSizePolicy(sizePolicy1);
          _labelDataSource->setMinimumSize(QSize(65, 0));
          _labelDataSource->setMaximumSize(QSize(65, 16777215));
          _labelDataSource->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          _hlayDataSource->addWidget(_labelDataSource);
  
          _cbDataSource = new QComboBox(_gbParams);
          _cbDataSource->setObjectName(QStringLiteral("_cbDataSource"));
  
          _hlayDataSource->addWidget(_cbDataSource);
  
  
          _vlayParams->addLayout(_hlayDataSource);
  
          _gbNetworkParams = new QGroupBox(_gbParams);
          _gbNetworkParams->setObjectName(QStringLiteral("_gbNetworkParams"));
          _vlayNetworkParams = new QVBoxLayout(_gbNetworkParams);
          _vlayNetworkParams->setObjectName(QStringLiteral("_vlayNetworkParams"));
          _hlayIp = new QHBoxLayout();
          _hlayIp->setObjectName(QStringLiteral("_hlayIp"));
          _labelIp = new QLabel(_gbNetworkParams);
          _labelIp->setObjectName(QStringLiteral("_labelIp"));
          sizePolicy1.setHeightForWidth(_labelIp->sizePolicy().hasHeightForWidth());
          _labelIp->setSizePolicy(sizePolicy1);
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
          sizePolicy1.setHeightForWidth(_labelPort->sizePolicy().hasHeightForWidth());
          _labelPort->setSizePolicy(sizePolicy1);
          _labelPort->setMinimumSize(QSize(65, 0));
          _labelPort->setMaximumSize(QSize(65, 16777215));
          _labelPort->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          _hlayPort->addWidget(_labelPort);
  
          _spinPort = new QSpinBox(_gbNetworkParams);
          _spinPort->setObjectName(QStringLiteral("_spinPort"));
          _spinPort->setMinimum(1);
          _spinPort->setMaximum(32565);
          _spinPort->setValue(8001);
  
          _hlayPort->addWidget(_spinPort);
  
  
          _vlayNetworkParams->addLayout(_hlayPort);
  
          _bnStartStopUDP = new QPushButton(_gbNetworkParams);
          _bnStartStopUDP->setObjectName(QStringLiteral("_bnStartStopUDP"));
          QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
          sizePolicy2.setHorizontalStretch(0);
          sizePolicy2.setVerticalStretch(0);
          sizePolicy2.setHeightForWidth(_bnStartStopUDP->sizePolicy().hasHeightForWidth());
          _bnStartStopUDP->setSizePolicy(sizePolicy2);
  
          _vlayNetworkParams->addWidget(_bnStartStopUDP);
  
  
          _vlayParams->addWidget(_gbNetworkParams);
  
          _gbArchParams = new QGroupBox(_gbParams);
          _gbArchParams->setObjectName(QStringLiteral("_gbArchParams"));
          _vlayArchParams = new QVBoxLayout(_gbArchParams);
          _vlayArchParams->setObjectName(QStringLiteral("_vlayArchParams"));
          _bnSelectArchiveFiles = new QPushButton(_gbArchParams);
          _bnSelectArchiveFiles->setObjectName(QStringLiteral("_bnSelectArchiveFiles"));
  
          _vlayArchParams->addWidget(_bnSelectArchiveFiles);
  
          _hlayCurrentArchiveFile = new QHBoxLayout();
          _hlayCurrentArchiveFile->setObjectName(QStringLiteral("_hlayCurrentArchiveFile"));
          _labelCurrentArchiveFile = new QLabel(_gbArchParams);
          _labelCurrentArchiveFile->setObjectName(QStringLiteral("_labelCurrentArchiveFile"));
          sizePolicy1.setHeightForWidth(_labelCurrentArchiveFile->sizePolicy().hasHeightForWidth());
          _labelCurrentArchiveFile->setSizePolicy(sizePolicy1);
          _labelCurrentArchiveFile->setMinimumSize(QSize(65, 0));
          _labelCurrentArchiveFile->setMaximumSize(QSize(65, 16777215));
          _labelCurrentArchiveFile->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          _hlayCurrentArchiveFile->addWidget(_labelCurrentArchiveFile);
  
          _editCurrentArchiveFile = new QLineEdit(_gbArchParams);
          _editCurrentArchiveFile->setObjectName(QStringLiteral("_editCurrentArchiveFile"));
          _editCurrentArchiveFile->setStyleSheet(QStringLiteral("background-color: rgb(240, 240, 240);"));
  
          _hlayCurrentArchiveFile->addWidget(_editCurrentArchiveFile);
  
  
          _vlayArchParams->addLayout(_hlayCurrentArchiveFile);
  
          _checkRepeatArchiveFiles = new QCheckBox(_gbArchParams);
          _checkRepeatArchiveFiles->setObjectName(QStringLiteral("_checkRepeatArchiveFiles"));
          _checkRepeatArchiveFiles->setChecked(true);
  
          _vlayArchParams->addWidget(_checkRepeatArchiveFiles);
  
          _bnStartStopArchive = new QPushButton(_gbArchParams);
          _bnStartStopArchive->setObjectName(QStringLiteral("_bnStartStopArchive"));
          sizePolicy2.setHeightForWidth(_bnStartStopArchive->sizePolicy().hasHeightForWidth());
          _bnStartStopArchive->setSizePolicy(sizePolicy2);
  
          _vlayArchParams->addWidget(_bnStartStopArchive);
  
  
          _vlayParams->addWidget(_gbArchParams);
  
          _gbPaintParams = new QGroupBox(_gbParams);
          _gbPaintParams->setObjectName(QStringLiteral("_gbPaintParams"));
          _vlayPaintParams = new QVBoxLayout(_gbPaintParams);
          _vlayPaintParams->setObjectName(QStringLiteral("_vlayPaintParams"));
          _hlayLinePointCount = new QHBoxLayout();
          _hlayLinePointCount->setObjectName(QStringLiteral("_hlayLinePointCount"));
          _labelLinePointCount = new QLabel(_gbPaintParams);
          _labelLinePointCount->setObjectName(QStringLiteral("_labelLinePointCount"));
          sizePolicy1.setHeightForWidth(_labelLinePointCount->sizePolicy().hasHeightForWidth());
          _labelLinePointCount->setSizePolicy(sizePolicy1);
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
          sizePolicy1.setHeightForWidth(_labelPaintBkgndColor->sizePolicy().hasHeightForWidth());
          _labelPaintBkgndColor->setSizePolicy(sizePolicy1);
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
          sizePolicy1.setHeightForWidth(_labelPaintDataColor->sizePolicy().hasHeightForWidth());
          _labelPaintDataColor->setSizePolicy(sizePolicy1);
          _labelPaintDataColor->setMinimumSize(QSize(65, 0));
          _labelPaintDataColor->setMaximumSize(QSize(65, 16777215));
          _labelPaintDataColor->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          _hlayPaintDataColor->addWidget(_labelPaintDataColor);
  
          _cbPaintDataColor = new QComboBox(_gbPaintParams);
          _cbPaintDataColor->setObjectName(QStringLiteral("_cbPaintDataColor"));
  
          _hlayPaintDataColor->addWidget(_cbPaintDataColor);
  
  
          _vlayPaintParams->addLayout(_hlayPaintDataColor);
  
  
          _vlayParams->addWidget(_gbPaintParams);
  
          _verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  
          _vlayParams->addItem(_verticalSpacer);
  
  
          _hlayMain->addWidget(_gbParams);
  


  // retranslateUi
  _bnStartStopUDP->setText(QApplication::translate("Form", "\320\241\321\202\320\260\321\200\321\202", Q_NULLPTR));
  _gbParams->setTitle(QString());
  _labelDataSource->setText(QApplication::translate("Form", "\320\230\321\201\321\202.\320\264\320\260\320\275\321\213\321\205", Q_NULLPTR));
  
  _cbDataSource->clear();
//  _cbDataSource->insertItems(0, QStringList()
//   << QApplication::translate("Form", "\320\241\320\265\321\202\321\214", Q_NULLPTR)
//   << QApplication::translate("Form", "\320\220\321\200\321\205\320\270\320\262", Q_NULLPTR)
//  );
  
  _gbNetworkParams->setTitle(QApplication::translate("Form", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \321\201\320\265\321\202\320\270", Q_NULLPTR));
  _labelIp->setText(QApplication::translate("Form", "\320\220\320\264\321\200\320\265\321\201", Q_NULLPTR));
  _editIp->setText(QApplication::translate("Form", "127.0.0.1", Q_NULLPTR));
  _editIp->setPlaceholderText(QApplication::translate("Form", "0.0.0.0", Q_NULLPTR));
  _labelPort->setText(QApplication::translate("Form", "\320\237\320\276\321\200\321\202", Q_NULLPTR));
  
  _gbArchParams->setTitle(QApplication::translate("Form", "\320\220\321\200\321\205\320\270\320\262", Q_NULLPTR));
//  _labelArchDateBegin->setText(QApplication::translate("Form", "\320\224\320\260\321\202\320\260", Q_NULLPTR));
//  _labelArchTimeBegin->setText(QApplication::translate("Form", "\320\222\321\200\320\265\320\274\321\217", Q_NULLPTR));
  _gbPaintParams->setTitle(QApplication::translate("Form", "\320\236\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\320\265", Q_NULLPTR));
  _labelLinePointCount->setText(QApplication::translate("Form", "\320\224\320\273\320\270\320\275\320\260 \320\273\321\203\321\207\320\260", Q_NULLPTR));
  _labelPaintBkgndColor->setText(QApplication::translate("Form", "\320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260", Q_NULLPTR));
  _labelPaintDataColor->setText(QApplication::translate("Form", "\320\246\320\262\320\265\321\202 \320\237\320\240\320\233\320\230", Q_NULLPTR));
 
  _bnSelectArchiveFiles->setText(QApplication::translate("Form", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214 \321\204\320\260\320\271\320\273\321\213", Q_NULLPTR));
  _labelCurrentArchiveFile->setText(QApplication::translate("Form", "\320\242\320\265\320\272\321\203\321\211\320\270\320\271", Q_NULLPTR));
  _checkRepeatArchiveFiles->setText(QApplication::translate("Form", "\320\237\320\276\320\262\321\202\320\276\321\200\321\217\321\202\321\214", Q_NULLPTR));
  _bnStartStopArchive->setText(QApplication::translate("Form", "\320\241\321\202\320\260\321\200\321\202", Q_NULLPTR));
  // retranslateUi

  
}

void SvRlsWidget::on__cbDataSource_currentIndexChanged(int index)
{
  switch (_cbDataSource->itemData(index).toInt()) {
    case svrlswdg::udp:
      _gbArchParams->setVisible(false);
      _gbNetworkParams->setVisible(true);
      break;
      
    default:
      _gbArchParams->setVisible(true);
      _gbNetworkParams->setVisible(false);
      break;
  }
}

void SvRlsWidget::on__bnSelectArchiveFiles_clicked()
{
  QDir dir(_params.archive_path);
  
  if(!dir.exists())
    dir.setPath(QDir::currentPath());
  
  _arch_files = QFileDialog::getOpenFileNames(this, tr("Выбрать файлы"), dir.path(), "Файлы архива bt2 (*.bt2);;Все файлы (*.*)");
  
  qSort(_arch_files);
  
  QFileInfo fi(_arch_files.first());
  _editCurrentArchiveFile->setText(fi.fileName());
  
}

void SvRlsWidget::on__bnStartStopUDP_clicked()
{
  bool ok;
  quint32 ip = QHostAddress(_editIp->text()).toIPv4Address(&ok);
  if(!ok) {
    QMessageBox::critical(this, "Ошибка", "Неверный IP адрес", QMessageBox::Ok);
    _editIp->setFocus();
    return;
  }
  
  _params.ip = ip;
  _params.port = _spinPort->value();
  
  _bnStartStopUDP->setEnabled(false);
  _editIp->setEnabled(false);
  _spinPort->setEnabled(false);
  _cbDataSource->setEnabled(false);
  
  emit start_stop_udp_clicked(_params.ip, _params.port);
  
}

void SvRlsWidget::on__bnStartStopArchive_clicked()
{
  if(_arch_files.isEmpty()) {
    QMessageBox::information(this, "Error", "Необходимо выбрать файлы", QMessageBox::Ok);
    _bnSelectArchiveFiles->setFocus();
    return;
  }
  
  _bnStartStopArchive->setEnabled(false);
  _bnSelectArchiveFiles->setEnabled(false);
  _cbDataSource->setEnabled(false);
  _checkRepeatArchiveFiles->setEnabled(false);
  
  emit start_stop_archive_clicked(&_arch_files);
  
}

void SvRlsWidget::startedUdp()
{
  _bnStartStopUDP->setStyleSheet("background-color: tomato");
  _bnStartStopUDP->setText("Стоп");  
  _bnStartStopUDP->setEnabled(true);
  
}

void SvRlsWidget::stoppedUdp()
{
   _bnStartStopUDP->setStyleSheet("");
   _bnStartStopUDP->setText("Старт");
   
   _bnStartStopUDP->setEnabled(true);
   _cbDataSource->setEnabled(true);
   _editIp->setEnabled(true);
   _spinPort->setEnabled(true);
}

void SvRlsWidget::startedArchive()
{
  _bnStartStopArchive->setStyleSheet("background-color: tomato");
  _bnStartStopArchive->setText("Стоп");  
  _bnStartStopArchive->setEnabled(true);
  
}

void SvRlsWidget::stoppedArchive()
{
   _bnStartStopArchive->setStyleSheet("");
   _bnStartStopArchive->setText("Старт");
   
   _bnStartStopArchive->setEnabled(true);
   _cbDataSource->setEnabled(true);
   _bnSelectArchiveFiles->setEnabled(true);
   _checkRepeatArchiveFiles->setEnabled(true);
   
   
}

void SvRlsWidget::fileReaded(QString filename)
{
  QFileInfo fi(filename);
  _editCurrentArchiveFile->setText(fi.fileName());
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
