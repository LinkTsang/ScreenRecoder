#include <QApplication>
#include "recorderwindow.h"
#include "windowmanager.h"
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  WindowManager::instance().getRecorderWindow().show();
  return a.exec();
}
