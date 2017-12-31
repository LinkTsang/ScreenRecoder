#ifndef MYLABEL_H_
#define MYLABEL_H_

#include <QLabel>
#include <QWidget>

class MyLabel : public QLabel {
  Q_OBJECT

 public:
  MyLabel(QWidget* parent = 0);
  ~MyLabel();

 signals:
  void Clicked(MyLabel* clicked);

 protected:
  void mouseReleaseEvent(QMouseEvent*);

 private:
  QString m_str;
};

#endif // MYLABEL_H_
