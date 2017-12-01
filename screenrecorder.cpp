#include "screenrecorder.h"
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QElapsedTimer>
#include <QMutex>
#include <QPixmap>
#include <QRunnable>
#include <QScreen>
#include <QTemporaryDir>
#include <QThreadPool>
#include <utility>

/*!
  \class PixmapSavingTask
  \brief runable task for saving the pixmap to file
*/
class PixmapSavingTask : public QRunnable {
 public:
  PixmapSavingTask(QString fileName, QPixmap &&pixmap)
      : fileName_(fileName), pixmap_(std::move(pixmap)) {}
  void run() override { pixmap_.save(fileName_, "PNG"); }

 private:
  QString fileName_;
  QPixmap pixmap_;
};

// !-- ScreenRecorder

ScreenRecorder::ScreenRecorder(const QRect &area, int fps, QObject *parent)
    : QThread(parent),
      status_(Status::Stopped),
      count_(0),
      area_(area),
      fps_(fps) {}

ScreenRecorder::ScreenRecorder(QObject *parent)
    : ScreenRecorder(QRect(), 30, parent) {}

void ScreenRecorder::start() {
  switch (status_) {
    case Status::Stopped:
      status_ = Status::Recording;
      QThread::start();
      emit statusChanged(Status::Recording);
      break;
    case Status::Paused:
      status_ = Status::Recording;
      waitCondiction_.wakeAll();
      emit statusChanged(Status::Recording);
      break;
  }
}

void ScreenRecorder::pause() {
  if (status_ == Status::Recording) status_ = Status::Paused;
}

void ScreenRecorder::stop() {
  Status oldStatus = status_;
  status_ = Status::Stopped;
  if (oldStatus == Status::Paused) {
    waitCondiction_.wakeAll();
  }
}

bool ScreenRecorder::setArea(const QRect &r) {
  if (isRunning()) return false;
  area_.setRect(r.x(), r.y(), r.width(), r.height());
  return true;
}

bool ScreenRecorder::setArea(int x, int y, int w, int h) {
  if (isRunning()) return false;
  area_.setRect(x, y, w, h);
  return true;
}

bool ScreenRecorder::setFPS(int fps) {
  if (isRunning()) return false;
  if (fps < 1 || fps > 60) fps = 30;
  fps_ = fps;
  return true;
}

void ScreenRecorder::moveAreaTo(int x, int y) {
  lock_.lockForWrite();
  area_.moveTo(x, y);
  lock_.unlock();
}

bool ScreenRecorder::setDir(QDir dir) {
  if (isRunning()) return false;
  dir_ = dir;
  return true;
}

int ScreenRecorder::count() const { return count_; }

ScreenRecorder::Status ScreenRecorder::status() const { return status_; }

// override the QThread::run()
void ScreenRecorder::run() {
  qDebug() << "Save screenshots to " << dir_.path();
  qDebug() << "FPS: " << fps_;
  if (!dir_.exists()) {
    dir_.mkpath(".");
  }
  count_ = 0;
  int interval = 1000 / fps_;
  QElapsedTimer timer;
  QScreen *screen = QApplication::primaryScreen();
  QThreadPool *threadPool = QThreadPool::globalInstance();
  QMutex mutex;
  forever {
    if (status_ == Status::Recording) {
      timer.restart();
      lock_.lockForRead();
      int x = area_.x();
      int y = area_.y();
      lock_.unlock();
      QPixmap pixmap =
          screen->grabWindow(0, x, y, area_.width(), area_.height());
      QString fileName = dir_.filePath(QString("%1.png").arg(count_++));

      threadPool->start(new PixmapSavingTask(fileName, std::move(pixmap)));

      emit caputured(count_);

      int sleepTime = qMax(static_cast<int>(interval - timer.elapsed()), 0);
      if (sleepTime > 0) QThread::msleep(sleepTime);

      int elapsedTime = timer.elapsed();
      qDebug() << "FileName: " << fileName;
      qDebug() << "Time elapsed: " << elapsedTime
               << "ms, fps: " << 1000.0 / elapsedTime;

      if (count_ == MAX_SCREENSHOT_COUNT) {
        status_ = Status::Stopped;
      }
    } else if (status_ == Status::Paused) {
      emit statusChanged(Status::Paused);
      mutex.lock();
      waitCondiction_.wait(&mutex);
      mutex.unlock();
    } else {
      break;
    }
  }
  emit statusChanged(Status::Stopped);
}

// ScreenRecorder --!
