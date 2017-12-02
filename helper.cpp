#include "helper.h"
#include <Windows.h>
#include <QApplication>
#include <QScreen>
#include <QtWin>
#include <QDebug>
namespace qScreenRecoder {
struct CursorInfoData {
  CursorInfoData() { cursorInfo.cbSize = sizeof(cursorInfo); }
  CURSORINFO cursorInfo;
};

CursorInfo::~CursorInfo() {
  if (data_ != nullptr) delete data_;
}

CursorInfo::CursorInfo(CursorInfo &&other) : data_(std::move(other.data_)) {
  other.data_ = nullptr;
}

CursorInfo::CursorInfo(const CursorInfo &other) {
  if (this != &other) {
    data_ = new CursorInfoData;
    data_->cursorInfo = other.data_->cursorInfo;
  }
}

CursorInfo::CursorInfo(CursorInfoData *data) : data_(data) {}

QPixmap ScreenShot::capture(int x, int y, int w, int h) {
  QScreen *screen = QApplication::primaryScreen();
  return screen->grabWindow(0, x, y, w, h);
}

CursorInfo ScreenShot::getCursorInfo() {
  CursorInfoData *cursorInfo = new CursorInfoData;
  if (::GetCursorInfo(&(cursorInfo->cursorInfo))) {
    return CursorInfo(cursorInfo);
  } else {
    delete cursorInfo;
    return CursorInfo(nullptr);
  }
}

void ScreenShot::drawCurosr(QPoint screenshotGlobalPos, QPixmap &pixmap,
                            const CursorInfo &cursorInfo) {
  HDC hScreenDC = GetDC(NULL);
  HDC hDest = ::CreateCompatibleDC(hScreenDC);
  HBITMAP hBitmap = QtWin::toHBITMAP(pixmap);
  HGDIOBJ hOldBitmap = ::SelectObject(hDest, hBitmap);

  if (!cursorInfo.data_) return;
  CURSORINFO info = cursorInfo.data_->cursorInfo;
  if (info.flags == CURSOR_SHOWING) {
    HICON hIcon = ::CopyIcon(info.hCursor);
    if (hIcon != NULL) {
      ICONINFO iconInfo;
      if (GetIconInfo(hIcon, &iconInfo)) {
        int cursorPosX =
            info.ptScreenPos.x - screenshotGlobalPos.x() - iconInfo.xHotspot;
        int cursorPosY =
            info.ptScreenPos.y - screenshotGlobalPos.y() - iconInfo.yHotspot;
        ::DrawIconEx(hDest, cursorPosX, cursorPosY, info.hCursor,
                     0,     // cxWidth
                     0,     // cyWidth
                     0,     // istepIfAniCur
                     NULL,  // hbrFlickerFreeDraw
                     DI_NORMAL);
      }
      ::DeleteObject(iconInfo.hbmColor);
      ::DeleteObject(iconInfo.hbmMask);
      ::DestroyIcon(hIcon);
    }
  }
  ::DeleteObject(info.hCursor);

  pixmap = QtWin::fromHBITMAP(hBitmap);

  ::SelectObject(hDest, hOldBitmap);
  ::DeleteObject(hOldBitmap);
  ::DeleteObject(hBitmap);
  ::DeleteDC(hDest);
  ::ReleaseDC(NULL, hScreenDC);
}
}
