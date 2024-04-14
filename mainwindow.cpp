#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaMetaData>
#include <QFileDialog>



bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() != QEvent::KeyPress)
    {
        return QWidget::eventFilter(watched,event);
    }
    QKeyEvent * key_event = static_cast<QKeyEvent*>(event);
    if(watched == ui->listWidget)
    {
        if(key_event->key() == Qt::Key_Up)
        {
            this->on_btnPrevious_clicked();
        }
        if(key_event->key() == Qt::Key_Down)
        {
            this->on_btnNext_clicked();
        }

        if(key_event->key() == Qt::Key_Delete)
        {

            delete ui->listWidget->takeItem(ui->listWidget->currentRow());
        }

    }
    return QWidget::eventFilter(watched,event);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->installEventFilter(this);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window,Qt::white);
    this->setPalette(pal);
    this->setAutoFillBackground(true);
    _player = new QMediaPlayer(this);
    QAudioOutput *output = new QAudioOutput(this);


    _player->setAudioOutput(output);


    connect(_player,&QMediaPlayer::positionChanged,this,&MainWindow::do_positionChanged);
    //播放源的时长发生变化
    connect(_player,&QMediaPlayer::durationChanged,this,&MainWindow::do_durationChanged);
    //播放源发生变化
    connect(_player,&QMediaPlayer::sourceChanged,this,&MainWindow::do_sourceChanged);
    //播放的状态发生变化
    connect(_player,&QMediaPlayer::playbackStateChanged,this,&MainWindow::do_playbackStateChanged);
    connect(_player,&QMediaPlayer::playbackStateChanged,ui->rorarionPic,&RotationalWidget::onStartorStopRote);
    //元数据发生变化
    connect(_player,&QMediaPlayer::metaDataChanged,this,&MainWindow::do_metaDataChanged);




}

void MainWindow::do_positionChanged(qint64 position)
{
    if(ui->sliderPosition->isSliderDown())
    {
        return;
    }
    ui->sliderPosition->setSliderPosition(position);
    int secs = position /1000;
    int mins = secs / 60;
    secs %=60;
    _position_time =QString("%1:%2").arg(mins).arg(secs);
    ui->labRatio->setText(_position_time + "/"  + _duration_time);
}

void MainWindow::do_durationChanged(qint64 duration)
{
    ui->sliderPosition->setMaximum(duration);
    int secs = duration /1000;
    int mins = secs / 60;
    secs %=60;
    _duration_time =QString("%1:%2").arg(mins).arg(secs);
    ui->labRatio->setText(_position_time + "/"  + _duration_time);
}

void MainWindow::do_sourceChanged(const QUrl &media)
{
    QString file_name = media.fileName();
    QFileInfo file_info(file_name);
    this->setWindowTitle("音乐播放器 - " + file_info.completeBaseName());


}

void MainWindow::do_playbackStateChanged(QMediaPlayer::PlaybackState newState)
{
    ui->btnPlay->setEnabled(newState != QMediaPlayer::PlayingState);
    ui->btnPause->setEnabled(newState == QMediaPlayer::PlayingState);
    ui->btnStop->setEnabled(newState == QMediaPlayer::PlayingState);

    //自动播放的情况
    if( _loop_play && (newState == QMediaPlayer::StoppedState) )
    {
        //循环播放 1.自动下一首,2.从最后一首跳到第一首
        int count = ui->listWidget->count();
        int cur_row = ui->listWidget->currentRow();
        ++cur_row;
        if(cur_row == 1)
        {
            _player->play();
            return;
        }
        if(cur_row >= count)
        {
            cur_row = 0;
        }
        ui->listWidget->setCurrentRow(cur_row);

        _player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
        _player->play();

    }
    //如果不是循环播放,播完一首就暂停
}

void MainWindow::do_metaDataChanged()
{

    //元数据发生变化,修改显示图片
    QMediaMetaData meta_data = _player->metaData();
    QVariant meta_img = meta_data.value(QMediaMetaData::ThumbnailImage);

    if(meta_img.isValid())
    {
        QImage img = meta_img.value<QImage>();
        QPixmap music_pix = QPixmap::fromImage(img);

        ui->rorarionPic->setPixmap(music_pix);
        if(ui->rorarionPic->width() < music_pix.width())
        {
            ui->rorarionPic->setPixmap(music_pix.scaledToWidth(ui->rorarionPic->width() - 20));
        }else
        {
            ui->rorarionPic->setPixmap(music_pix);
        }

        if(ui->rorarionPic->height() < music_pix.height())
        {
            ui->rorarionPic->setPixmap(music_pix.scaledToWidth(ui->rorarionPic->height() - 20));
        }else
        {
            ui->rorarionPic->setPixmap(music_pix);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAdd_clicked()
{
    QString cur_path = QDir::homePath();

    QString dlg_title = "选择音频文件";
    QString filter = "音频文件(*.mp3 *.wav *.wma *.flac);;所有文件(*.*)";
    QStringList file_list =  QFileDialog::getOpenFileNames(this,dlg_title,cur_path,filter);
    if(file_list.isEmpty())
    {
        return;
    }

    foreach (const auto& item, file_list) {

        QFileInfo file_info(item);
        QString complete_base_name = file_info.completeBaseName();


        // 检查是否已经存在相同的项目
        QList<QListWidgetItem *> existingItems = ui->listWidget->findItems(complete_base_name, Qt::MatchExactly);
        if (!existingItems.isEmpty()) {
            // 如果已经存在相同的项目，则跳过
            continue;
        }
        QListWidgetItem *a_item = new QListWidgetItem(complete_base_name);
        a_item->setIcon(QIcon(":/images/images/musicFile.png"));
        a_item->setData(Qt::UserRole,QUrl::fromLocalFile(item));
        ui->listWidget->addItem(a_item);


    }
    //人生现在没有正在播放,就开始播放第一个文件
    if(_player->playbackState() != QMediaPlayer::PlayingState)
    {
        ui->listWidget->setCurrentRow(0);
        QUrl source = ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>();
        _player->setSource(source);
        _player->play();
    }

}


void MainWindow::on_btnRemove_clicked()
{
    int row = ui->listWidget->currentRow();
    if(row < 0)
    {
        return;
    }

    _player->pause();
    delete ui->listWidget->takeItem(row);

}


void MainWindow::on_btnClear_clicked()
{
    ui->listWidget->clear();
    _player->pause();
}


void MainWindow::on_btnPlay_clicked()
{

    if(_player->playbackState() ==  QMediaPlayer::PausedState)
    {
        _player->play();
        return;
    }
    if(ui->listWidget->count() < 0)
    {
        return;
    }
    if(ui->listWidget->currentRow() < 0)
    {
        ui->listWidget->setCurrentRow(0);
    }


    _player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    _player->play();
}


void MainWindow::on_btnPause_clicked()
{
    _player->pause();
}


void MainWindow::on_btnStop_clicked()
{
    _loop_play = false;
    _player->stop();
}


void MainWindow::on_btnPrevious_clicked()
{
    int cur_row = ui->listWidget->currentRow();
    cur_row--;
    if(cur_row < 0)
    {
        cur_row = ui->listWidget->count() -1;
    }
    ui->listWidget->setCurrentRow(cur_row);
    _loop_play = false;//在_player->setSource()方法会触发playbackStateChanged信号
    _player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    _player->play();
    _loop_play = ui->btnLoop->isChecked();
}


void MainWindow::on_btnNext_clicked()
{
    int cur_row = ui->listWidget->currentRow();
    cur_row++;
    if(cur_row > ui->listWidget->count() -1 )
    {
        cur_row = 0;
    }
    ui->listWidget->setCurrentRow(cur_row);
    _loop_play = false;//在_player->setSource()方法会触发playbackStateChanged信号
    _player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    _player->play();
    _loop_play = ui->btnLoop->isChecked();
}


void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    _player->setPlaybackRate(arg1);
}


void MainWindow::on_btnLoop_clicked(bool checked)
{
    _loop_play = checked;
}


void MainWindow::on_btnSound_clicked()
{
    bool mute = _player->audioOutput()->isMuted();
    _player->audioOutput()->setMuted(!mute);
    if(mute)
    {
        ui->btnSound->setIcon(QIcon(":/images/images/volumn.bmp"));
    }
    else
    {
        ui->btnSound->setIcon(QIcon(":/images/images/mute.bmp"));
    }
}


void MainWindow::on_sliderVolumn_valueChanged(int value)
{
    _player->audioOutput()->setVolume(value/100.0);
}


void MainWindow::on_sliderPosition_valueChanged(int value)
{
    _player->setPosition(value);
}


void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    _loop_play = false;
    _player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    _player->play();
    _loop_play = ui->btnLoop->isChecked();
}

