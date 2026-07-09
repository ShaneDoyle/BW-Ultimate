#ifndef BLACKHACK_SETUPBOX_H
#define BLACKHACK_SETUPBOX_H

#include <assert.h>
#include <stdint.h>


struct SetupBoxCustom;
struct SetupControlCustom;

struct SetupBoxCustom {
  int vftptr = 0x00906AA8;
  float visibilityCopy = 1.0;
  float visibility = 1.0;
  float zoomer1_0xC = 0;
  float zoomer1_0x10 = 0;
  float zoomer1_0x14 = 0;
  float zoomer1_0x18 = 0.5;
  float zoomer1_0x1C = 0.5;
  float zoomer1_0x20 = 0;
  float zoomer1_0x24 = 0;
  float zoomer1_0x28 = 47.99998856;
  float zoomer1_0x2C = -383.9999084;
  float zoomer1_0x30 = 1151.999756;
  float zoomer2_0x34 = 0;
  float zoomer2_0x38 = 0;
  float zoomer2_0x3C = 0;
  float zoomer2_0x40 = 0;
  float zoomer2_0x44 = 0;
  float zoomer2_0x48 = 0;
  float zoomer2_0x4C = 0;
  float zoomer2_0x50 = 0;
  float zoomer2_0x54 = 0;
  float zoomer2_0x58 = 0;
  float zoomer2_0x5C = 0;
  float zoomer2_0x60 = 0;
  uint32_t field_0x64 = 0;
  struct SetupControlCustom* widgets_0x68 = 0x0000000;
  DWORD widget_list = 0x0000000;
  struct SetupControlCustom* focused_widget = 0x0000000;
  struct SetupControlCustom* widget_0x74 = 0x0000000;
  uint8_t field_0x78 = 1;
  uint32_t field_0x7c = 400;
  uint32_t field_0x80 = 300;
  float field_0x84 = 1.0;
  uint32_t field_0x88 = 0;
  uint32_t field_0x8c = 0;
  uint32_t field_0x90 = 0;
  uint32_t windowType = 2;
  uint32_t field_0x98 = 0;
  uint32_t width = 780;
  uint32_t height = 500;
  uint32_t field_0xa4 = 600;
  uint32_t field_0xa8 = 200;
  uint32_t default_text_size = 35;
  void* useCallback;
  uint32_t field_0xb4 = 0;
  uint32_t field_0xb8 = 0;
  struct SetupControlCustom* field_0xbc = 0x0000000;
  int field_0xc0 = 4294967295;
  float field_0xc4 = 1.0;
  uint32_t field_0xc8 = 0;
};

class SetupBoxCustomCreator
{

};

#endif // BLACKHACK_SETUPBOX_H
