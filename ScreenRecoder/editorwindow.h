#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QMainWindow>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolButton>
#include "drawwidget.h"

class EditorWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit EditorWindow(QWidget *parent = 0);
  ~EditorWindow();
  void createToolBar();
  void setWorkingDir(QString path);
  void keyPressEvent(QKeyEvent *e) override;
  void closeEvent(QCloseEvent *event) override;
 private slots:
  void ShowStyle();
  void ShowColor();
  void Next();
  void Before();
  void SetText();
  void Browse();

 private:
  DrawWidget *dw;
  QLabel *styleLabel;
  QComboBox *styleComboBox;
  QLabel *widthLabel;
  QSpinBox *widthSpinBox;
  QToolButton *colorBtn;
  QToolButton *clearBtn;
  QToolButton *takePictureBtn;
  QToolButton *savePictureBtn;
  QToolButton *setPathBtn;
  QToolButton *backBtn;
  QToolButton *penBtn;
  QToolButton *brushBtn;
  QLineEdit *lineEdit;
  QToolButton *okBtn;
  QToolButton *cancelBtn;
  QLabel *totalImgLabel;
  QLabel *totalImg;
  QLabel *NowImgLabel;
  QLabel *NowImg;
  QLabel *NowImgNameLabel;
  QLabel *NowImgName;
  QString currentWorkingDir;
};

#endif  // EDITORWINDOW_H
