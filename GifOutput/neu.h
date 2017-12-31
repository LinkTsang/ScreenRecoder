#ifndef NEUQUANTIZER_H
#define NEUQUANTIZER_H

#include <QColor>
#include <QImage>
#include "palette.h"

class NeuQuant{
 private:
  static const int ncycles = 100;  // no. of learning cycles

  static const int netsize = 256;            // number of colours used
  static const int specials = 3;             // number of reserved colours used
  static const int bgColour = specials - 1;  // reserved background colour
  static const int cutnetsize = netsize - specials;
  static const int maxnetpos = netsize - 1;

  static const int initrad = netsize / 8;  // for 256 cols, radius starts at 32
  static const int radiusbiasshift = 6;
  static const int radiusbias = 1 << radiusbiasshift;
  static const int initBiasRadius = initrad * radiusbias;
  static const int radiusdec = 30;  // factor of 1/30 each cycle

  static const int alphabiasshift = 10;              // alpha starts at 1
  static const int initalpha = 1 << alphabiasshift;  // biased by 10 bits

  static const double gamma;
  static const double beta;
  static const double betagamma;

  static const int prime1 = 499;
  static const int prime2 = 491;
  static const int prime3 = 487;
  static const int prime4 = 503;
  static const int maxprime = prime4;

  double network[netsize][3];  // the network itself
  int colormap[netsize][4];    // the network itself
  int netindex[256];           // for network lookup - really 256
  double bias[netsize];        // bias and freq arrays for learning
  double freq[netsize];

  int* pixels = nullptr;
  QImage image_;
  int samplefac = 0;

 public:
  NeuQuant(QImage image, int sample = 10) : image_(image), samplefac(sample) {
    Q_ASSERT(sample >= 1 && sample <= 30);
    setUpArrays();
  }

  QVector<QRgb> getColorTable();
  QByteArray getIndexArray(QImage);

  void init();
  int getColorCount() { return netsize; }
  QRgb convert(QRgb pixel);

private:
  int lookup(QRgb pixel);
  int lookup(bool rgb, int x, int g, int y);
  int inxsearch(int b, int g, int r);
  void altersingle(double alpha, int i, double b, double g, double r);
  void alterneigh(double alpha, int rad, int i, double b, double g, double r);
  int contest(double b, double g, double r);
  int specialFind(double b, double g, double r);
  void learn();
  void fix();
  void inxbuild();

  QRgb getColor(int i);
  void setUpArrays();
};

#endif  // NEUQUANTIZER_H
