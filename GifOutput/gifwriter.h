﻿#ifndef GIFWRITER_H
#define GIFWRITER_H
#include <QDataStream>
#include <QVector>
#include "giftype.h"

namespace gif {
#pragma pack(push, 1)
struct LogicalScreenDescriptor {
  uint16_t width;
  uint16_t height;

  union {
    struct {
      // little endian
      uint8_t globalColorTableSize : 3;
      uint8_t sortFlag : 1;
      uint8_t colorResolution : 3;
      uint8_t globalColorTableFlag : 1;
    };
    uint8_t packedField;
  };

  uint8_t backgroundColorIndex;
  uint8_t pixelAspectRatio;

  LogicalScreenDescriptor(uint16_t width, uint16_t height,
                          uint8_t colorResolution, uint8_t globalColorTableSize)
      : width(width),
        height(height),
        globalColorTableFlag(1),
        colorResolution(colorResolution),
        sortFlag(0),
        globalColorTableSize(globalColorTableSize),
        backgroundColorIndex(0),
        pixelAspectRatio(0) {}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GraphicsControl {
  union {
    struct {
      uint8_t transparentColorFlag : 1;
      uint8_t userInputFlag : 1;
      uint8_t disposalMethod : 3;
      uint8_t reserved : 3;
    };
    uint8_t packedFiled;
  };
  uint16_t delayTime;
  uint8_t transparentColorIndex;

  GraphicsControl() : packedFiled(0), delayTime(0), transparentColorIndex(0) {}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ImageDescriptor {
  uint16_t left;
  uint16_t top;
  uint16_t width;
  uint16_t height;
  union {
    struct {
      uint8_t localColorTableSize : 3;
      uint8_t reserved : 2;
      uint8_t sortFlag : 1;
      uint8_t interlaceFlag : 1;
      uint8_t localColorTableFlag : 1;
    };
    uint8_t packedField;
  };
  ImageDescriptor(uint16_t width, uint16_t height, bool localColorTableFlag,
                  uint8_t localColorTableSize)
      : left(0),
        top(0),
        width(width),
        height(height),
        localColorTableFlag(localColorTableFlag),
        interlaceFlag(0),
        sortFlag(0),
        reserved(0),
        localColorTableSize(localColorTableSize) {}
};
#pragma pack(pop)

class GifWriter {
 public:
  GifWriter(QIODevice *);
  void writeHeader();
  void writeLogicalScreenDescriptor(const LogicalScreenDescriptor &lsd);
  void writeColorTable(const QVector<Color> &colors);

  void writeGraphicsControl(const GraphicsControl &gc);
  void writeImageDescriptor(const ImageDescriptor &id);
  void writeImageData(const char *data, uint32_t len);

  void writeNetscape2_0Extension(uint16_t loop = 0);

  void writeTrailer();

 private:
  QDataStream stream_;
};
}
#endif  // GIFWRITER_H
