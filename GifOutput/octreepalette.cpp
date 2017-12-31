#include "octreepalette.h"
namespace gif {
// !-- OctreeNode
struct OctreeNode {
  std::array<OctreeNode*, 8> children;  // Pointers to child nodes
  OctreeNode* next;                     // Pointer to next reducible node
  uint32_t colorCount;  // Number of pixels represented by this leaf
  uint32_t redSum;      // Sum of red components
  uint32_t greenSum;    // Sum of green components
  uint32_t blueSum;     // Sum of blue components
  uint32_t colorIndex;  // index of current color
  bool isLeaf;  // true if node has no children
  OctreeNode(int level)
      : children{nullptr},
        next(nullptr),
        colorCount(0),
        redSum(0),
        greenSum(0),
        blueSum(0),
        colorIndex(0),
        isLeaf(level == 8) {}
};
// OctreeNode --!

// !-- OctreePalette
const int OctreePalette::COLOR_INDEX_MASK[8] = {0x80, 0x40, 0x20, 0x10,
                                                0x08, 0x04, 0x02, 0x01};

OctreePalette::OctreePalette(uint32_t maxColor)
    : Palette(maxColor),
      root_(nullptr),
      reducibleNodes_{nullptr},
      leafCount_(0) {}

OctreePalette::~OctreePalette() {
  if (root_) freeNode(root_);
}

void OctreePalette::addColor(QRgb color) {
  addColor(root_, color, 0);
  while (leafCount_ > maxColors_) reduceNode();
}

void OctreePalette::addColor(OctreeNode*& node, QRgb color, int level) {
  if (node == nullptr) node = createNode(level);
  int r = qRed(color), g = qGreen(color), b = qBlue(color);
  if (node->isLeaf) {
    ++node->colorCount;
    node->redSum += r;
    node->greenSum += g;
    node->blueSum += b;
  } else {
    int shift = 7 - level;
    int index = (((r & COLOR_INDEX_MASK[level]) >> shift) << 2) |
                (((g & COLOR_INDEX_MASK[level]) >> shift) << 1) |
                ((b & COLOR_INDEX_MASK[level]) >> shift);
    addColor(node->children[index], color, level + 1);
  }
}

OctreeNode* OctreePalette::createNode(int level) {
  OctreeNode* pNode = new OctreeNode(level);
  if (pNode->isLeaf) {
    ++leafCount_;
  } else {
    pNode->next = reducibleNodes_[level];
    reducibleNodes_[level] = pNode;
  }
  return pNode;
}

void OctreePalette::reduceNode() {
  // Find the deepest level containing at least one reducible node
  int i;
  for (i = 7; (i > 0) && (reducibleNodes_[i] == nullptr); --i)
    ;

  // Reduce the node most recently added to the list at level i
  OctreeNode* parent = reducibleNodes_[i];
  reducibleNodes_[i] = parent->next;

  int redSum = 0, greenSum = 0, blueSum = 0, colorCount = 0, childrenCount = 0;
  for (int i = 0; i < 8; ++i) {
    OctreeNode* children = parent->children[i];
    if (children != nullptr) {
      redSum += children->redSum;
      greenSum += children->greenSum;
      blueSum += children->blueSum;
      colorCount += children->colorCount;
      delete children;
      parent->children[i] = nullptr;
      ++childrenCount;
    }
  }
  parent->isLeaf = true;
  parent->redSum = redSum;
  parent->greenSum = greenSum;
  parent->blueSum = blueSum;
  parent->colorCount = colorCount;
  leafCount_ -= (childrenCount - 1);
}

QVector<QRgb> OctreePalette::getColorTable() {
  QVector<QRgb> table;
  if (root_ != nullptr) getPaletteColors(root_, table);
  return table;
}

QImage OctreePalette::apply(QImage image) const {
  QImage result = image.copy(0, 0, image.width(), image.height());
  QRgb* data = reinterpret_cast<QRgb*>(result.bits());
  int count = result.byteCount() / sizeof(QRgb);
  for (int i = 0; i < count; ++i) {
    data[i] = quant(data[i]);
  }
  return result;
}

QByteArray OctreePalette::getIndexArray(QImage image) const {
  uint32_t index = 0;
  updateIndex(root_, index);
  const QRgb* data = reinterpret_cast<const QRgb*>(image.bits());
  int count = image.byteCount() / sizeof(QRgb);
  QByteArray ba(count, Qt::Initialization::Uninitialized);
  for (int i = 0; i < count; ++i) {
    const OctreeNode* node = getLeaf(data[i]);
    ba[i] = node != nullptr ? node->colorIndex : 0;
  }
  return ba;
}

QRgb OctreePalette::quant(QRgb origin) const {
  const OctreeNode* current = getLeaf(origin);
  if (current == nullptr) return 0;
  return (qRgb(current->redSum / current->colorCount,
               current->greenSum / current->colorCount,
               current->blueSum / current->colorCount));
}

// pNode != nullptr
void OctreePalette::freeNode(OctreeNode*& pNode) {
  if (!pNode->isLeaf) {
    for (int i = 0; i < 8; ++i) {
      if (pNode->children[i] != nullptr) {
        freeNode(pNode->children[i]);
      }
    }
    delete pNode;
    pNode = nullptr;
  }
}

// pNode != nullptr
void OctreePalette::updateIndex(OctreeNode* pNode, uint32_t& index) {
  //Q_ASSERT(pNode != nullptr);
  if (pNode->isLeaf) {
    pNode->colorIndex = index;
    index++;
  } else {
    for (int i = 0; i < 8; ++i) {
      if (pNode->children[i] != nullptr) {
        updateIndex(pNode->children[i], index);
      }
    }
  }
}

const OctreeNode* OctreePalette::getLeaf(QRgb color) const {
  const OctreeNode* current = root_;
  for (int level = 0; current != nullptr && !current->isLeaf; ++level) {
    Q_ASSERT(level != 8);
    int shift = 7 - level;
    int r = qRed(color), g = qGreen(color), b = qBlue(color);
    int index = (((r & COLOR_INDEX_MASK[level]) >> shift) << 2) |
                (((g & COLOR_INDEX_MASK[level]) >> shift) << 1) |
                ((b & COLOR_INDEX_MASK[level]) >> shift);
    current = current->children[index];
  }
  return current;
}

// pNode != nullptr
void OctreePalette::getPaletteColors(OctreeNode* pNode,
                                     QVector<QRgb>& palette) {
  if (pNode->isLeaf) {
    palette.push_back(qRgb(pNode->redSum / pNode->colorCount,
                           pNode->greenSum / pNode->colorCount,
                           pNode->blueSum / pNode->colorCount));
  } else {
    for (int i = 0; i < 8; ++i) {
      if (pNode->children[i] != nullptr)
        getPaletteColors(pNode->children[i], palette);
    }
  }
}
// OctreePalette --!
}
