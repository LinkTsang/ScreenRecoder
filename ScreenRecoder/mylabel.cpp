#include <mylabel.h>

MyLabel::MyLabel(QWidget *parent) : QLabel(parent), m_str("") {
  setText(m_str);
}

MyLabel::~MyLabel() {}

void MyLabel::mouseReleaseEvent(QMouseEvent *) { emit Clicked(this); }
