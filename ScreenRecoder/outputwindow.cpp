#include "outputwindow.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QMessageBox>
#include <QMovie>
#include <QPixmap>
#include <QString>
#include <QTime>
#include <ctime>
#include "gifwriter.h"
#include "octreepalette.h"
#include "recorderwindow.h"
#include "ui_outputwindow.h"
#include "windowmanager.h"






OutputWindow::OutputWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::OutputWindow) {
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
}

void OutputWindow::closeEvent(QCloseEvent *event) {
    this->hide();
    event->ignore();

    auto &recorderWindow = WindowManager::getRecorderWindow();
    recorderWindow.show();
}
OutputWindow::~OutputWindow() { delete ui; }

void OutputWindow::on_btgetFilePath_clicked() {
    QString filePath = QFileDialog::getExistingDirectory(
                this, tr(u8"Open Directory"), "/home",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->filedirectory->setText(filePath);
}

void OutputWindow::on_btCompound_clicked() {
    ui->useTime->setText("");
    if (directoryPath.isNull()) {
        QMessageBox::warning(this, tr(u8"警告"), tr(u8"请选择图片目录"),
                             QMessageBox::Yes);
        return;
    }
    double timeStart = (double)clock();

    if (ui->filedirectory->text().isEmpty() || ui->filename->text().isEmpty()) {
        QMessageBox::warning(this, tr(u8"警告"), tr(u8"请填写文件路径"),
                             QMessageBox::Yes);
        return;
    }

    QString fileName =
            ui->filedirectory->text() + "/" + ui->filename->text() + ".gif";

    encodeAlgorithm = (gif::Quantizer)ui->comboBox->currentIndex();

    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, filelist.size() - 1);
    ui->progressBar->setValue(0);

    ui->btCompound->setEnabled(false);
    ui->btPreView->setEnabled(false);
    workerThread = new WorkerThread(fileName, directoryPath, filelist, encodeAlgorithm);
    connect(workerThread, &WorkerThread::progressChanged, this, [this](int count){
        ui->progressBar->setValue(count);
    });
    connect(workerThread, &WorkerThread::finished, this, [this, timeStart, fileName](){
        double timeStop = (double)clock();
        ui->useTime->setText(tr(u8"用时：") +
                             QString::number((timeStop - timeStart) / 1000) + " s");
        ui->progressBar->setVisible(false);
        // qDebug()<<(timeStop - timeStart)/1000<<"s";
        QMessageBox::StandardButton isShowGif = QMessageBox::information(
                    this, tr(u8"提示"), tr(u8"合成完毕，是否显示？"),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (isShowGif == QMessageBox::Yes) {
            QMovie *movie = new QMovie(fileName);
            ui->pictureMap->setMovie(movie);
            movie->start();
        }
        QDir dir(directoryPath);
        dir.removeRecursively();

        delete workerThread;
        ui->btCompound->setEnabled(true);
        ui->btPreView->setEnabled(true);
    });
    workerThread->start();
    //  for (int i = 0; i < filelist.size(); i++) {
    //    QFileInfo fileInfo = filelist.at(i);
    //    // qDebug()<<fileInfo.fileName();
    //    QString filename = directoryPath + "/" + fileInfo.fileName();
    //    QImage image(filename);
    //    gifwriter.writeFrame(image);

    //    //

    //  }
    //  gifwriter.writeTrailer();
    //  file.close();
    //





}

void OutputWindow::on_btPrevious_clicked() {
    if (index > 0) {
        QFileInfo fileInfo = filelist.at(--index);
        // qDebug()<<fileInfo.fileName();
        QString filename = directoryPath + "/" + fileInfo.fileName();
        QPixmap pix(filename);
        ui->pictureMap->setPixmap(pix);
        ui->pictureMap->setScaledContents(true);
        ui->pictureMap->show();
    }
}

void OutputWindow::on_btNext_clicked() {
    if (index < filelist.size() - 2) {
        QFileInfo fileInfo = filelist.at(++index);
        // qDebug()<<fileInfo.fileName();
        QString filename = directoryPath + "/" + fileInfo.fileName();
        QPixmap pix(filename);
        ui->pictureMap->setPixmap(pix);
        ui->pictureMap->setScaledContents(true);
        ui->pictureMap->show();
    }
}

void OutputWindow::on_btPreView_clicked() {
    if (directoryPath.isNull()) {
        QMessageBox::warning(this, tr(u8"警告"), tr(u8"请选择图片目录"),
                             QMessageBox::Yes);
        return;
    }
    index = 0;
    for (int i = 0; i < filelist.size(); i++) {
        QFileInfo fileInfo = filelist.at(i);
        // qDebug()<<fileInfo.fileName();
        QString filename = directoryPath + "/" + fileInfo.fileName();
        QPixmap pix(filename);
        ui->pictureMap->setPixmap(pix);
        ui->pictureMap->setScaledContents(true);
        ui->pictureMap->show();
        sleep(100);
    }
    index = filelist.size() - 1;
}
void OutputWindow::sleep(int msec) {
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void OutputWindow::on_chooseDirectory_clicked() {
    directoryPath = QFileDialog::getExistingDirectory(
                this, tr(u8"Open Directory"), "/home",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir dir(directoryPath);
    dir.setFilter(QDir::Files);
    filelist = dir.entryInfoList();
    QFileInfo fileInfo = filelist.at(index);
    // qDebug()<<fileInfo.fileName();
    QString filename = directoryPath + "/" + fileInfo.fileName();
    QPixmap pix(filename);
    ui->pictureMap->setPixmap(pix);
    ui->pictureMap->setScaledContents(true);
    ui->pictureMap->show();
}
void OutputWindow::setWorkingDir(QString path) {
    directoryPath = path;
    ui->chooseDirectory->setVisible(false);

    QDir dir(directoryPath);
    dir.setFilter(QDir::Files);
    filelist = dir.entryInfoList();
    QFileInfo fileInfo = filelist.at(index);
    // qDebug()<<fileInfo.fileName();
    QString filename = directoryPath + "/" + fileInfo.fileName();
    QPixmap pix(filename);
    ui->pictureMap->setPixmap(pix);
    ui->pictureMap->setScaledContents(true);
    ui->pictureMap->show();
}

void WorkerThread::run() {
    file.open(QFile::WriteOnly);
    gif::GifWriter gifwriter(&file, quantizer);
    for (int i = 0; i < filelist.size(); i++) {
        QFileInfo fileInfo = filelist.at(i);
        qDebug()<<fileInfo.fileName();
        QString filename = directoryPath + "/" + fileInfo.fileName();
        QImage image(filename);
        gifwriter.writeFrame(image);
        emit progressChanged(i);
    }

    gifwriter.writeTrailer();
    file.close();
}

WorkerThread::WorkerThread(QString filename, QString directoryPath, QFileInfoList filelist, gif::Quantizer quantizer, QObject *parent) :
    file(filename), directoryPath(directoryPath), filelist(filelist), quantizer(quantizer), QThread(parent) {

}

