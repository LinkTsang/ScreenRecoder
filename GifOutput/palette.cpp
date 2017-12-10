#include "palette.h"
#include <QDebug>

namespace gif {

// !-- Palette
Palette::Palette(uint32_t maxColors) : maxColors_(maxColors) {}
Palette::~Palette() {}
void Palette::addFrame(QImage image) {
  image = image.convertToFormat(QImage::Format_RGB32);
  const QRgb* colors = reinterpret_cast<const QRgb*>(image.constBits());
  int count = image.byteCount() / sizeof(QRgb);
  for (int i = 0; i < count; ++i) {
    addColor(colors[i]);
  }
}
// Palette --!

}
