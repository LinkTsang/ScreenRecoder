#include "neu.h"
#include <QDebug>
#include <QImage>

const double NeuQuant::gamma = 1024.0;
const double NeuQuant::beta = 1.0 / 1024.0;
const double NeuQuant::betagamma = NeuQuant::beta * NeuQuant::gamma;
QRgb NeuQuant::getColor(int i) {
    if (i < 0 || i >= netsize) return 0;
    int b = colormap[i][0];
    int g = colormap[i][1];
    int r = colormap[i][2];
    return qRgb(r, g, b);
}

void NeuQuant::setUpArrays() {
    network[0][0] = 0.0;  // black
    network[0][1] = 0.0;
    network[0][2] = 0.0;

    network[1][0] = 255.0;  // white
    network[1][1] = 255.0;
    network[1][2] = 255.0;

    // RESERVED bgColour	// background

    for (int i = 0; i < specials; i++) {
        freq[i] = 1.0 / netsize;
        bias[i] = 0.0;
    }

    for (int i = specials; i < netsize; i++) {
        double* p = network[i];
        p[0] = (255.0 * (i - specials)) / cutnetsize;
        p[1] = (255.0 * (i - specials)) / cutnetsize;
        p[2] = (255.0 * (i - specials)) / cutnetsize;

        freq[i] = 1.0 / netsize;
        bias[i] = 0.0;
    }
}

void NeuQuant::init() {
    learn();
    fix();
    inxbuild();
}

void NeuQuant::altersingle(double alpha, int i, double b, double g, double r) {
    // Move neuron i towards biased (b,g,r) by factor alpha
    double* n = network[i];  // alter hit neuron
    n[0] -= (alpha * (n[0] - b));
    n[1] -= (alpha * (n[1] - g));
    n[2] -= (alpha * (n[2] - r));
}

void NeuQuant::alterneigh(double alpha, int rad, int i, double b, double g,
                          double r) {
    int lo = i - rad;
    if (lo < specials - 1) lo = specials - 1;
    int hi = i + rad;
    if (hi > netsize) hi = netsize;

    int j = i + 1;
    int k = i - 1;
    int q = 0;
    while ((j < hi) || (k > lo)) {
        double a = (alpha * (rad * rad - q * q)) / (rad * rad);
        q++;
        if (j < hi) {
            double* p = network[j];
            p[0] -= (a * (p[0] - b));
            p[1] -= (a * (p[1] - g));
            p[2] -= (a * (p[2] - r));
            j++;
        }
        if (k > lo) {
            double* p = network[k];
            p[0] -= (a * (p[0] - b));
            p[1] -= (a * (p[1] - g));
            p[2] -= (a * (p[2] - r));
            k--;
        }
    }
}

int NeuQuant::contest(double b, double g,
                      double r) {  // Search for biased BGR values
    // finds closest neuron (min dist) and updates freq
    // finds best neuron (min dist-bias) and returns position
    // for frequently chosen neurons, freq[i] is high and bias[i] is negative
    // bias[i] = gamma*((1/netsize)-freq[i])

    double bestd = std::numeric_limits<double>::max();
    double bestbiasd = bestd;
    int bestpos = -1;
    int bestbiaspos = bestpos;

    for (int i = specials; i < netsize; i++) {
        double* n = network[i];
        double dist = n[0] - b;
        if (dist < 0) dist = -dist;
        double a = n[1] - g;
        if (a < 0) a = -a;
        dist += a;
        a = n[2] - r;
        if (a < 0) a = -a;
        dist += a;
        if (dist < bestd) {
            bestd = dist;
            bestpos = i;
        }
        double biasdist = dist - bias[i];
        if (biasdist < bestbiasd) {
            bestbiasd = biasdist;
            bestbiaspos = i;
        }
        freq[i] -= beta * freq[i];
        bias[i] += betagamma * freq[i];
    }
    freq[bestpos] += beta;
    bias[bestpos] -= betagamma;
    return bestbiaspos;
}

int NeuQuant::specialFind(double b, double g, double r) {
    for (int i = 0; i < specials; i++) {
        double* n = network[i];
        if (n[0] == b && n[1] == g && n[2] == r) return i;
    }
    return -1;
}

void NeuQuant::learn() {
    int biasRadius = initBiasRadius;
    int alphadec = 30 + ((samplefac - 1) / 3);

    // int lengthcount = pixels.length;
    int lengthcount = image_.byteCount() / sizeof(QRgb);

    int samplepixels = lengthcount / samplefac;
    int delta = samplepixels / ncycles;
    int alpha = initalpha;

    int i = 0;
    int rad = biasRadius >> radiusbiasshift;
    if (rad <= 1) rad = 0;

    //qDebug() << "beginning 1D learning: samplepixels=" << samplepixels
    //         << " rad=" << rad;

    int step = 0;
    int pos = 0;

    if ((lengthcount % prime1) != 0)
        step = prime1;
    else {
        if ((lengthcount % prime2) != 0)
            step = prime2;
        else {
            if ((lengthcount % prime3) != 0)
                step = prime3;
            else
                step = prime4;
        }
    }

    i = 0;
    QRgb* pixels = reinterpret_cast<QRgb*>(image_.bits());
    while (i < samplepixels) {
        int red = qRed(pixels[pos]);
        int green = qGreen(pixels[pos]);
        int blue = qBlue(pixels[pos]);

        double b = blue;
        double g = green;
        double r = red;

        if (i == 0) {  // remember background colour
            network[bgColour][0] = b;
            network[bgColour][1] = g;
            network[bgColour][2] = r;
        }

        int j = specialFind(b, g, r);
        j = j < 0 ? contest(b, g, r) : j;

        if (j >= specials) {  // don't learn for specials
            double a = (1.0 * alpha) / initalpha;
            altersingle(a, j, b, g, r);
            if (rad > 0) alterneigh(a, rad, j, b, g, r);  // alter neighbours
        }

        pos += step;
        while (pos >= lengthcount) pos -= lengthcount;

        i++;
        if (i % delta == 0) {
            alpha -= alpha / alphadec;
            biasRadius -= biasRadius / radiusdec;
            rad = biasRadius >> radiusbiasshift;
            if (rad <= 1) rad = 0;
        }
    }

    //qDebug() << "finished 1D learning: final alpha="
    //         << ((1.0 * alpha) / initalpha) << "!";
}

void NeuQuant::fix() {
    for (int i = 0; i < netsize; i++) {
        for (int j = 0; j < 3; j++) {
            int x = (int)(0.5 + network[i][j]);
            if (x < 0) x = 0;
            if (x > 255) x = 255;
            colormap[i][j] = x;
        }
        colormap[i][3] = i;
    }
}

void NeuQuant::inxbuild() {
    // Insertion sort of network and building of netindex[0..255]

    int previouscol = 0;
    int startpos = 0;

    for (int i = 0; i < netsize; i++) {
        int* p = colormap[i];
        int* q = nullptr;
        int smallpos = i;
        int smallval = p[1];  // index on g
        // find smallest in i..netsize-1
        for (int j = i + 1; j < netsize; j++) {
            q = colormap[j];
            if (q[1] < smallval) {  // index on g
                smallpos = j;
                smallval = q[1];  // index on g
            }
        }
        q = colormap[smallpos];
        // swap p (i) and q (smallpos) entries
        if (i != smallpos) {
            int j = q[0];
            q[0] = p[0];
            p[0] = j;
            j = q[1];
            q[1] = p[1];
            p[1] = j;
            j = q[2];
            q[2] = p[2];
            p[2] = j;
            j = q[3];
            q[3] = p[3];
            p[3] = j;
        }
        // smallval entry is now in position i
        if (smallval != previouscol) {
            netindex[previouscol] = (startpos + i) >> 1;
            for (int j = previouscol + 1; j < smallval; j++) netindex[j] = i;
            previouscol = smallval;
            startpos = i;
        }
    }
    netindex[previouscol] = (startpos + maxnetpos) >> 1;
    for (int j = previouscol + 1; j < 256; j++)
        netindex[j] = maxnetpos;  // really 256
}

QRgb NeuQuant::convert(QRgb pixel) {
    int alfa = (pixel >> 24) & 0xff;
    int r = (pixel >> 16) & 0xff;
    int g = (pixel >> 8) & 0xff;
    int b = (pixel)&0xff;
    int i = inxsearch(b, g, r);
    int bb = colormap[i][0];
    int gg = colormap[i][1];
    int rr = colormap[i][2];
    return (alfa << 24) | (rr << 16) | (gg << 8) | (bb);
}

int NeuQuant::lookup(QRgb pixel) {
    int r = (pixel >> 16) & 0xff;
    int g = (pixel >> 8) & 0xff;
    int b = (pixel)&0xff;
    int i = inxsearch(b, g, r);
    return i;
}
int NeuQuant::lookup(bool rgb, int x, int g, int y) {
    int i = rgb ? inxsearch(y, g, x) : inxsearch(x, g, y);
    return i;
}

int NeuQuant::inxsearch(int b, int g, int r) {
    // Search for BGR values 0..255 and return colour index
    int bestd = 1000;  // biggest possible dist is 256*3
    int best = -1;
    int i = netindex[g];  // index on g
    int j = i - 1;        // start at netindex[g] and work outwards

    while ((i < netsize) || (j >= 0)) {
        if (i < netsize) {
            int* p = colormap[i];
            int dist = p[1] - g;  // inx key
            if (dist >= bestd)
                i = netsize;  // stop iter
            else {
                if (dist < 0) dist = -dist;
                int a = p[0] - b;
                if (a < 0) a = -a;
                dist += a;
                if (dist < bestd) {
                    a = p[2] - r;
                    if (a < 0) a = -a;
                    dist += a;
                    if (dist < bestd) {
                        bestd = dist;
                        best = i;
                    }
                }
                i++;
            }
        }
        if (j >= 0) {
            int* p = colormap[j];
            int dist = g - p[1];  // inx key - reverse dif
            if (dist >= bestd)
                j = -1;  // stop iter
            else {
                if (dist < 0) dist = -dist;
                int a = p[0] - b;
                if (a < 0) a = -a;
                dist += a;
                if (dist < bestd) {
                    a = p[2] - r;
                    if (a < 0) a = -a;
                    dist += a;
                    if (dist < bestd) {
                        bestd = dist;
                        best = j;
                    }
                }
                j--;
            }
        }
    }
    return best;
}
QVector<QRgb> NeuQuant::getColorTable()
{
    QVector<QRgb> palette;

    for(int i=0;i<netsize;i++)
    {
        palette.append(getColor(i));
    }
    return palette;
}
QByteArray NeuQuant::getIndexArray(QImage image)
{
    QRgb* pixels = reinterpret_cast<QRgb*>(image.bits());
    int count = image.byteCount() / sizeof(QRgb);
    QByteArray indexArray(count, Qt::Initialization::Uninitialized);
    for(int i=0;i<count;i++)
    {
        indexArray[i] =(uint32_t)lookup(pixels[i]);
    }
    return indexArray;
}
