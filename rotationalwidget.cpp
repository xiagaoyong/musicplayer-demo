#include "rotationalwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <qDebug>
#include <QPalette>

RotationalWidget::RotationalWidget(QWidget *parent)
    : QWidget{parent}
{

    _ptimer = new QTimer();
    connect(_ptimer,&QTimer::timeout,this,&RotationalWidget::upDateRote);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window,Qt::white);
    this->setPalette(pal);
    this->setAutoFillBackground(true);




}
void RotationalWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPainterPath painterpath;
    painterpath.addEllipse(50,50,200,200);
    painter.setClipPath(painterpath);
    painter.drawPath(painterpath);

    if(pixmap.isNull())
    {

        pixmap = QPixmap(":/images/images/images.jpg");
    }

    QTransform transform;
    transform.translate(150,150);
    painter.setTransform(transform);

    painter.save();;
    painter.rotate(2 * _rote);

    painter.drawPixmap(-100,-100,200,200,pixmap);
    painter.restore();
}

void RotationalWidget::upDateRote()
{
    _rote = _rote  + 0.1;
    if(_rote > 360.0)
    {
        _rote= 0;
    }
    update();
}

void RotationalWidget::onStartorStopRote()
{
    qDebug() << "start";
    _start = !_start;
    if(_start)
    {
        qDebug()<< "rotation";
        _ptimer->start(10);

    }
    else
    {
       _ptimer->stop();
    }
}

void RotationalWidget::setPixmap(const QPixmap &newPixmap)
{
    pixmap = newPixmap;
    update();
}
