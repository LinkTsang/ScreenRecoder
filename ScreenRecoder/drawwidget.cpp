#include "drawwidget.h"
#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QList>
#include <QMessageBox>
#include <QPen>
#include <QtGui>
#include <vector>
#include "editorwindow.h"
DrawWidget::DrawWidget(QWidget *parent) : QWidget(parent) {
  QString fileName = ":/images/title.png";
  QImage image;
  image.load(fileName);
  setAutoFillBackground(true);
  setPalette(QPalette(Qt::white));
  pix = QPixmap(size());
  pix.load(fileName);
  setMinimumSize(pix.width(), pix.height());
}

void DrawWidget::setStyle(int s) { style = s; }
void DrawWidget::setPen() { this->isPen = true; }
void DrawWidget::setBrush() { this->isPen = false; }
bool DrawWidget::next() {
  if (isReadPicture) {
    if (this->now < (imgPath.size() - 1)) {
      this->now++;
      pix = QPixmap(size());
      pix = image[this->now].back();
      resize(pix.size());
      setMinimumSize(pix.width(), pix.height());
      update();
      return true;
    } else {
      qDebug() << "dao le zui hou yiye le" << endl;
      return false;
    }
  }
  return false;
}

bool DrawWidget::before() {
  if (isReadPicture) {
    if (this->now - 1 >= 0) {
      this->now--;
      pix = QPixmap(size());
      pix = image[this->now].back();
      resize(pix.size());
      setMinimumSize(pix.width(), pix.height());
      update();
      return true;
    } else {
      qDebug() << "dao le zui qian yiye le" << endl;
      return false;
    }
  }
  return false;
}

void DrawWidget::setWidth(int w) {
  weight = w;
  fontSize = w;
}

void DrawWidget::setColor(QColor c) { color = c; }

void DrawWidget::mousePressEvent(QMouseEvent *e) { startPos = e->pos(); }

void DrawWidget::mouseMoveEvent(QMouseEvent *e) {
  if (!this->isPen) {
    QPen pen;
    pen.setStyle((Qt::PenStyle)style);
    pen.setWidth((weight));
    pen.setColor(color);
    painter->begin(&pix);
    painter->setPen(pen);
    painter->drawLine(startPos, e->pos());
    painter->end();
    startPos = e->pos();
    update();
  }
}
void DrawWidget::mouseReleaseEvent(QMouseEvent *e) {
  if (!this->isPen) {
    image[this->now].push_back(pix);
    qDebug() << "JI LU CHENGGONG";
  } else {
    int x = e->pos().x();
    int y = e->pos().y();
    if (this->isPen) {
      cancel();
      QPen pen;
      QFont font = painter->font();
      font.setBold(true);                 //加粗
      font.setPixelSize(this->fontSize);  //改变字体大小
      pen.setColor(color);
      painter->begin(&pix);
      painter->setPen(pen);
      painter->setFont(font);
      painter->drawText(x, y, this->text);
      painter->end();
      update();
      image[this->now].push_back(pix);
    }
  }
}

void DrawWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.drawPixmap(QPoint(0, 0), pix);
}
void DrawWidget::cancel() {
  if (!image[this->now].empty() && image[this->now].size() != 1) {
    qDebug() << "CANCEL SUCCEED";

    image[this->now].pop_back();
    if (!image[this->now].empty()) {
      pix = image[this->now].back();
      resize(pix.size());
      setMinimumSize(pix.width(), pix.height());

      update();
    }
  }
}

void DrawWidget::resizeEvent(QResizeEvent *event) {
  if (height() > pix.height() || width() > pix.width()) {
    QPixmap newPix(size());
    newPix.fill(Qt::white);
    QPainter p(&newPix);
    p.drawPixmap(QPoint(0, 0), pix);
    p.end();
    pix = newPix;
  }
  QWidget::resizeEvent(event);
}
int DrawWidget::ImgSize() { return imgPath.size(); }
void DrawWidget::browse() {
  imgPath.clear();
  backImgPath.clear();
  image.clear();
  QString path = QFileDialog::getExistingDirectory(this, tr(""), "");
  if (path.isEmpty()) return;
  QDirIterator iter(path, QDirIterator::NoIteratorFlags);
  while (iter.hasNext()) {
    iter.next();
    QFileInfo info = iter.fileInfo();
    if (info.isFile()) {
      qDebug() << info.absoluteFilePath();
      imgPath.push_back(info.absoluteFilePath());
    }
  }
  this->now = 0;
  this->isReadPicture = true;
  // resize(pix.size());
  pix.load(imgPath[this->now]);
  setMinimumSize(pix.width(), pix.height());
  update();
  image.resize(imgPath.size());
  QPixmap p;
  for (int i = 0; i < image.size(); i++) {
    p.load(imgPath[this->now]);
    image[i].push_back(p);
    this->now++;
  }
  this->now = 0;
  // treeWalk(path);
}
QString DrawWidget::takeFileName() {
  QList<QString> list = imgPath[this->now].split("/");
  return list.back();
}
void DrawWidget::save() {
  QString fileName = this->setNormalPath + "/" + takeFileName();
  if (!findSame(takeFileName())) {
    backImgPath.push_back(imgPath[this->now]);
  }

  qDebug() << fileName;
  qDebug() << pix.save(fileName);
  imgPath[this->now] = fileName;
}
int DrawWidget::showNow() { return this->now; }

void DrawWidget::setPath() {
  setNormalPath = QFileDialog::getExistingDirectory(this, tr(""), "");
}

void DrawWidget::clear() {
  if (isReadPicture) {
    QPixmap clearPix(size());
    clearPix.fill(Qt::white);
    pix = clearPix;
    for (std::size_t i = 0; i < imgPath.size(); i++) {
      qDebug() << imgPath[i] << endl;
    }
    pix = image[this->now][0];
    resize(pix.size());
    setMinimumSize(pix.width(), pix.height());
    update();
  } else {
    QMessageBox msgBox;
    msgBox.setText("Please load images first!");
    msgBox.exec();
  }
}

void DrawWidget::back() {
  QList<QString> list1 = imgPath[this->now].split("/");
  QList<QString> list;
  QString backFileName;
  QString fileName = list1.back();
  if (!backImgPath.empty()) {
    for (int i = 0; i < backImgPath.size(); i++) {
      list = backImgPath[i].split("/");

      backFileName = list.back();
      if (backFileName.compare(fileName) == 0) {
        imgPath[this->now] = backImgPath[i];
        pix.load(imgPath[this->now]);
        update();
        backImgPath.erase(backImgPath.begin() + i);
        break;
      }
    }
  }
}

bool DrawWidget::findSame(QString fileName) {
  for (int i = 0; i < backImgPath.size(); i++) {
    QList<QString> list = backImgPath[i].split("/");

    QString backFileName = list.back();
    if (backFileName.compare(fileName) == 0) {
      return true;
    }
  }
  return false;
}

void DrawWidget::setWorkingDir(QString path) {
  if (path.isEmpty() && !QDir(path).exists()) return;
  QFileInfo info;
  imgPath.clear();
  backImgPath.clear();
  image.clear();
  QDirIterator iter(path, QDirIterator::NoIteratorFlags);
  while (iter.hasNext()) {
    iter.next();
    info = iter.fileInfo();
    if (info.isFile()) {
      qDebug() << info.absoluteFilePath();
      imgPath.push_back(info.absoluteFilePath());
    }
  }

  setNormalPath = info.absolutePath();


  this->now = 0;
  this->isReadPicture = true;
  // resize(pix.size());
  pix.load(imgPath[this->now]);
  setMinimumSize(pix.width(), pix.height());
  update();
  image.resize(imgPath.size());
  QPixmap p;
  for (int i = 0; i < image.size(); i++) {
    p.load(imgPath[this->now]);
    image[i].push_back(p);
    this->now++;
  }
  this->now = 0;
  // treeWalk(path);
}

void DrawWidget::setText(QString Text) {
  this->text = Text;
  if (this->isPen) {
    QPen pen;
    QFont font = painter->font();
    font.setBold(true);                 //加粗
    font.setPixelSize(this->fontSize);  //改变字体大小
    pen.setColor(color);
    painter->begin(&pix);
    painter->setPen(pen);
    painter->setFont(font);
    painter->drawText(100, 100, this->text);
    painter->end();
    update();
    image[this->now].push_back(pix);
  }
}
