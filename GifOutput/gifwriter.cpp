#include "gifwriter.h"
#include <QDataStream>
#include <QIODevice>
#include <QString>

namespace gif {

static const uint8_t EXTENSION_INTRODUCER = '\x21';
static const uint8_t GRAPHIC_CONTROL_LABEL = '\xF9';
static const uint8_t GRAPHIC_CONTROL_SIZE = sizeof(GraphicsControl);
static const uint8_t BLOCK_TERMINATOR = '\0';
static const uint8_t IMAGE_SEPARATOR = '\x2C';
static const uint8_t APPLICATION_EXTENSION_LABEL = '\xFF';
static const uint8_t GIF_TRAILER = '\x3B';
GifWriter::GifWriter(QIODevice* io) : stream_(io) {}

void GifWriter::writeHeader() {
  auto header = "GIF89a";
  stream_.writeRawData(header, qstrlen(header));
}

void GifWriter::writeLogicalScreenDescriptor(const LogicalScreenDescriptor& d) {
  stream_.writeRawData(reinterpret_cast<const char*>(&d),
                       sizeof(LogicalScreenDescriptor));
}

void GifWriter::writeColorTable(const QVector<Color>& colors) {
  for (auto color : colors) {
    stream_ << color.red;
    stream_ << color.green;
    stream_ << color.blue;
  }
}

void GifWriter::writeGraphicsControl(const GraphicsControl& gc) {
  stream_ << EXTENSION_INTRODUCER;
  stream_ << GRAPHIC_CONTROL_LABEL;
  stream_ << GRAPHIC_CONTROL_SIZE;
  stream_.writeRawData(reinterpret_cast<const char*>(&gc),
                       GRAPHIC_CONTROL_SIZE);
  stream_ << BLOCK_TERMINATOR;
}

void GifWriter::writeImageDescriptor(const ImageDescriptor& id) {
  stream_ << IMAGE_SEPARATOR;
  stream_.writeRawData(reinterpret_cast<const char*>(&id), sizeof(id));
}

void GifWriter::writeImageData(const char* data, uint32_t len) {
  stream_.writeRawData(data, len);
}

void GifWriter::writeNetscape2_0Extension(uint16_t loop) {
  stream_ << EXTENSION_INTRODUCER;
  stream_ << APPLICATION_EXTENSION_LABEL;

  const char* applicationBlock = "NETSCAPE2.0";
  uint8_t len = qstrlen(applicationBlock);
  stream_ << len;
  stream_.writeRawData(applicationBlock, len);

  stream_ << (uint8_t)0x03;  // Length of Data Sub-Block
  stream_ << (uint8_t)0x01;  // ???
  stream_ << loop;           // the number of times the loop should be executed.

  stream_ << BLOCK_TERMINATOR;
}

void GifWriter::writeTrailer() { stream_ << GIF_TRAILER; }
}
