#include "sv_rlswidget.h"


//SvSingleRlsWindow* SINGLERLS_UI;
//SvSelectRlsArchWindow* SELECT_RSL_ARCH_UI;

SvRlsWidget::SvRlsWidget(void *buffer, QString caption, int window_width, int window_height, int img_resolution, int draw_points_per_line)
{
  _buffer = buffer;
  _img_resolution = img_resolution;
  _draw_points_per_line = draw_points_per_line;
  _angle = double(2 * M_PI) / double(AZIMUTHS_COUNT);
  
  _img = QImage(_img_resolution, _img_resolution, QImage::Format_RGB888);
  
  
  _hLayMain = new QHBoxLayout(this);
  _hLayMain->setObjectName(QStringLiteral("_hLayMain"));
  
  _rls_painter = new SvRlsPainter(caption,
                                  &_img,
                                  _draw_points_per_line);
  
  _rls_painter->setObjectName(QStringLiteral("_rls_painter"));
  _rls_painter->setParent(this);  
  
  _vSlider = new QSlider(this);
  _vSlider->setObjectName(QStringLiteral("_vSlider"));
  _vSlider->setOrientation(Qt::Vertical);
  _vSlider->setMinimum(300);
  _vSlider->setMaximum(1000);
  _vSlider->setSingleStep(150);
  _vSlider->setValue(draw_points_per_line);
  _hLayMain->addWidget(_rls_painter);
  _hLayMain->addWidget(_vSlider);
  
  connect(_vSlider, SIGNAL(valueChanged(int)), _rls_painter, SLOT(setResolution(int)));
  
  
}

SvRlsWidget::~SvRlsWidget()
{
  deleteLater();
}

void SvRlsWidget::drawLine(int lineNum)
{
  void* d;
  d = _buffer + lineNum * MAX_LINE_SIZE;
  
  // угол 
  double a = double(0.5 * M_PI) + _angle * double(lineNum);
  
  // угол для индикатора
  double ind1 = double(0.5 * M_PI) + _angle * double(lineNum + 1);
  
  for(int i = 0; i < _draw_points_per_line; i++)
  {
    int x = _img_resolution / 2 - double(i * _img_resolution) / _draw_points_per_line / 2 * cos(a);
    int y = _img_resolution / 2 - double(i * _img_resolution) / _draw_points_per_line / 2 * sin(a);
    
    quint8* v = (quint8*)(d + i);
    if(*v < 0x55)
    {
//      if(background) img.setPixel(x, y, background->pixel(x, y));
//      else 
      _img.setPixel(x, y, qRgb(0, 0, 0)); // qRgb(3, 98, 155));
    }
    else if(*v < 0xAA) _img.setPixel(x, y, qRgb(255, 255, 50));
    else if(*v < 0xFF) _img.setPixel(x, y, qRgb(255, 255, 150));
    else _img.setPixel(x, y, qRgb(255, 255, 0));
    
    // индикатор
    x = _img_resolution / 2 - double(i * _img_resolution) / _draw_points_per_line / 2 * cos(ind1);
    y = _img_resolution / 2 - double(i * _img_resolution) / _draw_points_per_line / 2 * sin(ind1);
    
    _img.setPixel(x, y, qRgb(255, 100, 30));
       
  }
  
//  _rls_painter->repaint();
  
}

/** *************************************** **/

SvRlsPainter::SvRlsPainter(QString caption,
                           QImage *img,
                           int draw_points_per_line)
{
  _img = img;
  _img_resolution = _img->width();
  _draw_points_per_line = draw_points_per_line;
  _caption = caption;

  _timer.setInterval(10);
  _timer.setSingleShot(false);
  connect(&_timer, SIGNAL(timeout()), this, SLOT(repaint()));
  _timer.start();

}


void SvRlsPainter::paintEvent(QPaintEvent *event)
{
//  if(!_rls_thread->isPlaying()) return;

  QPainter painter(this);
  painter.drawImage(0, 0, *_img);
  
  QPen pen(QColor::fromRgb(qRgb(255, 255, 150)));
  pen.setWidth(2);
  painter.setPen(pen);
  
  painter.drawEllipse(0, 0, _img_resolution + 1, _img_resolution + 1);
  
  pen.setWidth(1);
  painter.setPen(pen);
  painter.drawEllipse(_img_resolution/2 - 50, _img_resolution/2 - 50, 100, 100);
  
//  painter.drawEllipse(200, 200, 100, 100);
}
