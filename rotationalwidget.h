#ifndef ROTATIONALWIDGET_H
#define ROTATIONALWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>

class RotationalWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RotationalWidget(QWidget *parent = nullptr);

    void setPixmap(const QPixmap &newPixmap);

signals:

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
public slots:
    void upDateRote();
    void onStartorStopRote();
private:
    QTimer *_ptimer;
    qreal _rote = 0;

    bool _start  = false;
    QPixmap pixmap;


};



#endif // ROTATIONALWIDGET_H
