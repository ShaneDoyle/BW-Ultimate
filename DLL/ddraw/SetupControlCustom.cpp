#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <ctime>
#include "SetupControlCustom.h"
#include "SetupBoxCustom.h"




// Tab-Panel, like in statistics, options or main menu
SetupControlCustom CreateSetupTabButtonCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD focus, DWORD _color) {
	SetupControlCustom newControl;
	newControl.setupControlTypCi = SetupTabButtonCi;
	newControl.text_size = 22;
	wcscpy(newControl.label, textLabel);
	//mbstowcs(newControl.label, textLabel, 0x100);
	//newControl.label = textLabel;
	newControl.field_0x22a = 1;
	newControl.next = nextSetupControl;
	newControl.setup_box = _setupBox;
	newControl.continue_button_callback = _callfunction;
	newControl.left = left;
	newControl.top = top;
	newControl.right = right;
	newControl.bottom = bottom;
	newControl.tabButtonFocus = focus;
	newControl._color = _color;
	return newControl;
}

// Checkbox
SetupControlCustom CreateSetupCheckBoxCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD textregion, int _checked) {
	SetupControlCustom newControl;
	newControl.setupControlTypCi = SetupCheckBoxCi;
	newControl.text_size = 22;
	wcscpy(newControl.label, textLabel);
	//mbstowcs(newControl.label, textLabel, 0x100);
	//newControl.label = textLabel;
	newControl.field_0x22a = 1;
	newControl.next = nextSetupControl;
	newControl.setup_box = _setupBox;
	newControl.continue_button_callback = _callfunction;
	newControl.left = left;
	newControl.top = top;
	newControl.right = right;
	newControl.bottom = bottom;
	newControl.tabButtonFocus = textregion;
	newControl.checked = _checked;
	return newControl;
}

// BigButton
SetupControlCustom CreateSetupBigButtonCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD textregion, int buttonType) {
	SetupControlCustom newControl;
	newControl.setupControlTypCi = SetupBigButtonCi;
	newControl.text_size = 22;
	wcscpy(newControl.label, textLabel);
	//mbstowcs(newControl.label, textLabel, 0x100);
	//newControl.label = textLabel;
	newControl.field_0x22a = 1;
	newControl.next = nextSetupControl;
	newControl.setup_box = _setupBox;
	newControl.continue_button_callback = _callfunction;
	newControl.left = left;
	newControl.top = top;
	newControl.right = right;
	newControl.bottom = bottom;
	newControl.tabButtonFocus = textregion;
	newControl.checked = buttonType;
	return newControl;
}



int _setval25C = 26;
int _setval260 = 0;
// Listview
SetupControlCustom CreateSetupMultiListCi(int left, int top, int right, int bottom, DWORD nextSetupControl, DWORD _setupBox, int _numEntries, DWORD _firstEntryLabel, DWORD _firstEntryValue, DWORD _val25C, DWORD _val260, DWORD _val268, DWORD _val26C, DWORD _scrollBarScroller, DWORD _scrollBarEnd, void* _callfunction) {
/*
	DWORD* menuValPointer1 = (DWORD*)(0x00D4AB44);
	DWORD* menuValPointer2 = (DWORD*)(menuValPointer1[0] + 0x34);
	DWORD* menuValPointer25C = (DWORD*)(menuValPointer2[0] + 0x25C);
	DWORD* menuValPointer258 = (DWORD*)(menuValPointer2[0] + 0x258);
	DWORD* menuValPointer260 = (DWORD*)(menuValPointer2[0] + 0x260);
	DWORD* menuValPointer264 = (DWORD*)(menuValPointer2[0] + 0x264);
	printf("menuValPointer25C: %x\n", menuValPointer25C[0]);
	printf("menuValPointer260: %x\n", menuValPointer260[0]);
	*/
	SetupControlCustom newControl;
	newControl.setupControlTypCi = SetupMultiListCi;
	newControl.text_size = 35;
	//newControl.label = textLabel;
	newControl.field_0x22a = 1;
	newControl.next = nextSetupControl;
	newControl.setup_box = _setupBox;
	newControl.left = left;
	newControl.top = top;
	newControl.right = right;
	newControl.bottom = bottom;
	newControl.tabButtonFocus = 0;
	newControl.checked = 1;
	newControl._color = _numEntries;
	newControl.id = 999;
	newControl.field_0x23C = 1;
	newControl.field_0x240 = 24;
	newControl.field_0x254 = 27;
	newControl.field_0x258 = _firstEntryLabel;
	newControl.field_0x25C = _val25C;
	newControl.field_0x260 = _val260;
	newControl.field_0x268 = _val268;
	newControl.field_0x26C = _val26C;
	newControl.scrollBarScroller = _scrollBarScroller;
	newControl.scrollBarEnd = _firstEntryValue;
	newControl.continue_button_callback = _callfunction;
	newControl.scrollBarOn = 1;
	newControl.field_0x27C = 0;
	return newControl;
}

// Textline
SetupControlCustom CreateSetupStaticTextCi(int left, int top, int right, int bottom, wchar_t* textLabel, DWORD nextSetupControl, DWORD _setupBox, void* _callfunction, DWORD textregion, int id, int textsize) {
	SetupControlCustom newControl;
	newControl.setupControlTypCi = SetupStaticTextCi;
	newControl.text_size = textsize;
	wcscpy(newControl.label, textLabel);
	//mbstowcs(newControl.label, textLabel, 0x100);
	newControl.field_0x22a = 1;
	newControl.next = nextSetupControl;
	newControl.setup_box = _setupBox;
	newControl.continue_button_callback = _callfunction;
	newControl.left = left;
	newControl.top = top;
	newControl.right = right;
	newControl.bottom = bottom;
	newControl.id = id;
	newControl.field_0x23C = textregion;		//Textausrichtung, 0 = Links, 1 = Mitte, 2 = rechts



	return newControl;
}