#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H
#include <QColor>
#include <QDir>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPoint>
#include <QResizeEvent>
#include <QWidget>
#include <QtGui>
#include <vector>

class DrawWidget : public QWidget {
  Q_OBJECT
 public:
  explicit DrawWidget(QWidget *parent = nullptr);
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  bool next();
  bool before();
  bool findSame(QString fileName);
  void setWorkingDir(QString path);
  int showNow();
  int ImgSize();
  QString takeFileName();
  int n = 5;
 signals:

 public slots:
  void setStyle(int);
  void setWidth(int);
  void setColor(QColor);
  void clear();
  void setPen();
  void setBrush();
  void save();
  void browse();
  void setPath();
  void back();
  void setText(QString text);
  void cancel();

 private:
  QPixmap pix;
  QPoint startPos;
  QPoint endPos;
  int style;
  int weight;
  QColor color;
  int imgCount;
  std::vector<QString> imgPath;
  std::vector<QString> backImgPath;
  std::vector<std::vector<QPixmap>> image;
  int now = 0;
  QString setNormalPath = QDir::currentPath();
  bool isReadPicture = false;
  bool isPen = false;
  QPainter *painter = new QPainter;
  QString text = "text";
  int fontSize;
};

#endif  // DRAWWIDGET_H
