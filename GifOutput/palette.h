#ifndef PALETTE_H
#define PALETTE_H
#include <QVector>
#include <QImage>
#include "giftype.h"

namespace gif {
class Palette {
 public:
  Palette(uint32_t maxColor);
  virtual void addFrame(QImage) = 0;
  virtual QVector<Color> getColorTable() = 0;
  virtual QImage apply(QImage) const = 0;

 private:
  uint32_t maxColor_;
};

class OctreePalette : public Palette {
 public:
  OctreePalette(uint32_t maxColor);
  void addFrame(QImage) override;
  QVector<Color> getColorTable() override;
  QImage apply(QImage) const override;
};
}

#endif  // PALETTE_H
