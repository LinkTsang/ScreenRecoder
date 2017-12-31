#include "editorwindow.h"
#include <QApplication>
#include <QColorDialog>
#include <QLayout>
#include <QLineEdit>
#include <QPixmap>
#include <QToolBar>
#include <QCloseEvent>
#include "drawwidget.h"
#include "mylabel.h"
#include "windowmanager.h"
#include "outputwindow.h"

EditorWindow::~EditorWindow() {}

EditorWindow::EditorWindow(QWidget *parent) : QMainWindow(parent) {
  // Q_INIT_RESOURCE(res);
  QWidget *q = new QWidget();
  setCentralWidget(q);
  dw = new DrawWidget;
  createToolBar();
  ShowStyle();
  dw->setWidth(widthSpinBox->value());
  dw->setColor(Qt::black);
}

void EditorWindow::createToolBar() {
  setWindowTitle(tr("Editor"));
  QToolBar *toolBar = addToolBar("Tool");
  styleLabel = new QLabel(tr("Line_Style: "));
  styleComboBox = new QComboBox;
  styleComboBox->addItem(tr("SolidLine"), static_cast<int>(Qt::SolidLine));
  styleComboBox->addItem(tr("DashLine"), static_cast<int>(Qt::DashLine));
  styleComboBox->addItem(tr("DotLine"), static_cast<int>(Qt::DotLine));
  styleComboBox->addItem(tr("DashDotLine"), static_cast<int>(Qt::DashDotLine));
  styleComboBox->addItem(tr("DashDotDotLine"),
                         static_cast<int>(Qt::DashDotDotLine));
  connect(styleComboBox, SIGNAL(activated(int)), this, SLOT(ShowStyle()));
  widthLabel = new QLabel(tr("Font_Width: "));
  widthSpinBox = new QSpinBox;
  connect(widthSpinBox, SIGNAL(valueChanged(int)), dw, SLOT(setWidth(int)));
  colorBtn = new QToolButton;
  QPixmap pixmap(20, 20);
  pixmap.fill(Qt::black);
  colorBtn->setIcon(QIcon(pixmap));
  connect(colorBtn, SIGNAL(clicked()), this, SLOT(ShowColor()));
  clearBtn = new QToolButton;
  clearBtn->setText(tr("Clear"));
  connect(clearBtn, SIGNAL(clicked()), dw, SLOT(clear()));
  // nextBtn = new QToolButton;
  // nextBtn ->setText(tr("下一张"));
  takePictureBtn = new QToolButton;
  takePictureBtn->setText(tr("Load"));
  connect(takePictureBtn, SIGNAL(clicked()), this, SLOT(Browse()));
  // beforeBtn = new QToolButton;
  // beforeBtn ->setText(tr("上一张"));
  savePictureBtn = new QToolButton;
  savePictureBtn->setText(tr("Save"));
  connect(savePictureBtn, SIGNAL(clicked()), dw, SLOT(save()));
  setPathBtn = new QToolButton;
  setPathBtn->setText(tr("Set_Save_Path"));
  connect(setPathBtn, SIGNAL(clicked()), dw, SLOT(setPath()));
  backBtn = new QToolButton;
  backBtn->setText(tr("Undo_Save"));
  penBtn = new QToolButton;
  penBtn->setText(tr("Pen"));
  brushBtn = new QToolButton;
  brushBtn->setText(tr("Brush"));
  okBtn = new QToolButton;
  okBtn->setText("OK");
  cancelBtn = new QToolButton;
  cancelBtn->setText("back");
  lineEdit = new QLineEdit;
  lineEdit->setFixedSize(200, 20);
  connect(penBtn, SIGNAL(clicked()), dw, SLOT(setPen()));
  connect(brushBtn, SIGNAL(clicked()), dw, SLOT(setBrush()));
  connect(backBtn, SIGNAL(clicked()), dw, SLOT(back()));
  connect(okBtn, SIGNAL(clicked()), this, SLOT(SetText()));
  connect(cancelBtn, SIGNAL(clicked()), dw, SLOT(cancel()));
  totalImgLabel = new QLabel(tr(u8"图片总数:"));
  totalImg = new QLabel(tr("0"));
  NowImgLabel = new QLabel(tr(u8"当前图片序号:"));
  NowImg = new QLabel(tr(u8"无"));
  NowImgNameLabel = new QLabel(tr(u8"图片名:"));
  NowImgName = new QLabel(tr(u8"无"));

  toolBar->addWidget(styleLabel);
  toolBar->addWidget(styleComboBox);
  toolBar->addWidget(widthLabel);
  toolBar->addWidget(widthSpinBox);
  toolBar->addWidget(colorBtn);

  toolBar->addWidget(setPathBtn);  // toolBar->addWidget(nextBtn);
  // toolBar->addWidget(beforeBtn);
  toolBar->addWidget(takePictureBtn);
  toolBar->addWidget(savePictureBtn);
  toolBar->addWidget(penBtn);
  toolBar->addWidget(brushBtn);
  toolBar->addWidget(backBtn);

  QToolBar *toolBar1 = addToolBar("ImageTool");

  toolBar1->addWidget(cancelBtn);
  toolBar1->addWidget(clearBtn);
  toolBar1->addWidget(lineEdit);
  toolBar1->addWidget(okBtn);
  toolBar1->addWidget(totalImgLabel);
  toolBar1->addWidget(totalImg);
  toolBar1->addWidget(NowImgLabel);
  toolBar1->addWidget(NowImg);
  toolBar1->addWidget(NowImgNameLabel);
  toolBar1->addWidget(NowImgName);
  QVBoxLayout *Tlayout = new QVBoxLayout;

  QHBoxLayout *topLayout = new QHBoxLayout;
  QHBoxLayout *bottomLayout = new QHBoxLayout;
  QHBoxLayout *centralLayout = new QHBoxLayout;
  topLayout->addWidget(toolBar);

  Tlayout->addLayout(topLayout);
  Tlayout->addLayout(centralLayout);
  Tlayout->addLayout(bottomLayout);

  centralWidget()->setLayout(Tlayout);
  MyLabel *next = new MyLabel();
  QPixmap nextPix = QPixmap(":/icon/next_.png");
  qDebug() << QApplication::applicationDirPath();

  next->setPixmap(nextPix);
  MyLabel *before = new MyLabel();
  QPixmap beforePix(":/icon/before_.png");
  next->setPixmap(nextPix);
  before->setPixmap(beforePix);
  centralLayout->addWidget(before);
  centralLayout->addWidget(dw);
  centralLayout->addWidget(next);
  centralLayout->setSpacing(100);
  bottomLayout->addWidget(toolBar1);

  connect(next, SIGNAL(Clicked(MyLabel *)), this, SLOT(Next()));
  connect(before, SIGNAL(Clicked(MyLabel *)), this, SLOT(Before()));
}

void EditorWindow::setWorkingDir(QString path) {
  dw->setWorkingDir(path);
  currentWorkingDir = path;
}

void EditorWindow::keyPressEvent(QKeyEvent *e) {
  switch (e->key()) {
    case Qt::Key_Left:
      Before();
      break;
    case Qt::Key_Right:
      Next();
      break;
  }
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
  this->hide();
  event->ignore();

  auto& outputWindow = WindowManager::getOuputWindow();
  outputWindow.setWorkingDir(currentWorkingDir);
  outputWindow.show();
}

void EditorWindow::ShowStyle() {
  dw->setStyle(
      styleComboBox->itemData(styleComboBox->currentIndex(), Qt::UserRole)
          .toInt());
}
void EditorWindow::SetText() { dw->setText(lineEdit->text()); }
void EditorWindow::ShowColor() {
  QColor color = QColorDialog::getColor(static_cast<int>(Qt::black), this);
  if (color.isValid()) {
    dw->setColor(color);
    QPixmap p(20, 20);
    p.fill(color);
    colorBtn->setIcon(QIcon(p));
  }
}

void EditorWindow::Next() {
  if (dw->next()) {
    qDebug() << dw->width() << dw->height() << endl;
    this->resize(dw->width(), dw->height());
    this->NowImg->setText(QString::number(dw->showNow()));
    this->NowImgName->setText(dw->takeFileName());
  }
}

void EditorWindow::Before() {
  if (dw->before()) {
    qDebug() << dw->width() << dw->height() << endl;
    this->resize(dw->width(), dw->height());
    this->NowImg->setText(QString::number(dw->showNow()));
    this->NowImgName->setText(dw->takeFileName());
  }
}
void EditorWindow::Browse() {
  dw->browse();
  this->totalImg->setText(QString::number(dw->ImgSize()));
  this->NowImg->setText(QString::number(dw->showNow()));
  this->NowImgName->setText(dw->takeFileName());
}
