#ifndef PALETTE_H
#define PALETTE_H
#include <QImage>
#include <QVector>
#include <array>
#include "giftype.h"

namespace gif {
class Palette {
 public:
  Palette(uint32_t maxColor);
  virtual void addFrame(QImage);
  virtual void addColor(QRgb color) = 0;
  virtual QVector<QRgb> getColorTable() = 0;
  virtual QByteArray getIndexArray(QImage) const = 0;
  virtual QImage apply(QImage) const = 0;
  virtual ~Palette();

 protected:
  uint32_t maxColors_;
};
}

#endif  // PALETTE_H
