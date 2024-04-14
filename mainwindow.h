#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include "rotationalwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QMediaPlayer * _player;
    //循环播放
    bool _loop_play=true;
    QString _position_time;
    QString _duration_time;
signals:
    void roration();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void do_positionChanged(qint64 position);
    void do_durationChanged(qint64 duration);
    void do_sourceChanged(const QUrl &media);
    void do_playbackStateChanged(QMediaPlayer::PlaybackState newState);
    void do_metaDataChanged();




    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void on_btnClear_clicked();

    void on_btnPlay_clicked();

    void on_btnPause_clicked();

    void on_btnStop_clicked();

    void on_btnPrevious_clicked();

    void on_btnNext_clicked();

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_btnLoop_clicked(bool checked);

    void on_btnSound_clicked();

    void on_sliderVolumn_valueChanged(int value);

    void on_sliderPosition_valueChanged(int value);

    void on_listWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;


    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};


#endif // MAINWINDOW_H
