#ifndef BLACKHACK_SETUPCONTROL_H
#define BLACKHACK_SETUPCONTROL_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

struct SetupControlCustom;

enum setupControlTyp {
	SetupControlCi = 0xFFFF4646,
	SetupSliderCi = 0xFF47FF54,
	SetupEditCi = 0xFFE347FF,
	SetupStaticTextCi = 0x008D6638,					//
	SetupStaticTextNoHitCi = 0xFF47F9FF,
	SetupListCi = 0xFF47F9FF,
	SetupMultiListCi	= 0x008D639C,				//
	SetupButtonCi = 0x008D632C,						//
	SetupHLineGraphCi = 0xFF47F9FF,
	SetupBigButtonCi	= 0x008D6458,				// FERTIG
	SetupTabButtonCi	= 0x008D6548,				// FERTIG
	SetupPictureCi		= 0x008D6458,
	SetupCheckBoxCi		= 0x008D6600,				// FERTIG
	SetupColourPickerCi = 0xFF47F9FF,
	SetupMP3ButtonCi = 0xFF47F9FF,
	SetupVBarGraphCi = 0x008D649C,					//
	SetupHSBarGraphCi = 0x008D64F8					//
};

enum _buttonType {
	_typeUnchecked = 0,
	_typeChecked = 1,
	_typeLeftArrow = 2,
	_typeRightArrow = 3,
	_typeRotateButton = 4,
	_typeSpeechBubble = 5,
	_typeSpeechBubbleCross = 6,
	_typeSpeechBubbleBack = 7,
	_typeSpeechBubbleFlash = 8,
	_typeMailBack = 9,
	_typeMail = 10,
	_typeSun = 11,
	_typeSunCloud = 12,
	_typeCloud = 13,
	_typeSunRainCloud = 14,
	_typeRainCloud = 15,
	_typeSunSnowCloud = 16,
	_typeSnowCloud = 17,
	_typeSnowRainCloud = 18,
	_typeThunderRainCloud = 19,
	_typeColoredHand = 20
};
struct SetupSliderControl {
	uint32_t setupControlTypCi;      // 0x000
	uint32_t field_0x4;              // 0x004
	uint32_t left;                   // 0x008
	uint32_t top;                    // 0x00C
	uint32_t right;                  // 0x010
	uint32_t bottom;                 // 0x014
	int id;                          // 0x018
	int field_0x1c;                  // 0x01C
	int text_size;                   // 0x020
	wchar_t label[0x100];            // 0x024
	const wchar_t* tooltip;          // 0x224
	bool focus;                      // 0x228
	bool hidden;                     // 0x229
	bool field_0x22a;                // 0x22A
	bool field_0x22b;                // 0x22B
	uint32_t field_0x22c;            // 0x22C
	DWORD next;                      // 0x230
	DWORD setup_box;                 // 0x234
	void* continue_button_callback;  // 0x238
	float value;                     // 0x23C
	float value_copy;                // 0x240
	uint32_t range;                  // 0x244
	uint32_t field_0x248;            // 0x248
	uint32_t field_0x24C;            // 0x24C
};                                   // = 592 bytes


struct SetupControlCustom {
  uint32_t setupControlTypCi;
  uint32_t field_0x4 = 0x0;
  uint32_t left;
  uint32_t top;
  uint32_t right;
  uint32_t bottom;
  int id = 0;
  int field_0x1c = 0;
  int text_size = 35;
  wchar_t label[0x100];
  const wchar_t* tooltip = 0x0;
  bool focus = 0;
  bool hidden = 0;
  bool field_0x22a;
  bool field_0x22b = 0;
  uint32_t field_0x22c = 0;
  DWORD next;
  DWORD setup_box = 0;
  void* continue_button_callback = 0;
  uint32_t field_0x23C = 0;
  uint32_t field_0x240 = 0;
  uint32_t tabButtonFocus;
  uint32_t checked = 0;
  uint32_t field_0x24C = 0;
  uint32_t _color = 0xFFFFFFFF;
  uint32_t field_0x254 = 0;
  uint32_t field_0x258 = 0;
  uint32_t field_0x25C = 0;
  uint32_t field_0x260 = 0;
  uint32_t field_0x264 = 0;
  uint32_t field_0x268 = 0;
  uint32_t field_0x26C = 0;
  uint32_t scrollBarScroller = 2860;
  uint32_t scrollBarOn = 1;
  uint32_t scrollBarEnd = 1440;
  uint32_t field_0x27C = 0;
  uint32_t field_0x280 = 0;
  uint32_t field_0x284 = 0;
  uint32_t field_0x288 = 297;
  uint32_t field_0x28C = 463;
  uint32_t field_0x290 = 256;
  uint32_t borderColor = 0xFFFFFFFF;
  uint32_t choosenColor = 0xFFFFFFFF;
  uint32_t field_0x29C = 0x00000000;
  uint32_t field_0x2A0 = 40;
  uint32_t field_0x2A4 = 443;
  uint32_t field_0x2A8 = 734;
  uint32_t field_0x2AC = 469;
  uint32_t field_0x2B0 = 0;
};

struct SetupMultiListItemDetail {
	uint32_t numEntries = 1;
	uint32_t entry1 = 0;
	uint32_t entry2 = 0;
	uint32_t entry3 = 0;
	uint32_t entry4 = 0;
	uint32_t entry5 = 0;
	uint32_t entry6 = 0;
	uint32_t entry7 = 0;
	uint32_t entry8 = 0;
	uint32_t color1 = 0;
	uint32_t color2 = 0;
	uint32_t color3 = 0;
	uint32_t color4 = 0;
	uint32_t color5 = 0;
	uint32_t color6 = 0;
	uint32_t color7 = 0;
	uint32_t color8 = 0;
	uint32_t unknown1 = 0;
	uint32_t unknown2 = 0;
	uint32_t unknown3 = 3;
	uint32_t unknown4 = 1;
	wchar_t label[16];
	uint32_t unknown5 = 1;
};

SetupControlCustom CreateSetupTabButtonCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD focus, DWORD _color = 0xFFFFFFFF);
SetupControlCustom CreateSetupCheckBoxCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD textregion, int _checked = 0);
SetupControlCustom CreateSetupBigButtonCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD textregion, int buttonType = 0);
SetupControlCustom CreateSetupMultiListCi(int left, int top, int right, int bottom, DWORD nextSetupControl, DWORD _setupBox, int _numEntries, DWORD _firstEntryLabel, DWORD _firstEntryValue, DWORD _val25C, DWORD _val260, DWORD _val268, DWORD _val26C, DWORD _scrollBarScroller, DWORD _scrollBarEnd, void* _callfunction);
SetupControlCustom CreateSetupStaticTextCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD textregion, int id, int textsize);
#endif // BLACKHACK_SETUPCONTROL_H
