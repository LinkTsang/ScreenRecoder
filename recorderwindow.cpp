#include "recorderwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMouseEvent>
#include <QPainter>
#include <QTemporaryFile>
#include <cmath>
#include "ui_mainwindow.h"

RecorderWindow::RecorderWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  Qt::WindowFlags flags = Qt::Dialog;
  flags |= Qt::CustomizeWindowHint;
  flags |= Qt::FramelessWindowHint;
  flags |= Qt::WindowStaysOnTopHint;
  flags |= Qt::X11BypassWindowManagerHint;
  setWindowFlags(flags);
  setAttribute(Qt::WA_TranslucentBackground, true);

  connect(ui->minimizeButton, &QPushButton::clicked, this,
          [this]() { setWindowState(Qt::WindowMinimized); });
  connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
  connect(
      &screenRecorder_, &ScreenRecorder::caputured, this,
      [this](int count) { ui->titleLabel->setText(QString("%1").arg(count)); });
}

RecorderWindow::~RecorderWindow() { delete ui; }

void RecorderWindow::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setPen(Qt::NoPen);
  p.setBrush(backgroundColor_);

  QRect titleBarRect = ui->titleBarLayout->geometry();
  QRect bottomBarRect = ui->bottomBarLayout->geometry();

  p.drawRect(titleBarRect);
  p.drawRect(bottomBarRect);

  int sideBarHeight = bottomBarRect.y() - titleBarRect.height();
  p.drawRect(titleBarRect.x(), titleBarRect.y() + titleBarRect.height(),
             sideBarWidth, sideBarHeight);
  p.drawRect(titleBarRect.x() + titleBarRect.width() - sideBarWidth,
             titleBarRect.y() + titleBarRect.height(), sideBarWidth,
             sideBarHeight);

  QPen edgePen(windowBorderColor_, lineWidth);
  p.setPen(edgePen);
  p.setBrush(QColor(0, 0, 0, 0));
  p.drawRect(outsideBoxRect_);
  p.drawRect(insideBoxRect_);
}

void RecorderWindow::resizeEvent(QResizeEvent *e) {
  calcBoxRects();
  QWidget::resizeEvent(e);
}

void RecorderWindow::showEvent(QShowEvent *e) {
  calcBoxRects();
  QWidget::showEvent(e);
}

void RecorderWindow::moveEvent(QMoveEvent *e) {
  calcBoxRects();
  QRect area = insideBoxRect_;
  area.moveTo(mapToGlobal(area.topLeft()));
  screenRecorder_.moveAreaTo(area.x(), area.y());
  QWidget::moveEvent(e);
}

void RecorderWindow::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    mousePressedEdge_ = getMouseIntersectEdge(e);
    if (mousePressedEdge_ == Edge::None) {
      beginMoveWindow(e);
    } else if (screenRecorder_.status() == ScreenRecorder::Status::Stopped) {
      beginResizeWindow(e);
    }
  }
}

void RecorderWindow::mouseMoveEvent(QMouseEvent *e) {
  if (isMovingWindow_) {
    moveWindow(e);
  } else if (isResizingWindow_) {
    resizeWindow(e);
  } else if (screenRecorder_.status() == ScreenRecorder::Status::Stopped) {
    Edge edge = getMouseIntersectEdge(e);
    setCursorByEdge(edge);
  }
}

void RecorderWindow::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    isMovingWindow_ = false;
    isResizingWindow_ = false;
  }
}

void RecorderWindow::closeEvent(QCloseEvent *e)
{
  screenRecorder_.stop();
  screenRecorder_.wait();
  QMainWindow::closeEvent(e);
}

RecorderWindow::Edge RecorderWindow::getMouseIntersectEdge(QMouseEvent *e) const {
  const int precision = 3;
  auto titleBarRect = ui->titleBarLayout->geometry();
  auto bottomBarRect = ui->bottomBarLayout->geometry();
  int left = titleBarRect.x();
  int top = titleBarRect.y();
  int right = left + titleBarRect.width();
  int bottom = bottomBarRect.y() + bottomBarRect.height();
  int mouseX = e->x();
  int mouseY = e->y();
  bool onLeft = std::abs(mouseX - left) < precision;
  bool onTop = std::abs(mouseY - top) < precision;
  bool onRight = std::abs(mouseX - right) < precision;
  bool onBottom = std::abs(mouseY - bottom) < precision;
  if (onLeft && onTop) {
    return Edge::TopLeft;
  } else if (onLeft && onBottom) {
    return Edge::BottomLeft;
  } else if (onRight && onTop) {
    return Edge::TopRight;
  } else if (onRight && onBottom) {
    return Edge::BottomRight;
  } else if (onLeft) {
    return Edge::Left;
  } else if (onRight) {
    return Edge::Right;
  } else if (onTop) {
    return Edge::Top;
  } else if (onBottom) {
    return Edge::Bottom;
  } else {
    return Edge::None;
  }
}

void RecorderWindow::setCursorByEdge(RecorderWindow::Edge edge) {
  switch (edge) {
    case Edge::TopLeft:
    case Edge::BottomRight:
      setCursor(Qt::SizeFDiagCursor);
      break;
    case Edge::BottomLeft:
    case Edge::TopRight:
      setCursor(Qt::SizeBDiagCursor);
      break;
    case Edge::Left:
    case Edge::Right:
      setCursor(Qt::SizeHorCursor);
      break;
    case Edge::Top:
    case Edge::Bottom:
      setCursor(Qt::SizeVerCursor);
      break;
    default:
      setCursor(Qt::ArrowCursor);
      break;
  }
}

void RecorderWindow::beginResizeWindow(QMouseEvent *e) {
  oldMousePosition_ = e->globalPos();
  isResizingWindow_ = true;
}

void RecorderWindow::resizeWindow(QMouseEvent *e) {
  if (mousePressedEdge_ == Edge::None) return;
  QPoint newMousePosition = e->globalPos();
  int deltaX = newMousePosition.x() - oldMousePosition_.x();
  int deltaY = newMousePosition.y() - oldMousePosition_.y();
  QRect rect = geometry();
  int left = rect.x();
  int top = rect.y();
  int right = left + rect.width();
  int bottom = top + rect.height();
  switch (mousePressedEdge_) {
    case Edge::Left:
      left += deltaX;
      break;
    case Edge::TopLeft:
      left += deltaX;
      top += deltaY;
      break;
    case Edge::Top:
      top += deltaY;
      break;
    case Edge::TopRight:
      right += deltaX;
      top += deltaY;
      break;
    case Edge::Right:
      right += deltaX;
      break;
    case Edge::BottomRight:
      right += deltaX;
      bottom += deltaY;
      break;
    case Edge::Bottom:
      bottom += deltaY;
      break;
    case Edge::BottomLeft:
      left += deltaX;
      bottom += deltaY;
      break;
  }
  setGeometry(left, top, right - left, bottom - top);
  oldMousePosition_ = newMousePosition;
}

void RecorderWindow::endResizeWindow() {}

void RecorderWindow::beginMoveWindow(QMouseEvent *event) {
  oldMousePosition_ = event->globalPos();
  isMovingWindow_ = true;
}

void RecorderWindow::moveWindow(QMouseEvent *event) {
  QPoint currentPosition = event->globalPos();
  QPoint delta = currentPosition - oldMousePosition_;
  move(currentPosition + delta - event->pos());
  oldMousePosition_ = currentPosition;
}

void RecorderWindow::endMoveWindow() {}

void RecorderWindow::calcBoxRects() {
  QRect titleBarRect = ui->titleBarLayout->geometry();
  QRect bottomBarRect = ui->bottomBarLayout->geometry();
  outsideBoxRect_.setRect(
      titleBarRect.x() - lineWidth, titleBarRect.y() - lineWidth,
      bottomBarRect.x() + bottomBarRect.width() - titleBarRect.x() + lineWidth,
      bottomBarRect.y() + bottomBarRect.height() - titleBarRect.y() +
          lineWidth);
  insideBoxRect_.setRect(
      titleBarRect.x() + sideBarWidth, titleBarRect.y() + titleBarRect.height(),
      titleBarRect.width() - 2 * sideBarWidth,
      bottomBarRect.y() - titleBarRect.y() - titleBarRect.height());
}

void RecorderWindow::startRecord() {
  QRect area = insideBoxRect_;
  area.moveTo(mapToGlobal(area.topLeft()));
  screenRecorder_.setArea(area);
  auto currentTime =
      QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.zzz");
  screenRecorder_.setDir(QDir(currentTime));
  screenRecorder_.setFPS(ui->fpsBox->text().toInt());
  screenRecorder_.start();
}

void RecorderWindow::stopRecord() { screenRecorder_.stop(); }

void RecorderWindow::on_startAndPauseButton_clicked() {
  QPushButton *startAndPauseButton = ui->startAndPauseButton;
  QPushButton *stopButton = ui->stopButton;
  switch (screenRecorder_.status()) {
    case ScreenRecorder::Status::Paused:
    case ScreenRecorder::Status::Stopped:
      startRecord();
      stopButton->setEnabled(true);
      startAndPauseButton->setText(tr("Pause"));
      break;
    case ScreenRecorder::Status::Recording:
      screenRecorder_.pause();
      startAndPauseButton->setText(tr("Start"));
      break;
    default:
      break;
  }
}

void RecorderWindow::on_stopButton_clicked() {
  switch (screenRecorder_.status()) {
    case ScreenRecorder::Status::Recording:
    case ScreenRecorder::Status::Paused:
      stopRecord();
      ui->startAndPauseButton->setText(tr("Start"));
      ui->stopButton->setEnabled(false);
      break;
    default:
      break;
  }
}
