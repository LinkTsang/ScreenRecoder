#include "palette.h"

namespace gif {
Palette::Palette(uint32_t maxColor) : maxColor_(maxColor) {}

void OctreePalette::addFrame(QImage) {}

QVector<Color> OctreePalette::getColorTable() { return QVector<Color>(); }

QImage OctreePalette::apply(QImage image) const {
  return image.copy(0, 0, image.width(), image.height());
}
}
