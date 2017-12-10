#ifndef OCTREEPALETTE_H
#define OCTREEPALETTE_H
#include "palette.h"
namespace gif {
struct OctreeNode;
class OctreePalette : public Palette {
 public:
  OctreePalette(uint32_t maxColor);
  ~OctreePalette();
  void addColor(QRgb color) override;
  QVector<QRgb> getColorTable() override;
  QImage apply(QImage) const override;
  QByteArray getIndexArray(QImage) const override;

 private:
  static const int COLOR_INDEX_MASK[8];
  OctreeNode* root_;
  std::array<OctreeNode*, 8> reducibleNodes_;
  uint32_t leafCount_;

  void addColor(OctreeNode*& node, QRgb color, int level);
  OctreeNode* createNode(int level);
  void reduceNode();
  QRgb quant(QRgb original) const;
  const OctreeNode* getLeaf(QRgb color) const;
  static void getPaletteColors(OctreeNode* pNode, QVector<QRgb>& palette);
  static void freeNode(OctreeNode*& pNode);
  static void updateIndex(OctreeNode* pNode, uint32_t& index);
};
}
#endif  // OCTREEPALETTE_H
