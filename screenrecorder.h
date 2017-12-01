#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <QDir>
#include <QObject>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QRect>
#include <QThread>
#include <QTimer>
#include <limits>

/*!
  \class ScreenRecorder
  \brief The ScreenRecorder class is a QThread object
         for recording the screen in the background thread.
*/
class ScreenRecorder : public QThread {
  Q_OBJECT
 public:
  ScreenRecorder(const QRect &area, int fps, QObject *parent = Q_NULLPTR);
  ScreenRecorder(QObject *parent = Q_NULLPTR);

  // start to record the screen
  void start();
  // pause the recorder
  void pause();
  // stop the recorder
  void stop();

  bool setFPS(int fps);
  bool setArea(const QRect &r);
  bool setArea(int x, int y, int w, int h);
  void moveAreaTo(int x, int y);
  bool setDir(QDir dir_);
  int count() const;
  enum class Status { Recording, Paused, Stopped };
  Status status() const;

 protected:
  void run() override;
 signals:
  void caputured(int currentCount);
  void statusChanged(Status status);

 private:
  QReadWriteLock lock_;
  QWaitCondition waitCondiction_;
  QRect area_;
  int count_;
  int fps_;
  Status status_;
  QDir dir_;
  const int MAX_SCREENSHOT_COUNT = std::numeric_limits<int>::max();
};

#endif  // SCREENRECORDER_H
