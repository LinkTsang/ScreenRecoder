#ifndef RECORDERWINDOW_H
#define RECORDERWINDOW_H

#include <QMainWindow>
#include "screenrecorder.h"
namespace Ui {
class RecorderWindow;
}

/*!
  \class RecorderWindow
  \brief The RecorderWindow class is a window
         for recording the screen.
*/
class RecorderWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit RecorderWindow(QWidget *parent = 0);
  ~RecorderWindow();

 protected:
  void paintEvent(QPaintEvent *) override;
  void resizeEvent(QResizeEvent *) override;
  void showEvent(QShowEvent *) override;
  void moveEvent(QMoveEvent *) override;
  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;
  void closeEvent(QCloseEvent *) override;

 private slots:
  void on_startAndPauseButton_clicked();
  void on_stopButton_clicked();

 private:
  const int lineWidth = 1;
  const int sideBarWidth = 5;
  enum class Edge {
    None,
    Left,
    TopLeft,
    Top,
    TopRight,
    Right,
    BottomRight,
    Bottom,
    BottomLeft
  };
  ScreenRecorder screenRecorder_;

  Ui::RecorderWindow *ui;
  QColor backgroundColor_ = QColor(0xffffff);
  QColor windowBorderColor_ = QColor(0x748193);

  bool isMovingWindow_ = false;
  bool isResizingWindow_ = false;
  QPoint oldMousePosition_;
  Edge mousePressedEdge_ = Edge::None;
  QRect outsideBoxRect_;
  QRect insideBoxRect_;

  // get the edge the mouse intersected
  Edge getMouseIntersectEdge(QMouseEvent *) const;
  // set the cursor according to the mouse position
  void setCursorByEdge(Edge);

  // !-- handle window resizing
  void beginResizeWindow(QMouseEvent *);
  void resizeWindow(QMouseEvent *);
  void endResizeWindow();
  // handle window resizing --!

  // !-- handle window moving
  void beginMoveWindow(QMouseEvent *);
  void moveWindow(QMouseEvent *);
  void endMoveWindow();
  // handle window moving --!

  // calc the rects for the outside box and the inside box
  void calcBoxRects();

  void startRecord();
  void stopRecord();
};

#endif  // RECORDERWINDOW_H
