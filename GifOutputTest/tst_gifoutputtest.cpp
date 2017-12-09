#include <QFile>
#include <QString>
#include <QtTest>
#include "gifwriter.h"

class GifOutputTest : public QObject {
  Q_OBJECT

 public:
  GifOutputTest();

 private Q_SLOTS:
  void testCase1();
};

GifOutputTest::GifOutputTest() {}

void GifOutputTest::testCase1() {
  const char excepted[] = {
      '\x47', '\x49', '\x46', '\x38', '\x39', '\x61', '\x0A', '\x00', '\x0A',
      '\x00', '\x91', '\x00', '\x00', '\xFF', '\xFF', '\xFF', '\xFF', '\x00',
      '\x00', '\x00', '\x00', '\xFF', '\x00', '\x00', '\x00', '\x21', '\xF9',
      '\x04', '\x00', '\x00', '\x00', '\x00', '\x00', '\x2C', '\x00', '\x00',
      '\x00', '\x00', '\x0A', '\x00', '\x0A', '\x00', '\x00', '\x02', '\x16',
      '\x8C', '\x2D', '\x99', '\x87', '\x2A', '\x1C', '\xDC', '\x33', '\xA0',
      '\x02', '\x75', '\xEC', '\x95', '\xFA', '\xA8', '\xDE', '\x60', '\x8C',
      '\x04', '\x91', '\x4C', '\x01', '\x00', '\x21', '\xFF', '\x0B', '\x4E',
      '\x45', '\x54', '\x53', '\x43', '\x41', '\x50', '\x45', '\x32', '\x2E',
      '\x30', '\x03', '\x01', '\x00', '\x00', '\x00', '\x3B'};

  QBuffer buffer;
  buffer.open(QBuffer::WriteOnly);
  gif::GifWriter writer(&buffer);
  gif::LogicalScreenDescriptor desc(10, 10, 1, 1);
  QVector<gif::Color> table{0xffffff, 0xff0000, 0x0000ff, 0x000000};
  gif::ImageDescriptor id(10, 10, 0, 0);
  const char imageData[] = {
      '\x02', '\x16', '\x8C', '\x2D', '\x99', '\x87', '\x2A', '\x1C', '\xDC',
      '\x33', '\xA0', '\x02', '\x75', '\xEC', '\x95', '\xFA', '\xA8', '\xDE',
      '\x60', '\x8C', '\x04', '\x91', '\x4C', '\x01', '\x00'};

  writer.writeHeader();
  writer.writeLogicalScreenDescriptor(desc);
  writer.writeColorTable(table);
  writer.writeGraphicsControl(gif::GraphicsControl());
  writer.writeImageDescriptor(id);
  writer.writeImageData(imageData, sizeof(imageData) / sizeof(*imageData));
  writer.writeNetscape2_0Extension();
  writer.writeTrailer();

  buffer.close();
  QByteArray result = buffer.data();
  int exceptedLength = sizeof(excepted) / sizeof(*excepted);
  QVERIFY(exceptedLength == result.length());
  for (int i = 0; i < exceptedLength; ++i) {
    QVERIFY(excepted[i] == result[i]);
  }
}

QTEST_APPLESS_MAIN(GifOutputTest)

#include "tst_gifoutputtest.moc"
