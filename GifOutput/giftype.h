#ifndef GIFTYPE_H
#define GIFTYPE_H
namespace gif {
struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
  Color(uint32_t c)
      : red((c >> 16) & 0xff), green((c >> 8) & 0xff), blue(c & 0xff) {}
};
}
#endif  // GIFTYPE_H
