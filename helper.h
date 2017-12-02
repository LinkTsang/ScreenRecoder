#ifndef HELPER_H
#define HELPER_H

#include <QPixmap>
namespace qScreenRecoder {
struct CursorInfoData;

class CursorInfo {
 public:
  ~CursorInfo();
  CursorInfo(const CursorInfo& other);
  CursorInfo(CursorInfo&& other);

 private:
  friend class ScreenShot;
  CursorInfo(CursorInfoData* data);
  CursorInfoData* data_;
};

class ScreenShot {
 public:
  static QPixmap capture(int x, int y, int w, int h);
  static CursorInfo getCursorInfo();
  static void drawCurosr(QPoint screenshotGlobalPos, QPixmap& pixmap,
                         const CursorInfo& cursorInfo);
};
}
#endif  // HELPER_H
