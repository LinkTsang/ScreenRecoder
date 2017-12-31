#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include <QFileInfo>
#include <QMainWindow>
#include <QThread>
#include "gifwriter.h"

class WorkerThread : public QThread
{
    Q_OBJECT

signals:
    void progressChanged(int count);
public:
    void run() override;
    WorkerThread(QString filename, QString directoryPath, QFileInfoList filelist, gif::Quantizer quantizer, QObject *parent = Q_NULLPTR);
private:
    QFile file;
    QString directoryPath;
    gif::Quantizer quantizer;
    QFileInfoList filelist;
};

namespace Ui {
class OutputWindow;
}

class OutputWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit OutputWindow(QWidget *parent = 0);
  void closeEvent(QCloseEvent *event) override;
  void setWorkingDir(QString);
  ~OutputWindow();
 private slots:
  void on_btgetFilePath_clicked();
  void on_chooseDirectory_clicked();

  void on_btCompound_clicked();
  void on_btPrevious_clicked();
  void on_btNext_clicked();

  void on_btPreView_clicked();

 private:
  Ui::OutputWindow *ui;
  QString directoryPath;
  gif::Quantizer encodeAlgorithm;  // 0 -- Oct || 1 -- Neu
  int index = 0;
  QFileInfoList filelist;
  void sleep(int msec);
  WorkerThread* workerThread;
};

#endif  // OUTPUTWINDOW_H
