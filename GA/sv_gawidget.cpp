#include "sv_gawidget.h"

QMap<int, int>BUFFERS = {{0, 96000}, {1, 48000}, {2, 24000}, {3, 16000}, 
                          {4, 12000}, {5, 8000}, {6, 6000}, 
                          {7, 4000}, {8, 3000}, {9, 2000}}; //, 48};


//QStringList divstr = {"", };

svgawdg::SvGAWidget::SvGAWidget(void *buffer, svgawdg::SvGAWidgetParams &params)
{
  _params = params;
  _buffer = buffer;
  
  
//  resize(_params.display_point_count, _params.display_point_count);
  
  _setupUI();  
  
  svgraph::GraphParams gp;
  gp.line_color = _params.painter_data_color;
  gp.line_width = 1; //!!
  painter()->addGraph(0, gp);
  
  // чтобы задать максимум и минимум
//  painter()->appendData(0, -2500000);
//  painter()->appendData(0, 2500000);

  painter()->setYRange(-2500000.0, 2500000.0);
  
  painter()->setBuffersXY(_params.buffer_point_count);
  
  _cbDataSource->addItem("UDP", QVariant(svgawdg::udp));
  _cbDataSource->addItem("\320\220\321\200\321\205\320\270\320\262", QVariant(svgawdg::archive));

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
  _spinPort->setValue(quint16(_params.port));
  _sliderBufferPointCount->setValue(BUFFERS.key(_params.buffer_point_count));
  _cbPaintBkgndColor->setCurrentIndex(_cbPaintBkgndColor->findData(_params.painter_bkgnd_color, Qt::DecorationRole));
  _cbPaintDataColor->setCurrentIndex(_cbPaintDataColor->findData(_params.painter_data_color, Qt::DecorationRole));
  
  QMetaObject::connectSlotsByName(this);
  
  on__cbDataSource_currentIndexChanged(_params.source);
  
  _gbParams->setVisible((_params.source == svgawdg::archive) || !_params.no_controls);
  
}

void svgawdg::SvGAWidget::_setupUI()
{
  this->setObjectName(tr("SvGAWidget"));

  _hlayMain = new QHBoxLayout(this);
  _hlayMain->setObjectName(QStringLiteral("_hlayMain"));
  
  _ga_painter = new svgawdg::SvGAPainter(_buffer, &_params);
  _ga_painter->setParent(this);
  _ga_painter->setObjectName(QStringLiteral("_ga_painter"));
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(_ga_painter->sizePolicy().hasHeightForWidth());
  _ga_painter->setSizePolicy(sizePolicy);

  _hlayMain->addWidget(_ga_painter);
  
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
          _spinPort->setMaximum(65535);
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
          _labelDivider = new QLabel(_gbPaintParams);
          _labelDivider->setObjectName(QStringLiteral("_labelDivider"));
          sizePolicy1.setHeightForWidth(_labelDivider->sizePolicy().hasHeightForWidth());
          _labelDivider->setSizePolicy(sizePolicy1);
          _labelDivider->setMinimumSize(QSize(65, 0));
          _labelDivider->setMaximumSize(QSize(65, 16777215));
          _labelDivider->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          _hlayLinePointCount->addWidget(_labelDivider);
  
          _sliderBufferPointCount = new QSlider(_gbPaintParams);
          _sliderBufferPointCount->setObjectName(QStringLiteral("_sliderLinePointCount"));
          _sliderBufferPointCount->setMinimum(0);
          _sliderBufferPointCount->setMaximum(9);
          _sliderBufferPointCount->setSingleStep(1);
          _sliderBufferPointCount->setPageStep(1);
          _sliderBufferPointCount->setValue(1);
          _sliderBufferPointCount->setOrientation(Qt::Horizontal);
          _sliderBufferPointCount->setTickPosition(QSlider::TicksBelow);
          _sliderBufferPointCount->setTickInterval(1);
  
          _hlayLinePointCount->addWidget(_sliderBufferPointCount);
  
  
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
  
  _gbNetworkParams->setTitle(QApplication::translate("Form", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \321\201\320\265\321\202\320\270", Q_NULLPTR));
  _labelIp->setText(QApplication::translate("Form", "\320\220\320\264\321\200\320\265\321\201", Q_NULLPTR));
  _editIp->setText(QApplication::translate("Form", "127.0.0.1", Q_NULLPTR));
  _editIp->setPlaceholderText(QApplication::translate("Form", "0.0.0.0", Q_NULLPTR));
  _labelPort->setText(QApplication::translate("Form", "\320\237\320\276\321\200\321\202", Q_NULLPTR));
  
  _gbArchParams->setTitle(QApplication::translate("Form", "\320\220\321\200\321\205\320\270\320\262", Q_NULLPTR));
  _gbPaintParams->setTitle(QApplication::translate("Form", "\320\236\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\320\265", Q_NULLPTR));
  _labelDivider->setText(QApplication::translate("Form", "\320\224\320\265\320\273\320\270\321\202\320\265\320\273\321\214", Q_NULLPTR));
  _labelPaintBkgndColor->setText(QApplication::translate("Form", "\320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260", Q_NULLPTR));
  _labelPaintDataColor->setText(QApplication::translate("Form", "\320\246\320\262\320\265\321\202 \320\237\320\240\320\233\320\230", Q_NULLPTR));
 
  _bnSelectArchiveFiles->setText(QApplication::translate("Form", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214 \321\204\320\260\320\271\320\273\321\213", Q_NULLPTR));
  _labelCurrentArchiveFile->setText(QApplication::translate("Form", "\320\242\320\265\320\272\321\203\321\211\320\270\320\271", Q_NULLPTR));
  _checkRepeatArchiveFiles->setText(QApplication::translate("Form", "\320\237\320\276\320\262\321\202\320\276\321\200\321\217\321\202\321\214", Q_NULLPTR));
  _bnStartStopArchive->setText(QApplication::translate("Form", "\320\241\321\202\320\260\321\200\321\202", Q_NULLPTR));
  // retranslateUi

  
}

void svgawdg::SvGAWidget::on__cbDataSource_currentIndexChanged(int index)
{
  switch (_cbDataSource->itemData(index).toInt()) {
    case svgawdg::udp:
      _gbArchParams->setVisible(false);
      _gbNetworkParams->setVisible(true);
      break;
      
    default:
      _gbArchParams->setVisible(true);
      _gbNetworkParams->setVisible(false);
      break;
  }
}

void svgawdg::SvGAWidget::on__bnSelectArchiveFiles_clicked()
{
  QDir dir(_params.archive_path);
  
  if(!dir.exists())
    dir.setPath(QDir::currentPath());
  
  _arch_files = QFileDialog::getOpenFileNames(this, tr("Выбрать файлы"), dir.path(), "Файлы архива ga (*.ga);;Все файлы (*.*)");
  
  qSort(_arch_files);
  
  QFileInfo fi(_arch_files.first());
  _editCurrentArchiveFile->setText(fi.fileName());
  
}

void svgawdg::SvGAWidget::on__bnStartStopUDP_clicked()
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

void svgawdg::SvGAWidget::on__bnStartStopArchive_clicked()
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

void svgawdg::SvGAWidget::startedUdp()
{
  _bnStartStopUDP->setStyleSheet("background-color: tomato");
  _bnStartStopUDP->setText("Стоп");  
  
  _bnStartStopUDP->setEnabled(true);
  _editIp->setEnabled(false);
  _spinPort->setEnabled(false);
  _cbDataSource->setEnabled(false);
}

void svgawdg::SvGAWidget::stoppedUdp()
{
   _bnStartStopUDP->setStyleSheet("");
   _bnStartStopUDP->setText("Старт");
   
   _bnStartStopUDP->setEnabled(true);
   _cbDataSource->setEnabled(true);
   _editIp->setEnabled(true);
   _spinPort->setEnabled(true);
}

void svgawdg::SvGAWidget::startedArchive()
{
  _bnStartStopArchive->setStyleSheet("background-color: tomato");
  _bnStartStopArchive->setText("Стоп"); 
  
  _bnStartStopArchive->setEnabled(true);
  _bnSelectArchiveFiles->setEnabled(false);
  _cbDataSource->setEnabled(false);
  _checkRepeatArchiveFiles->setEnabled(false);
}

void svgawdg::SvGAWidget::stoppedArchive()
{
   _bnStartStopArchive->setStyleSheet("");
   _bnStartStopArchive->setText("Старт");
   
   _bnStartStopArchive->setEnabled(true);
   _cbDataSource->setEnabled(true);
   _bnSelectArchiveFiles->setEnabled(true);
   _checkRepeatArchiveFiles->setEnabled(true);
}

void svgawdg::SvGAWidget::fileReaded(QString filename)
{
  QFileInfo fi(filename);
  _editCurrentArchiveFile->setText(fi.fileName());
}

void svgawdg::SvGAWidget::on__sliderLinePointCount_valueChanged(int val) { 
  _params.buffer_point_count = BUFFERS.value(val);
  _ga_painter->setBuffersXY(_params.buffer_point_count);
}

/** *************************************** **/



svgawdg::SvGAPainter::SvGAPainter(void *buffer, SvGAWidgetParams *params, QWidget *parent)
{
  setParent(parent);
  
  _params = params;
  _buffer = buffer;
  
  setupUi();
  
  spinXRange->setValue(_params->x_range);
  bnYAutoscale->setChecked(_params->y_autoscale);  
  
  resize(_params->display_point_count, _params->display_point_count);
  
  on_bnResetChart_clicked();
  
//  _timer.setInterval(30);
//  _timer.setSingleShot(false);
//  connect(&_timer, SIGNAL(timeout()), this, SLOT(replot()));
//  _timer.start();
  
  _customplot->setBackground(QBrush(_params->painter_bkgnd_color));// StyleSheet("background-color: #FF0000");
  _customplot->xAxis->setTickLabelColor(_params->painter_data_color);
  _customplot->yAxis->setTickLabelColor(_params->painter_data_color);
  _customplot->yAxis->setLabel("Уровень сигнала");
  _customplot->yAxis->setLabelColor(_params->painter_data_color);
  _customplot->xAxis->setLabel("Время, мсек.");  
  _customplot->xAxis->setLabelColor(_params->painter_data_color);
  
}

void svgawdg::SvGAPainter::setupUi()
{
//    this->setObjectName(QStringLiteral("SvChartWidget1"));
  
    vlayMain = new QVBoxLayout(this);
    vlayMain->setObjectName(QStringLiteral("vlayMain"));
    
    hlay1 = new QHBoxLayout();
    hlay1->setSpacing(6);
    hlay1->setObjectName(QStringLiteral("hlay1"));
    hlay1->setContentsMargins(4, -1, -1, -1);
    
    bnResetChart = new QPushButton(this);
    bnResetChart->setObjectName(QStringLiteral("bnResetChart"));
    bnResetChart->setMaximumSize(QSize(25, 16777215));
    
    QIcon icon;
    icon.addFile(QStringLiteral(":/new/icons/Refresh.ico"), QSize(), QIcon::Normal, QIcon::Off);
    bnResetChart->setIcon(icon);

    hlay1->addWidget(bnResetChart);

    frameXRange = new QFrame(this);
    frameXRange->setObjectName(QStringLiteral("frameXRange"));
    frameXRange->setFrameShape(QFrame::StyledPanel);
    frameXRange->setFrameShadow(QFrame::Raised);
    hlayXRange = new QHBoxLayout(frameXRange);
    hlayXRange->setObjectName(QStringLiteral("hlayXRange"));
    hlayXRange->setContentsMargins(4, 4, 4, 4);
    hspacer1 = new QSpacerItem(122, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlayXRange->addItem(hspacer1);

    bnXRangeDown = new QPushButton(frameXRange);
    bnXRangeDown->setObjectName(QStringLiteral("bnXRangeDown"));
    bnXRangeDown->setMaximumSize(QSize(25, 16777215));
    QIcon icon1;
    icon1.addFile(QStringLiteral(":/new/icons/Zoom out.ico"), QSize(), QIcon::Normal, QIcon::Off);
    bnXRangeDown->setIcon(icon1);

    hlayXRange->addWidget(bnXRangeDown);

    hspacer2 = new QSpacerItem(123, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlayXRange->addItem(hspacer2);

    bnXRangeActual = new QPushButton(frameXRange);
    bnXRangeActual->setObjectName(QStringLiteral("bnXRangeActual"));
    bnXRangeActual->setMaximumSize(QSize(25, 16777215));
    QIcon icon2;
    icon2.addFile(QStringLiteral(":/new/icons/Search.ico"), QSize(), QIcon::Normal, QIcon::Off);
    bnXRangeActual->setIcon(icon2);

    hlayXRange->addWidget(bnXRangeActual);

    hspacer3 = new QSpacerItem(122, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlayXRange->addItem(hspacer3);

    spinXRange = new QSpinBox(frameXRange);
    spinXRange->setObjectName(QStringLiteral("spinXRange"));
    spinXRange->setMinimum(10);
    spinXRange->setMaximum(10000);

    hlayXRange->addWidget(spinXRange);

    bnXSetRange = new QPushButton(frameXRange);
    bnXSetRange->setObjectName(QStringLiteral("bnXSetRange"));
    bnXSetRange->setMaximumSize(QSize(25, 16777215));
    QIcon icon3;
    icon3.addFile(QStringLiteral(":/new/icons/Ok2.ico"), QSize(), QIcon::Normal, QIcon::Off);
    bnXSetRange->setIcon(icon3);

    hlayXRange->addWidget(bnXSetRange);

    hspacer4 = new QSpacerItem(123, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlayXRange->addItem(hspacer4);

    bnXRangeUp = new QPushButton(frameXRange);
    bnXRangeUp->setObjectName(QStringLiteral("bnXRangeUp"));
    bnXRangeUp->setMaximumSize(QSize(25, 16777215));
    QIcon icon4;
    icon4.addFile(QStringLiteral(":/new/icons/Zoom in.ico"), QSize(), QIcon::Normal, QIcon::Off);
    bnXRangeUp->setIcon(icon4);

    hlayXRange->addWidget(bnXRangeUp);

    hspacer5 = new QSpacerItem(122, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlayXRange->addItem(hspacer5);


    hlay1->addWidget(frameXRange);


    vlayMain->addLayout(hlay1);

    hlay2 = new QHBoxLayout();
    hlay2->setObjectName(QStringLiteral("hlay2"));
    frameYRange = new QFrame(this);
    frameYRange->setObjectName(QStringLiteral("frameYRange"));
    frameYRange->setFrameShape(QFrame::StyledPanel);
    frameYRange->setFrameShadow(QFrame::Raised);
    vlayYRange = new QVBoxLayout(frameYRange);
    vlayYRange->setSpacing(2);
    vlayYRange->setObjectName(QStringLiteral("vlayYRange"));
    vlayYRange->setContentsMargins(4, 4, 4, 4);
    bnYAutoscale = new QPushButton(frameYRange);
    bnYAutoscale->setObjectName(QStringLiteral("bnYAutoscale"));
    bnYAutoscale->setMaximumSize(QSize(25, 16777215));
    QIcon icon5;
    icon5.addFile(QStringLiteral(":/new/icons/Stats.ico"), QSize(), QIcon::Normal, QIcon::Off);
    bnYAutoscale->setIcon(icon5);
    bnYAutoscale->setCheckable(true);
  
    vlayYRange->addWidget(bnYAutoscale);

    vspacer1 = new QSpacerItem(20, 84, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vlayYRange->addItem(vspacer1);

    bnYRangeDown = new QPushButton(frameYRange);
    bnYRangeDown->setObjectName(QStringLiteral("bnYRangeDown"));
    bnYRangeDown->setMaximumSize(QSize(25, 16777215));
    bnYRangeDown->setIcon(icon4);

    vlayYRange->addWidget(bnYRangeDown);

    vspacer2 = new QSpacerItem(20, 85, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vlayYRange->addItem(vspacer2);

    bnYRangeActual = new QPushButton(frameYRange);
    bnYRangeActual->setObjectName(QStringLiteral("bnYRangeActual"));
    bnYRangeActual->setMaximumSize(QSize(25, 16777215));
    bnYRangeActual->setIcon(icon2);

    vlayYRange->addWidget(bnYRangeActual);

    vspacer3 = new QSpacerItem(20, 84, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vlayYRange->addItem(vspacer3);

    bnYRangeUp = new QPushButton(frameYRange);
    bnYRangeUp->setObjectName(QStringLiteral("bnYRangeUp"));
    bnYRangeUp->setMaximumSize(QSize(25, 16777215));
    QIcon icon6;
    icon6.addFile(QStringLiteral(":/new/icons/Zoom out.ico"), QSize(), QIcon::Normal, QIcon::Off);
    bnYRangeUp->setIcon(icon6);

    vlayYRange->addWidget(bnYRangeUp);

    vspacer4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vlayYRange->addItem(vspacer4);

    hlay2->addWidget(frameYRange);


    _customplot = new QCustomPlot(this);
    _customplot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _customplot->xAxis->setRange(0, _params->x_range, Qt::AlignLeft);
    
    _customplot->yAxis->setRange(0, 1, Qt::AlignCenter);
//    _customplot->axisRect()->setupFullAxesBox(true);
//    _customplot->yAxis->setScaleType(QCPAxis::stLogarithmic);
//    _customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    _customplot->legend->setVisible(true);
        
    hlay2->addWidget(_customplot);
    
    vlayMain->addLayout(hlay2);

    frameXRange->setVisible(false);
//    frameYRange->setVisible(false);
    bnResetChart->setVisible(false);
//    retranslateUi(this);

    QMetaObject::connectSlotsByName(this);
} // setupUi

void svgawdg::SvGAPainter::addGraph(int graph_id, svgraph::GraphParams &graphParams)
{
  /* если такой график уже есть, то ничего не добавляем и выходим */
  if(findGraph(graph_id))
    return;
    
  svgawdg::GRAPH* g = new svgawdg::GRAPH;
  g->params = graphParams;
  g->graph = _customplot->addGraph();
  
  _graphs.insert(graph_id, g);

  QPen pen(graphParams.line_color);
  pen.setStyle(Qt::PenStyle(graphParams.line_style));
  pen.setWidth(graphParams.line_width);
    
  _graphs.value(graph_id)->graph->setPen(pen);
  _graphs.value(graph_id)->graph->setName(svgraph::GraphTypes.value(graph_id));
  
  
//  _graphs.value(graph_id)->graph->setLineStyle(QCPGraph::lsLine); //lsImpulse);lsLine // LeStyle 
//  _graphs.value(graph_id)->graph->setAntialiased(true);
  
}

void svgawdg::SvGAPainter::setGraphParams(int graph_id, svgraph::GraphParams &graphParams)
{
  _graphs.value(graph_id)->params = graphParams;
  
  QPen pen(graphParams.line_color);
  pen.setStyle(Qt::PenStyle(graphParams.line_style));
  pen.setWidth(graphParams.line_width);
    
  _graphs.value(graph_id)->graph->setPen(pen);
  
  _customplot->repaint();
  
}

void svgawdg::SvGAPainter::removeGraph(int graph_id)
{
  /* очищаем и удаляем graph */
  _graphs.value(graph_id)->graph->clearData();
  _customplot->removeGraph(_graphs.value(graph_id)->graph);
  
  /* удаляем GRAPH */
  delete _graphs.value(graph_id);
  
  /* удаляем запись о графике из map'а */
  _graphs.remove(graph_id);
  
  _customplot->replot();
  
}

void svgawdg::SvGAPainter::appendData(int graph_id, double y)
{
  double x = _graphs.value(graph_id)->graph->data()->count();
  _graphs.value(graph_id)->graph->data()->insert(x, QCPData(x, y));
  
  setMaxMinY(y);
  
  if(_params->y_autoscale)
    setActualYRange();
}

void svgawdg::SvGAPainter::insertData(int graph_id, QCPData xy)
{
  _graphs.value(graph_id)->graph->data()->insert(xy.key, xy);
  
  setMaxMinY(xy.value);
  
  if(_params->y_autoscale)
    setActualYRange();
}

void svgawdg::SvGAPainter::on_bnXRangeUp_clicked()
{
  _params->x_range *= 1.25;
  _customplot->xAxis->setRangeUpper(_params->x_range);
  _customplot->replot(QCustomPlot::rpQueued);
}

void svgawdg::SvGAPainter::on_bnXRangeDown_clicked()
{
  _params->x_range /= 1.25;
  _customplot->xAxis->setRangeUpper(_params->x_range);
  _customplot->replot(QCustomPlot::rpQueued);
}

void svgawdg::SvGAPainter::on_bnXRangeActual_clicked()
{
  _customplot->xAxis->setRange(0, _customplot->graph()->data()->count(), Qt::AlignLeft);
  _customplot->replot(QCustomPlot::rpQueued);
}

void svgawdg::SvGAPainter::on_bnXSetRange_clicked()
{
  _params->x_range = spinXRange->value();
  _customplot->xAxis->setRangeUpper(_params->x_range);
  _customplot->replot(QCustomPlot::rpQueued);
}

void svgawdg::SvGAPainter::on_bnYRangeUp_clicked()
{
  _customplot->yAxis->setRangeUpper(_customplot->yAxis->range().upper * 0.75);
  _customplot->yAxis->setRangeLower(_customplot->yAxis->range().lower * 0.75);
  _customplot->replot(QCustomPlot::rpQueued);
}

void svgawdg::SvGAPainter::on_bnYRangeDown_clicked()
{
  _customplot->yAxis->setRangeUpper(_customplot->yAxis->range().upper * 1.25);
  _customplot->yAxis->setRangeLower(_customplot->yAxis->range().lower * 1.25);
  _customplot->replot(QCustomPlot::rpQueued);
}

void svgawdg::SvGAPainter::on_bnYRangeActual_clicked()
{
  setActualYRange();
}

void svgawdg::SvGAPainter::setActualYRange()
{
  _customplot->yAxis->setRange(_y_min, _y_max);
  _customplot->replot(QCustomPlot::rpQueued);
}

void svgawdg::SvGAPainter::on_bnResetChart_clicked()
{
  for(int i = 0; i < _customplot->graphCount(); i++)
    _customplot->graph(i)->clearData();
  
  _customplot->xAxis->setRange(0, spinXRange->value(), Qt::AlignLeft);
  
  _y_max = -1000000000;
  _y_min =  1000000000;
  
  _customplot->replot();
  
}

void svgawdg::SvGAPainter::on_bnYAutoscale_clicked(bool checked)
{
  if(checked)
    on_bnYRangeActual_clicked();
  
  _params->y_autoscale = checked;
  
}

void svgawdg::SvGAPainter::replot()
{
  _customplot->replot();
}

void svgawdg::SvGAPainter::setBuffersXY(int bufsize)
{
  if(_customplot->graphCount() == 0) return;
  
  x_data.resize(bufsize);
  y_data.resize(bufsize);
  
  for(int i = 0; i < bufsize; i++)
    x_data[i] = i;
  
  _customplot->xAxis->setRangeUpper(bufsize);
  
  _customplot->graph(0)->setAdaptiveSampling(true);
  _customplot->replot();
  
}

void svgawdg::SvGAPainter::drawData(quint32 pointCount)
{
  if(!_buffer) return;
  
  quint64 datalen = pointCount * sizeof(double);
  quint64 offset = y_data.size() * sizeof(double) - datalen;
  
  void *d = (void*)(y_data.data());
  memcpy(d, d + datalen, offset);
  memcpy(d + offset, _buffer, datalen);
  
  
  double y_max = _customplot->yAxis->range().upper;
  double y_min = _customplot->yAxis->range().lower;
  for(int i = 0; i < x_data.count(); i++ ) {
    x_data[i] += pointCount;
   
    if(y_data[i] < y_min) y_min = y_data[i];
    if(y_data[i] > y_max) y_max = y_data[i];
  }
  
  _customplot->xAxis->setRange(x_data.first(), x_data.last());
  
  if(_params->y_autoscale)
    setYRange(y_min, y_max);
    
  
  _customplot->graph(0)->setData(x_data, y_data);
  _customplot->replot();
  
}
