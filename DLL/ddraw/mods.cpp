#include "mods.h"

#include <iostream>
#include <filesystem>

#include <Windows.h>
#include <detours.h>
#include <KnownFolders.h>
#include <ShlObj.h>

#include "bwfuncs.h"
#include <iostream>
#include <windows.h>

#include <iostream>
#include <fstream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

int i = 0;
PlayerSymbolSprite* VillageCentrePointers[100];
int VillageCentrePlayers[100];
int TownCentreSize = 0;
bool UpdateVillageCentre = false;
int CHL_ToLoad = 0; //0 Ultimate, 1 Creature Isle

//Needs to be cleaned vars.
int Player1Templetype = -1;


//CHL_FUNCTIONS
//////////////////////////////////////////////////
enum Colours {
	RED = 0xFFFF4646,
	GREEN = 0xFF47FF54,
	PURPLE = 0xFFE347FF,
	SKYBLUE = 0xFF47F9FF,
	YELLOW = 0xFFFFFD47,
	BLUE = 0xFF4777FF,
	ORANGE = 0xFFFFA247,
	BRED = 0x00FF4646,
	BGREEN = 0x0047FF54,
	BPURPLE = 0x00E347FF,
	BSKYBLUE = 0x0047F9FF,
	BYELLOW = 0x00FFFD47,
	BBLUE = 0x004777FF,
	BORANGE = 0x00FFA247,
};

bool IsCI() {
	DWORD* PartToCheck = (DWORD*)(0x00800000);
	return (*PartToCheck == 0xD4B95308); //0x14244489 in B&W
}

//Set a registry value.
void setReg(CHAR* keyName, DWORD value)
{
	HKEY hKeys;
	HKEY hKey;
	char regFolder[300];
	DWORD* dllBase = (DWORD*)GetModuleHandleA("LHMultiplayerR.DLL");
	char* profilId = (char*)(IsCI() ? dllBase + (0x0006B288 / 4) : dllBase + (0x0006B378 / 4));
	sprintf(regFolder, "Software\\Lionhead Studios Ltd\\Black & White\\LHMultiplayer\\ProfilesCI\\%s", profilId);

	LPCTSTR sk = TEXT(regFolder);

	//Create key if it doesn't exist.
	RegCreateKeyEx(
		HKEY_CURRENT_USER,
		sk,
		0,
		NULL,
		REG_OPTION_BACKUP_RESTORE,
		KEY_ALL_ACCESS,
		NULL,
		&hKeys,
		NULL);

	//Load key if available.
	RegOpenKeyEx(HKEY_CURRENT_USER,
		sk,
		0, KEY_WRITE, &hKey);
	RegSetValueEx(hKey, TEXT(keyName), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
}


//Read a registry value.
DWORD getReg(CHAR* keyName)
{
	HKEY hKey;
	DWORD dwType = REG_DWORD;
	DWORD value = 0;
	DWORD lengh = sizeof(value);
	char regFolder[300];
	DWORD* dllBase = (DWORD*)GetModuleHandleA("LHMultiplayerR.DLL");
	char* profilId = (char*)(dllBase + (0x0006B288 / 4));
	sprintf(regFolder, "Software\\Lionhead Studios Ltd\\Black & White\\LHMultiplayer\\ProfilesCI\\%s", profilId);

	LPCTSTR sk = TEXT(regFolder);
	RegOpenKey(HKEY_CURRENT_USER, sk, &hKey);
	RegQueryValueEx(hKey, keyName, NULL, NULL, (LPBYTE)&value, &lengh);
	RegCloseKey(hKey);
	return value;
}

//Get all the player colours.
DWORD* GetPlayerColours()
{
	return (DWORD*)(0x00C6FB88);
}

//Get all the player influence colours.
DWORD* GetPlayerInfluenceColours()
{
	return (DWORD*)(0x00F2E85C);
}

//Set colours for all players.
void SetPlayerColours(DWORD P1, DWORD P2, DWORD P3, DWORD P4, DWORD P5, DWORD P6, DWORD P7)
{
	DWORD* PlayerColours = GetPlayerColours();
	PlayerColours[0] = P1;
	PlayerColours[1] = P2;
	PlayerColours[2] = P3;
	PlayerColours[3] = P4;
	PlayerColours[4] = P5;
	PlayerColours[5] = P6;
	PlayerColours[6] = P7;
}

//Set influence colours for all players.
void SetPlayerInfluenceColours(DWORD P1, DWORD P2, DWORD P3, DWORD P4, DWORD P5, DWORD P6, DWORD P7)
{
	DWORD* PlayerInfluenceColours = GetPlayerInfluenceColours();
	PlayerInfluenceColours[0] = P1;
	PlayerInfluenceColours[1] = P2;
	PlayerInfluenceColours[2] = P3;
	PlayerInfluenceColours[3] = P4;
	PlayerInfluenceColours[4] = P5;
	PlayerInfluenceColours[5] = P6;
	PlayerInfluenceColours[6] = P7;
}

//Set a Player's temple design.
void SetTemples2(const char* Player, const char* DefaultD) {
	char* Evil = nullptr;
	char* Neutral = nullptr;
	char* Good = nullptr;
	int Skip = 0;
	if (Player == "Player1"){
		Evil = (char*)(IsCI() ? 0x00CAD10B : 0x00CAD116);
		Neutral = (char*)(IsCI() ? 0x00CAD14E : 0x00CAD159);
		Good = (char*)(IsCI() ? 0x00CAD18B : 0x00CAD196);
	}
	else if (Player == "Player2") {
		Evil = (char*)(IsCI() ? 0x00CAD1CB : 0x00CAD1D6);
		Neutral = (char*)(IsCI() ? 0x00CAD20E : 0x00CAD219);
		Good = (char*)(IsCI() ? 0x00CAD24B : 0x00CAD256);
	}
	else if (Player == "Player3") {
		Evil = (char*)(IsCI() ? 0x00CAD28B : 0x00CAD296);
		Neutral = (char*)(IsCI() ? 0x00CAD2CE : 0x00CAD2D9);
		Good = (char*)(IsCI() ? 0x00CAD30B : 0x00CAD316);
	}
	else if (Player == "Player4") {
		Evil = (char*)(IsCI() ? 0x00CAD34B : 0x00CAD356);
		Neutral = (char*)(IsCI() ? 0x00CAD38E : 0x00CAD399);
		Good = (char*)(IsCI() ? 0x00CAD3CB : 0x00CAD3D6);
	}
	else if (Player == "Player5") {
		Evil = (char*)(IsCI() ? 0x00CAD40B : 0x00CAD416);
		Neutral = (char*)(IsCI() ? 0x00CAD44E : 0x00CAD459);
		Good = (char*)(IsCI() ? 0x00CAD48B : 0x00CAD496);
	}
	else if (Player == "Player6") {
		Evil = (char*)(IsCI() ? 0x00CAD4CB : 0x00CAD4D6);
		Neutral = (char*)(IsCI() ? 0x00CAD50E : 0x00CAD519);
		Good = (char*)(IsCI() ? 0x00CAD54B : 0x00CAD556);
	}
	else if (Player == "Player7") {
		Evil = (char*)(IsCI() ? 0x00CAD58B : 0x00CAD596);
		Neutral = (char*)(IsCI() ? 0x00CAD5CE : 0x00CAD5D9);
		Good = (char*)(IsCI() ? 0x00CAD60B : 0x00CAD616);
	}
	else if (Player == "Neutral") {
		Evil = (char*)(IsCI() ? 0x00CAD64B : 0x00CAD656);
		Neutral = (char*)(IsCI() ? 0x00CAD68E : 0x00CAD699);
		Good = (char*)(IsCI() ? 0x00CAD6CB : 0x00CAD6D6);
	}
	else {
		Skip = 1;
	}
	if (Skip == 0) 
	{

		strncpy(Evil, DefaultD, 10);
		strncpy(Neutral, DefaultD, 10);
		strncpy(Good, DefaultD, 10);
	}
}

void SetPlayer1TempleType(FLOAT Player1TempleTypeIG) {
	char* EvilP1 = (char*)(IsCI() ? 0x00CAD10B : 0x00CAD116);
	char* NeutralP1 = (char*)(IsCI() ? 0x00CAD14E : 0x00CAD159);
	char* GoodP1 = (char*)(IsCI() ? 0x00CAD18B : 0x00CAD196);
	DWORD* PTValue1 = (DWORD*)(IsCI() ? 0x00D7B614 : 0x00D7B61C);
	DWORD* PTValue2 = (DWORD*)(IsCI() ? PTValue1[0] + 0x7f8 : PTValue1[0] + 0x7fC);
	DWORD* PTValue3 = (DWORD*)(IsCI() ? PTValue2[0] + 0x2D0 : PTValue2[0] + 0x2D4);
	INT32 Value;
	char Returnvalue[12] = "";
	char Returnvalue2[5] = ".16b";
	if (Player1Templetype != (int)Player1TempleTypeIG)
	{
		Player1Templetype = (int)Player1TempleTypeIG;
		Value = (int)Player1TempleTypeIG;
		_ultoa(Value, Returnvalue, 10);
		strcat(Returnvalue, Returnvalue2);
		strncpy(EvilP1, Returnvalue, 10);
		strncpy(NeutralP1, Returnvalue, 10);
		strncpy(GoodP1, Returnvalue, 10);
		//_SaveAllMap("xyz.txt");
	 //   printf_to_log("PTValue1: %x\n", PTValue1[0]);
	 //   printf_to_log("PTValue2: %x\n", PTValue2[0]);
	 //   printf_to_log("PTValue3: %x\n", PTValue3[0]);
		if (PTValue3[0] != 0) {
			DWORD* PTValue4 = (DWORD*)(IsCI() ? PTValue3[0] + 0x30 : PTValue3[0] + 0x34);
			//Sleep(500);
			DWORD* PTValue5 = (DWORD*)(IsCI() ? PTValue4[0] + 0x40 : PTValue4[0] + 0x44);
			//Sleep(800);
			FLOAT* PTValue6 = (FLOAT*)(IsCI() ? PTValue5[0] + 0x88 : PTValue5[0] + 0x98);
			PTValue6 = (FLOAT*)(IsCI() ? PTValue5[0] + 0x88 : PTValue5[0] + 0x98);
			//     printf_to_log("PTValue4: %x\n", PTValue4[0]);
			//     printf_to_log("PTValue5: %x\n", PTValue5[0]);
			//     printf_to_log("PTValue6: %i\n", PTValue6[0]);
			PTValue6[0] = 0;
		}
	}
}

//Set all temples on a map.
void SetPlayerTemples(const char* p1, const char* p2, const char* p3, const char* p4, const char* p5, const char* p6, const char* p7)
{
	char* EvilP1 = (char*)(IsCI() ? 0x00CAD10B : 0x00CAD116);
	char* NeutralP1 = (char*)(IsCI() ? 0x00CAD14E : 0x00CAD159);
	char* GoodP1 = (char*)(IsCI() ? 0x00CAD18B : 0x00CAD196);
	char* EvilP2 = (char*)(IsCI() ? 0x00CAD1CB : 0x00CAD1D6);
	char* NeutralP2 = (char*)(IsCI() ? 0x00CAD20E : 0x00CAD219);
	char* GoodP2 = (char*)(IsCI() ? 0x00CAD24B : 0x00CAD256);
	char* EvilP3 = (char*)(IsCI() ? 0x00CAD28B : 0x00CAD296);
	char* NeutralP3 = (char*)(IsCI() ? 0x00CAD2CE : 0x00CAD2D9);
	char* GoodP3 = (char*)(IsCI() ? 0x00CAD30B : 0x00CAD316);
	char* EvilP4 = (char*)(IsCI() ? 0x00CAD34B : 0x00CAD356);
	char* NeutralP4 = (char*)(IsCI() ? 0x00CAD38E : 0x00CAD399);
	char* GoodP4 = (char*)(IsCI() ? 0x00CAD3CB : 0x00CAD3D6);
	char* EvilP5 = (char*)(IsCI() ? 0x00CAD40B : 0x00CAD416);
	char* NeutralP5 = (char*)(IsCI() ? 0x00CAD44E : 0x00CAD459);
	char* GoodP5 = (char*)(IsCI() ? 0x00CAD48B : 0x00CAD496);
	char* EvilP6 = (char*)(IsCI() ? 0x00CAD4CB : 0x00CAD4D6);
	char* NeutralP6 = (char*)(IsCI() ? 0x00CAD50E : 0x00CAD519);
	char* GoodP6 = (char*)(IsCI() ? 0x00CAD54B : 0x00CAD556);
	char* EvilP7 = (char*)(IsCI() ? 0x00CAD58B : 0x00CAD596);
	char* NeutralP7 = (char*)(IsCI() ? 0x00CAD5CE : 0x00CAD5D9);
	char* GoodP7 = (char*)(IsCI() ? 0x00CAD60B : 0x00CAD616);
	strncpy(EvilP1, p1, 10);
	strncpy(NeutralP1, p1, 10);
	strncpy(GoodP1, p1, 10);
	strncpy(EvilP2, p2, 10);
	strncpy(NeutralP2, p2, 10);
	strncpy(GoodP2, p2, 10);
	strncpy(EvilP3, p3, 10);
	strncpy(NeutralP3, p3, 10);
	strncpy(GoodP3, p3, 10);
	strncpy(EvilP4, p4, 10);
	strncpy(NeutralP4, p4, 10);
	strncpy(GoodP4, p4, 10);
	strncpy(EvilP5, p5, 10);
	strncpy(NeutralP5, p5, 10);
	strncpy(GoodP5, p5, 10);
	strncpy(EvilP6, p6, 10);
	strncpy(NeutralP6, p6, 10);
	strncpy(GoodP6, p6, 10);
	strncpy(EvilP7, p7, 10);
	strncpy(NeutralP7, p7, 10);
	strncpy(GoodP7, p7, 10);
}


void InitializePlayerSettings() {
	/*
	BYTE* Playerfixbyte1 = (BYTE*)(IsCI() ? 0x00565003 : 0x00565007);
	Playerfixbyte1[0] = 0x7;
	BYTE* Playerfixbyte2 = (BYTE*)(IsCI() ? 0x00569710 : 0x00569714);
	Playerfixbyte2[0] = 0x7;
	BYTE* Playerfixbyte3 = (BYTE*)(IsCI() ? 0x0057B508 : 0x0057B50C);
	Playerfixbyte3[0] = 0x7;
	BYTE* Playerfixbyte4 = (BYTE*)(IsCI() ? 0x0065FD3D : 0x0065FD41);
	Playerfixbyte4[0] = 0x7;
	BYTE* Playerfixbyte5 = (BYTE*)(IsCI() ? 0x0057B566 : 0x0057B56A);
	Playerfixbyte5[0] = 0x7;
	BYTE* Playerfixbyte6 = (BYTE*)(IsCI() ? 0x0057B67F : 0x0057B683);
	Playerfixbyte6[0] = 0x7;
	BYTE* Playerfixbyte7 = (BYTE*)(IsCI() ? 0x00583374 : 0x00583378);
	Playerfixbyte7[0] = 0x5;
	BYTE* Playerfixbyte8 = (BYTE*)(IsCI() ? 0x0058339E : 0x005833A2);
	Playerfixbyte8[0] = 0x73;
	BYTE* Playerfixbyte9 = (BYTE*)(IsCI() ? 0x005833E9 : 0x005833ED);
	Playerfixbyte9[0] = 0x4;
	BYTE* Playerfixbyte10 = (BYTE*)(IsCI() ? 0x005801BF : 0x005801C3);
	Playerfixbyte10[0] = 0x1;
	BYTE* Playerfixbyte11 = (BYTE*)(IsCI() ? 0x005804AF : 0x005804B3);
	Playerfixbyte11[0] = 0x1;
	BYTE* Playerfixbyte12 = (BYTE*)(IsCI() ? 0x005834BB : 0x005834BF);
	Playerfixbyte12[0] = 0x1A;
	DWORD* Playerfixbyte13 = (DWORD*)(IsCI() ? 0x0057B5E4 : 0x0057B7F0);
	Playerfixbyte13[0] = 0x72B38966;
	Playerfixbyte13[1] = 0x9000D7B7;
	DWORD* Playerfixbyte14 = (DWORD*)(IsCI() ? 0x0057B58C : 0x0057B598);
	Playerfixbyte14[0] = 0xD7B77283;
	Playerfixbyte14[1] = 0x90000000;
	BYTE* Symbolbyte1 = (BYTE*)(IsCI() ? 0x005F6D68 : 0x005F6D6C);
	Symbolbyte1[0] = 0x5;
	BYTE* Symbolbyte2 = (BYTE*)(IsCI() ? 0x005F6D64 : 0x005F6D68);
	Symbolbyte2[0] = 0x5;
	*/
	BYTE* Templebyte1 = (BYTE*)(IsCI() ? 0x008A1FC2 : 0x008A1FC6);
	Templebyte1[0] = 0x7;
	BYTE* Templebyte2 = (BYTE*)(IsCI() ? 0x008A1FDF : 0x008A1FE3);
	Templebyte2[0] = 0x7;
	BYTE* Templebyte3 = (BYTE*)(IsCI() ? 0x008A1FC8 : 0x008A1FCC);
	Templebyte3[0] = 0x6;
	BYTE* Templebyte4 = (BYTE*)(IsCI() ? 0x008A1FEB : 0x008A1FEF);
	Templebyte4[0] = 0x6;
	DWORD* Templebyte5 = (DWORD*)(IsCI() ? 0x008A1FD1 : 0x008A1FD5);
	Templebyte5[0] = 0x00CAD12C;
	DWORD* Templebyte6 = (DWORD*)(IsCI() ? 0x008A1FEE : 0x008A1FF2);
	Templebyte6[0] = 0x00CAD12C;
	DWORD* Templebyte7 = (DWORD*)(IsCI() ? 0x008A1FF4 : 0x008A1FF8);
	Templebyte7[0] = 0x00CAD16C;
	char* NeutralP1 = (char*)(IsCI() ? 0x00CAD12C : 0x00CAD160);
	char* GoodP1 = (char*)(IsCI() ? 0x00CAD16C : 0x00CAD1A0);
	char* EvilP2 = (char*)(IsCI() ? 0x00CAD1AC : 0x00CAD1E0);
	char* NeutralP2 = (char*)(IsCI() ? 0x00CAD1EC : 0x00CAD220);
	char* GoodP2 = (char*)(IsCI() ? 0x00CAD22C : 0x00CAD260);
	char* EvilP3 = (char*)(IsCI() ? 0x00CAD26C : 0x00CAD2A0);
	char* NeutralP3 = (char*)(IsCI() ? 0x00CAD2AC : 0x00CAD2E0);
	char* GoodP3 = (char*)(IsCI() ? 0x00CAD2EC : 0x00CAD320);
	char* EvilP4 = (char*)(IsCI() ? 0x00CAD32C : 0x00CAD360);
	char* NeutralP4 = (char*)(IsCI() ? 0x00CAD36C : 0x00CAD3A0);
	char* GoodP4 = (char*)(IsCI() ? 0x00CAD3AC : 0x00CAD3E0);
	char* EvilP5 = (char*)(IsCI() ? 0x00CAD3EC : 0x00CAD420);
	char* NeutralP5 = (char*)(IsCI() ? 0x00CAD42C : 0x00CAD460);
	char* GoodP5 = (char*)(IsCI() ? 0x00CAD46C : 0x00CAD4A0);
	char* EvilP6 = (char*)(IsCI() ? 0x00CAD4AC : 0x00CAD4E0);
	char* NeutralP6 = (char*)(IsCI() ? 0x00CAD4EC : 0x00CAD520);
	char* GoodP6 = (char*)(IsCI() ? 0x00CAD52C : 0x00CAD560);
	char* EvilP7 = (char*)(IsCI() ? 0x00CAD56C : 0x00CAD5A0);
	char* NeutralP7 = (char*)(IsCI() ? 0x00CAD5AC : 0x00CAD5E0);
	char* GoodP7 = (char*)(IsCI() ? 0x00CAD5EC : 0x00CAD620);
	char* EvilPN = (char*)(IsCI() ? 0x00CAD62C : 0x00CAD660);
	char* NeutralPN = (char*)(IsCI() ? 0x00CAD66C : 0x00CAD6A0);
	char* GoodPN = (char*)(IsCI() ? 0x00CAD6AC : 0x00CAD6E0);
	strncpy(NeutralP1, "Data\\Citadel\\OutsideMeshes\\Neutral0.16B", 54);
	strncpy(GoodP1, "Data\\Citadel\\OutsideMeshes\\Good0.16B", 54);
	strncpy(EvilP2, "Data\\Citadel\\OutsideMeshes\\Evil0.16B", 54);
	strncpy(NeutralP2, "Data\\Citadel\\OutsideMeshes\\Neutral0.16B", 54);
	strncpy(GoodP2, "Data\\Citadel\\OutsideMeshes\\Good0.16B", 54);
	strncpy(EvilP3, "Data\\Citadel\\OutsideMeshes\\Evil2.16B", 54);
	strncpy(NeutralP3, "Data\\Citadel\\OutsideMeshes\\Neutral2.16B", 54);
	strncpy(GoodP3, "Data\\Citadel\\OutsideMeshes\\Good2.16B", 54);
	strncpy(EvilP4, "Data\\Citadel\\OutsideMeshes\\Evil3.16B", 54);
	strncpy(NeutralP4, "Data\\Citadel\\OutsideMeshes\\Neutral3.16B", 54);
	strncpy(GoodP4, "Data\\Citadel\\OutsideMeshes\\Good3.16B", 54);
	strncpy(EvilP5, "Data\\Citadel\\OutsideMeshes\\Evil0.16B", 54);
	strncpy(NeutralP5, "Data\\Citadel\\OutsideMeshes\\Neutral0.16B", 54);
	strncpy(GoodP5, "Data\\Citadel\\OutsideMeshes\\Good0.16B", 54);
	strncpy(EvilP6, "Data\\Citadel\\OutsideMeshes\\Evil1.16B", 54);
	strncpy(NeutralP6, "Data\\Citadel\\OutsideMeshes\\Neutral1.16B", 54);
	strncpy(GoodP6, "Data\\Citadel\\OutsideMeshes\\Good1.16B", 54);
	strncpy(EvilP7, "Data\\Citadel\\OutsideMeshes\\Evil2.16B", 54);
	strncpy(NeutralP7, "Data\\Citadel\\OutsideMeshes\\Neutral2.16B", 54);
	strncpy(GoodP7, "Data\\Citadel\\OutsideMeshes\\Good2.16B", 54);
	strncpy(EvilPN, "Data\\Citadel\\OutsideMeshes\\Evil3.16B", 54);
	strncpy(NeutralPN, "Data\\Citadel\\OutsideMeshes\\Neutral3.16B", 54);
	strncpy(GoodPN, "Data\\Citadel\\OutsideMeshes\\Good3.16B", 54);
	DWORD* Spelldialogue_byte2 = (DWORD*)(IsCI() ? 0x00797CCC : 0x00797CF4);
	Spelldialogue_byte2[0] = 0x00458775;
	Spelldialogue_byte2[2] = 0x0045876B;
	Spelldialogue_byte2[4] = 0x00458780;
	DWORD* Spelldialogue_byte1 = (DWORD*)(IsCI() ? 0x0045876B : 0x0045886B);
	Spelldialogue_byte1[0] = 0x00000EB8;
	Spelldialogue_byte1[1] = 0xF4BFE900;
	Spelldialogue_byte1[2] = 0x0DB80033;
	Spelldialogue_byte1[3] = 0xE9000000;
	Spelldialogue_byte1[4] = 0x0033F4B5;
	Spelldialogue_byte1[5] = 0x000FB890;
	Spelldialogue_byte1[6] = 0xAAE90000;
	Spelldialogue_byte1[7] = 0x900033F4;
}

///Check if Player has the "Player[X]" name. A way (hacky + bad) to check if player is AI or not.
bool CheckPlayerHasComputerName(int PlayerNum)
{
	DWORD* LandnumberValue1 = (DWORD*)(IsCI() ? 0x00D7B614 : 0x00D7B618);

	wchar_t* NameValue;

	NameValue = (wchar_t*)(IsCI() ? (LandnumberValue1[0] + 0x394) : LandnumberValue1[0] + 0x3C8);
	//printf_to_log("%S", NameValue);

	if (PlayerNum == 1)
	{
		NameValue = (wchar_t*)(IsCI() ? (LandnumberValue1[0] + 0x114) : LandnumberValue1[0] + 0x3C8);
		if (wcsncmp(NameValue, L"Player[0]", 9) == 0)
		{
			//printf_to_log("Player 1 is Computer Controlled!\n");
			return true;
		}
		return false;
	}
	else if (PlayerNum == 2)
	{
		NameValue = (wchar_t*)(IsCI() ? (LandnumberValue1[0] + 0x394) : LandnumberValue1[0] + 0x3C8);

		if (wcsncmp(NameValue, L"Player[1]", 9) == 0)
		{
			//printf_to_log("Player 2 is Computer Controlled!\n");
			return true;
		}
		return false;
	}
	else if (PlayerNum == 3)
	{
		NameValue = (wchar_t*)(IsCI() ? (LandnumberValue1[0] + 0x614) : LandnumberValue1[0] + 0x648);
		if (wcsncmp(NameValue, L"Player[2]", 9) == 0)
		{
			//printf_to_log("Player 3 is Computer Controlled!\n");
			return true;
		}
		return false;
	}
	else if (PlayerNum == 4)
	{
		NameValue = (wchar_t*)(IsCI() ? (LandnumberValue1[0] + 0x894) : LandnumberValue1[0] + 0x8C8);
		if (wcsncmp(NameValue, L"Player[3]", 9) == 0)
		{
			//printf_to_log("Player 4 is Computer Controlled!\n");
			return true;
		}
		return false;
	}

	//Don't do anything for other player numbers.
	return false;
}

//Creature Variables.
char CreatureFileName[50];
char CreatureFilePath[50];
char CreatureBackupFilePath[50];
int CreatureFileLength;

DWORD getRegString(CHAR* keyName)
{
	HKEY hKey;

	DWORD dwType = REG_DWORD;
	DWORD lengh = sizeof(CreatureFileName);
	char regFolder[300];
	DWORD* dllBase = (DWORD*)GetModuleHandleA("LHMultiplayerR.DLL");
	char* profilId = (char*)(IsCI() ? dllBase + (0x0006B288 / 4) : dllBase + (0x0006B378 / 4));
	sprintf(regFolder, "Software\\Lionhead Studios Ltd\\Black & White\\LHMultiplayer\\ProfilesCI\\%s", profilId);

	LPCTSTR sk = TEXT(regFolder);

	//Set up some Creature file crap.
	RegOpenKey(HKEY_CURRENT_USER, sk, &hKey);
	RegQueryValueEx(hKey, keyName, NULL, NULL, (LPBYTE)&CreatureFileName, &lengh);
	RegCloseKey(hKey);
	printf("CreatureFile: %s\n", CreatureFileName);
	char CreatureMindPath[50] = "./Scripts/CreatureMind/";
	char CreatureBackupName[50] = "-BWUTEMP";

	//Get full path to Creature file.
	strcat(CreatureFilePath, CreatureMindPath);
	strcat(CreatureFilePath, CreatureFileName);

	//Create path to backup
	strcat(CreatureBackupFilePath, CreatureFilePath);
	strcat(CreatureBackupFilePath, CreatureBackupName);


	printf("CreatureFilePath: %s\n", CreatureFilePath);
	printf("CreatureBackupFilePath: %s\n", CreatureBackupFilePath);
	return 1;
}

//Backup the Creature file.
void CreateCreatureBackup()
{
	//Setup Creature Stuff.

	//Check if backup file exists. If not, create one.
	std::ifstream BackupCreature(CreatureBackupFilePath, std::ifstream::binary);
	if (BackupCreature.is_open())
	{
		printf("Creature backup already exists. Skipping. (This should probably never be hit).\n");
		BackupCreature.close();
	}
	else
	{
		CopyFile(CreatureFilePath, CreatureBackupFilePath, TRUE);
		printf("Creature backup created!\n");
	}
}

//Restore backup of Creature (if it exists).
void RestoreCreatureBackup()
{
	//Setup Creature Stuff.

	//Check if backup file exists. If so, restore it.
	std::ifstream BackupCreature(CreatureBackupFilePath, std::ifstream::binary);
	if (BackupCreature.is_open())
	{
		printf("Creature backup found. Restoring to this Creature. \n");
		CopyFile(CreatureBackupFilePath, CreatureFilePath, false);
		BackupCreature.close();

		//Delete the backup file.
		DeleteFile(CreatureBackupFilePath);
	}
	else
	{
		printf("Unable to find Creature backup. Restore failed.\n");
	}
}

//Adjust Creature stats for MP Colours.
void AdjustCreatureForMP()
{
	//Setup Creature Stuff for MP.
	std::ifstream file(CreatureFilePath, std::ifstream::binary);
	if (file.is_open())
	{
		file.seekg(0, file.end);
	}


	int length = file.tellg();
	int healthoffset = 0;
	int hungeroffset = 0;
	int poooffset = 0;
	int tirednessoffset = 0;
	int x = 0;
	char* buffer = new char[length];

	printf("Attemping to open Creature file...\n");

	if (file.is_open())
	{
		printf("Creature File Loaded! Length of : %d\n", length);
		for (int i = 0; i < length; i++)
		{
			file.seekg(i);

			file.read(reinterpret_cast<char*>(&x), 1);
			if (x == 0x0D)
			{
				file.seekg(i + 1);
				file.read(reinterpret_cast<char*>(&x), 1);
				if (x == 0x00)
				{
					healthoffset = length - 0x04;
					hungeroffset = i + 0x18;
					tirednessoffset = i + 0x26;
					poooffset = i + 0x22;
				}
			}
		}
		file.close();


		printf("Hunger Offset: %d\n", hungeroffset);
		// _sleep(10);

		 ///Open original CreatureMind.
		std::ifstream in;
		in.open(CreatureFilePath, std::ios::in | std::ios::binary);
		in.seekg(0, std::ios::end);
		CreatureFileLength = (int)in.tellg();
		printf("Creature File Length: %d\n", CreatureFileLength);
		char* buffer = new char[CreatureFileLength];
		in.seekg(0, in.beg);
		in.read(buffer, CreatureFileLength);
		in.close();


		 ///Write the Original Creature File into the new one (clean, unedited CreatureMind).
		std::ofstream out;
		out.open(CreatureFilePath, std::ios::out | std::ios::binary);
		out.write(reinterpret_cast<const char*>(buffer), CreatureFileLength);

		///Edit Creature Health.
		FLOAT f;
		f = FLOAT(getReg((char*)"PlayerRedColour")) * 0.002; //Same as /5 * 0.01;
		printf("Health = %f\n", f);

		//f += FLOAT(getReg("PlayerTemple")) * 0.1;
		f = (f - 1) * -1; //This needs to be reversed.
		out.seekp(healthoffset, std::ios::beg);
		out.write(reinterpret_cast<const char*>(&f), sizeof(float));

		// _sleep(100);

		 ///Edit Creature Hunger.
		f = FLOAT(getReg((char*)"PlayerGreenColour")) * 0.002; //Same as /5 * 0.01;
		printf("Hunger = %f\n", f);
		f = (f - 1) * -1; //This needs to be reversed.
		out.seekp(hungeroffset, std::ios::beg);
		out.write(reinterpret_cast<const char*>(&f), sizeof(float));

		//_sleep(100);

		///Edit Creature Tiredness.
		f = FLOAT(getReg((char*)"PlayerBlueColour")) * 0.002; //Same as /5 * 0.01;
		printf("Tiredness = %f\n", f);
		out.seekp(tirednessoffset, std::ios::beg);
		out.write(reinterpret_cast<const char*>(&f), sizeof(float));

		//_sleep(100);


		///Edit Creature Poo.
		f = FLOAT(getReg((char*)"PlayerTemple")) * 0.1; //Same as /5 * 0.01;
		out.seekp(poooffset, std::ios::beg);
		out.write(reinterpret_cast<const char*>(&f), sizeof(float));

		out.close();
	}
	else
	{
		file.close();
	}
}

//Let's do our own customs shit here, like colours, hard mode and multiplayer management. */
void ApplyCustomSettings()
{
	//Globals from the .chl file.
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LoadCreatureIsle = (FLOAT*)(PTType1[0] + 340);

	FLOAT* UseOriginalSettings = (FLOAT*)(IsCI() ? PTType1[0] + 16 : PTType1[0] + 20);
	FLOAT* MyTemple = (FLOAT*)(IsCI() ? PTType1[0] + 28 : PTType1[0] + 32);
	FLOAT* MyRed = (FLOAT*)(IsCI() ? PTType1[0] + 40 : PTType1[0] + 44);
	FLOAT* MyGreen = (FLOAT*)(IsCI() ? PTType1[0] + 52 : PTType1[0] + 56);
	FLOAT* MyBlue = (FLOAT*)(IsCI() ? PTType1[0] + 64 : PTType1[0] + 68);

	FLOAT* LandNumber = (FLOAT*)(IsCI() ? PTType1[0] + 76 : PTType1[0] + 80);
	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);
	FLOAT* ReloadSymbols = (FLOAT*)(IsCI() ? PTType1[0] + 100 : PTType1[0] + 0x68);
	FLOAT* SaveAllMap = (FLOAT*)(IsCI() ? PTType1[0] + 112 : PTType1[0] + 0x74);
	FLOAT* CHL_IsMultiplayer = (FLOAT*)(IsCI() ? PTType1[0] + 124 : PTType1[0] + 128);

	FLOAT* MP_P1_Red = (FLOAT*)(IsCI() ? PTType1[0] + 136 : PTType1[0] + 140);
	FLOAT* MP_P1_Green = (FLOAT*)(IsCI() ? PTType1[0] + 148 : PTType1[0] + 152);
	FLOAT* MP_P1_Blue = (FLOAT*)(IsCI() ? PTType1[0] + 160 : PTType1[0] + 164);
	FLOAT* MP_P1_Temple = (FLOAT*)(IsCI() ? PTType1[0] + 172 : PTType1[0] + 176);

	FLOAT* MP_P2_Red = (FLOAT*)(IsCI() ? PTType1[0] + 184 : PTType1[0] + 188);
	FLOAT* MP_P2_Green = (FLOAT*)(IsCI() ? PTType1[0] + 196 : PTType1[0] + 200);
	FLOAT* MP_P2_Blue = (FLOAT*)(IsCI() ? PTType1[0] + 208 : PTType1[0] + 212);
	FLOAT* MP_P2_Temple = (FLOAT*)(IsCI() ? PTType1[0] + 220 : PTType1[0] + 224);

	FLOAT* MP_P3_Red = (FLOAT*)(IsCI() ? PTType1[0] + 232 : PTType1[0] + 188);
	FLOAT* MP_P3_Green = (FLOAT*)(IsCI() ? PTType1[0] + 244 : PTType1[0] + 200);
	FLOAT* MP_P3_Blue = (FLOAT*)(IsCI() ? PTType1[0] + 256 : PTType1[0] + 212);
	FLOAT* MP_P3_Temple = (FLOAT*)(IsCI() ? PTType1[0] + 268 : PTType1[0] + 224);

	FLOAT* MP_P4_Red = (FLOAT*)(IsCI() ? PTType1[0] + 280 : PTType1[0] + 188);
	FLOAT* MP_P4_Green = (FLOAT*)(IsCI() ? PTType1[0] + 292 : PTType1[0] + 200);
	FLOAT* MP_P4_Blue = (FLOAT*)(IsCI() ? PTType1[0] + 304 : PTType1[0] + 212);
	FLOAT* MP_P4_Temple = (FLOAT*)(IsCI() ? PTType1[0] + 316 : PTType1[0] + 224);

	FLOAT* MP_Got_Player_Stats = (FLOAT*)(IsCI() ? PTType1[0] + 328 : PTType1[0] + 236);

	//Prep the Player's colour.
	DWORD Player1Colour = 0xFF000000 + getReg((char*)"PlayerColour");
	DWORD Player1InfluenceColour = getReg((char*)"PlayerColour");

	//Get Player's Temple. TODO: Replace this with cast rather than value check.
	DWORD Player1TempleNum = getReg((char*)"PlayerTemple");
	const char* Player1Temple = "0.16b";
	if (Player1TempleNum == 0)
	{
		Player1Temple = "0.16b";
	}
	else if (Player1TempleNum == 1)
	{
		Player1Temple = "1.16b";
	}
	else if (Player1TempleNum == 2)
	{
		Player1Temple = "2.16b";
	}
	else if (Player1TempleNum == 3)
	{
		Player1Temple = "3.16b";
	}

	//Check if Multiplayer.
	//bool isMultiplayer = IsMultiplayerGame(GetGame());

	BYTE* IsMultiplayer = (BYTE*)GetModuleHandleA("LHMultiplayerR.DLL") + 0x0069B50;
	//printf("\nIs Multiplayer?: %d \n", isMultiplayer);


	//BYTE* IsGamePaused = (BYTE*)(IsCI() ? 0x00D7B68C : 0x00D7B68C); //0x00F5DC48
	//IsGamePaused[0] = 1;

	//Player variables.
	bool P1_isAI = CheckPlayerHasComputerName(1);
	bool P2_isAI = CheckPlayerHasComputerName(2);
	bool P3_isAI = CheckPlayerHasComputerName(3);
	bool P4_isAI = CheckPlayerHasComputerName(4);

	///Mutliplayer colours
	DWORD MP_P1_Colour;
	DWORD MP_P1_TempleColour;

	DWORD MP_P2_Colour;
	DWORD MP_P2_TempleColour;

	DWORD MP_P3_Colour;
	DWORD MP_P3_TempleColour;

	DWORD MP_P4_Colour;
	DWORD MP_P4_TempleColour;


	///Colour/Temple stuff.
	if (IsMultiplayer[0] == 0)
	{
		//Always P1 in offline, so set this to our own temple always.
		//SetPlayer1TempleType(MyTemple[0]);


		//Intro Menu.
		if (LandNumber[0] == 99)
		{
			DWORD MyInfluenceColour = (DWORD((MyRed[0])) * 65536) + (DWORD((MyGreen[0])) * 256) + (DWORD((MyBlue[0])) * 1);

			SetPlayerColours(Player1Colour, GREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, BGREEN, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);

			setReg((char*)"PlayerColour", MyInfluenceColour);
			setReg((char*)"PlayerTemple", DWORD(MyTemple[0]));
			SetPlayer1TempleType(MyTemple[0]);

			//Set RGB values.
			if (MyRed[0] > 0)
			{
				setReg((char*)"PlayerRedColour", DWORD(MyRed[0]));
			}
			if (MyGreen[0] > 0)
			{
				setReg((char*)"PlayerGreenColour", DWORD(MyGreen[0]));
			}
			if (MyBlue[0] > 0)
			{
				setReg((char*)"PlayerBlueColour", DWORD(MyBlue[0]));
			}
		}
		//Skirmish, Land 1 & Land 4.
		else if (LandNumber[0] == 0 || LandNumber[0] == 1 || LandNumber[0] == 4)
		{
			SetPlayerColours(Player1Colour, GREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, BGREEN, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);
			SetTemples2("Player1", Player1Temple);
			SetTemples2("Player2", "1.16b");
			SetTemples2("Player3", "2.16b");
			SetTemples2("Player4", "3.16b");
		}
		//Land 2.
		else if (LandNumber[0] == 2)
		{
			SetPlayerColours(Player1Colour, YELLOW, BLUE, ORANGE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, BYELLOW, BBLUE, BORANGE, BYELLOW, BBLUE, BORANGE);
			SetTemples2("Player1", Player1Temple);
			SetTemples2("Player2", "0.16b");
			SetTemples2("Player3", "1.16b");

			//Default (Unused).
			SetTemples2("Player4", "3.16b");
		}
		//Land 3.
		else if (LandNumber[0] == 3)
		{
			SetPlayerColours(Player1Colour, BLUE, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, BBLUE, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);
			SetTemples2("Player1", Player1Temple);
			SetTemples2("Player2", "1.16b");

			//Default (Unused).
			SetTemples2("Player3", "2.16b");
			SetTemples2("Player4", "3.16b");
		}
		//Land 5.
		else if (LandNumber[0] == 5)
		{
			SetPlayerColours(Player1Colour, ORANGE, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, BORANGE, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);
			SetTemples2("Player1", Player1Temple);
			SetTemples2("Player2", "2.16b");

			//Default (Unused).
			SetTemples2("Player3", "2.16b");
			SetTemples2("Player4", "3.16b");
		}
	}
	//If Multiplayer
	else if (MP_P1_Red[0] > 0 || MP_P1_Blue[0] > 0 || MP_P1_Green[0] > 0 || MP_P2_Red[0] > 0 || MP_P2_Blue[0] > 0 || MP_P2_Green[0] > 0 || MP_P3_Red[0] > 0 || MP_P3_Blue[0] > 0 || MP_P3_Green[0] > 0 || MP_P4_Red[0] > 0 || MP_P4_Blue[0] > 0 || MP_P4_Green[0] > 0)//(MP_Got_Player_Stats[0] >= 0)//(CheckPlayerHasComputerName(2) == false || CheckPlayerHasComputerName(3) == false || CheckPlayerHasComputerName(4) == false )//(MP_Got_Player_Stats[0] == 1 || MP_Got_Player_Stats[0] == 2) ///Need to reliably check for if a Multiplayer Game...
	{
		//Save map/land for Multiplayer.
		if (SaveAllMap[0] > 0)
		{
			//Save the current map (buildings etc.)
			_SaveAllMap((char*)"oos.txt");
			_SaveLandOnDisk((char*)"oos.lnd");
			SaveAllMap[0] = 0;
		}

		//Temporary variables to hold Player colours.
		MP_Got_Player_Stats[0] = 2;
		FLOAT RedColour = 0;
		FLOAT GreenColour = 0;
		FLOAT BlueColour = 0;

		///Player 1
		P1_isAI = CheckPlayerHasComputerName(1);
		if (P1_isAI == false)
		{
			//P1 Red.
			RedColour = (FLOAT(MP_P1_Red[0]) - 1) * -1; //This needs to be reversed.
			RedColour *= 100 * 5;
			DWORD FinalRedColour = DWORD(RedColour);

			//P1 Green.
			GreenColour = (FLOAT(MP_P1_Green[0]) - 1) * -1; //This needs to be reversed.
			GreenColour *= 100 * 5;
			DWORD FinalGreenColour = DWORD(GreenColour);

			//P1 Blue.
			BlueColour = (FLOAT(MP_P1_Blue[0]));
			BlueColour *= 100 * 5;
			DWORD FinalBlueColour = DWORD(BlueColour);


			//P1 also set temple.
			if ((FLOAT(MP_P1_Temple[0]) < 0.09))
			{
				SetTemples2("Player1", "0.16b");
			}
			else if ((FLOAT(MP_P1_Temple[0]) < 0.19))
			{
				SetTemples2("Player1", "1.16b");
			}
			else if ((FLOAT(MP_P1_Temple[0]) < 0.29))
			{
				SetTemples2("Player1", "2.16b");
			}
			else if ((FLOAT(MP_P1_Temple[0]) < 0.39))
			{
				SetTemples2("Player1", "3.16b");
			}

			//Calculate Colours for Player 1.
			MP_P1_Colour = (DWORD((FinalRedColour)) * 65536) + (DWORD((FinalGreenColour)) * 256) + (DWORD((FinalBlueColour)) * 1);// + DWORD((MyGreen * 256)) + DWORD(MyBlue);
			MP_P1_TempleColour = 0xFF000000 + MP_P1_Colour;

		}
		else
		{
			DWORD* PlayerColors = GetPlayerColours();
			MP_P1_Colour = PlayerColors[0] - 0xFF000000;
			MP_P1_TempleColour = PlayerColors[0];
			SetTemples2("Player1", "0.16b");
		}

		///Player 2
		P2_isAI = CheckPlayerHasComputerName(2);
		if (P2_isAI == false)
		{
			//P2 Red.
			RedColour = (FLOAT(MP_P2_Red[0]) - 1) * -1; //This needs to be reversed.
			RedColour *= 100 * 5;
			DWORD FinalRedColour = DWORD(RedColour);

			//P2 Green.
			GreenColour = (FLOAT(MP_P2_Green[0]) - 1) * -1; //This needs to be reversed.
			GreenColour *= 100 * 5;
			DWORD FinalGreenColour = DWORD(GreenColour);

			//P2 Blue.
			BlueColour = (FLOAT(MP_P2_Blue[0]));
			BlueColour *= 100 * 5;
			DWORD FinalBlueColour = DWORD(BlueColour);

			//P2 also set temple.
			if ((FLOAT(MP_P2_Temple[0]) < 0.09))
			{
				SetTemples2("Player2", "0.16b");
			}
			else if ((FLOAT(MP_P2_Temple[0]) < 0.19))
			{
				SetTemples2("Player2", "1.16b");
			}
			else if ((FLOAT(MP_P2_Temple[0]) < 0.29))
			{
				SetTemples2("Player2", "2.16b");
			}
			else if ((FLOAT(MP_P2_Temple[0]) < 0.39))
			{
				SetTemples2("Player2", "3.16b");
			}

			//Calculate Colours for Player 2.
			MP_P2_Colour = (DWORD((FinalRedColour)) * 65536) + (DWORD((FinalGreenColour)) * 256) + (DWORD((FinalBlueColour)) * 1);
			MP_P2_TempleColour = 0xFF000000 + MP_P2_Colour;
		}
		else
		{
			DWORD* PlayerColors = GetPlayerColours();
			MP_P2_Colour = PlayerColors[1] - 0xFF000000;
			MP_P2_TempleColour = PlayerColors[1];
			SetTemples2("Player2", "1.16b");
		}

		///Player 3
		P3_isAI = CheckPlayerHasComputerName(3);
		if (P3_isAI == false)
		{
			//P3 Red.
			RedColour = (FLOAT(MP_P3_Red[0]) - 1) * -1; //This needs to be reversed.
			RedColour *= 100 * 5;
			DWORD FinalRedColour = DWORD(RedColour);

			//P3 Green.
			GreenColour = (FLOAT(MP_P3_Green[0]) - 1) * -1; //This needs to be reversed.
			GreenColour *= 100 * 5;
			DWORD FinalGreenColour = DWORD(GreenColour);

			//P3 Blue.
			BlueColour = (FLOAT(MP_P3_Blue[0]));
			BlueColour *= 100 * 5;
			DWORD FinalBlueColour = DWORD(BlueColour);

			//P3 also set temple.
			if (FLOAT(MP_P3_Temple[0] < 0.09))
			{
				SetTemples2("Player3", "0.16b");
			}
			else if (FLOAT(MP_P3_Temple[0] < 0.19))
			{
				SetTemples2("Player3", "1.16b");
			}
			else if (FLOAT(MP_P3_Temple[0] < 0.29))
			{
				SetTemples2("Player3", "2.16b");
			}
			else if (FLOAT(MP_P3_Temple[0] < 0.39))
			{
				SetTemples2("Player3", "3.16b");
			}

			//Calculate Colours for Player 3.
			MP_P3_Colour = (DWORD((FinalRedColour)) * 65536) + (DWORD((FinalGreenColour)) * 256) + (DWORD((FinalBlueColour)) * 1);
			MP_P3_TempleColour = 0xFF000000 + MP_P3_Colour;
		}
		else
		{
			DWORD* PlayerColors = GetPlayerColours();
			MP_P3_Colour = PlayerColors[2] - 0xFF000000;
			MP_P3_TempleColour = PlayerColors[2];
			SetTemples2("Player3", "2.16b");
		}

		///Player 4
		P4_isAI = CheckPlayerHasComputerName(4);
		if (P4_isAI == false)
		{
			//P4 Red.
			RedColour = (FLOAT(MP_P4_Red[0]) - 1) * -1; //This needs to be reversed.
			RedColour *= 100 * 5;
			DWORD FinalRedColour = DWORD(RedColour);

			//P4 Green.
			GreenColour = (FLOAT(MP_P4_Green[0]) - 1) * -1; //This needs to be reversed.
			GreenColour *= 100 * 5;
			DWORD FinalGreenColour = DWORD(GreenColour);

			//P4 Blue.
			BlueColour = (FLOAT(MP_P4_Blue[0]));
			BlueColour *= 100 * 5;
			DWORD FinalBlueColour = DWORD(BlueColour);

			//P4 also set temple.
			if (FLOAT(MP_P4_Temple[0] < 0.09))
			{
				SetTemples2("Player4", "0.16b");
			}
			else if (FLOAT(MP_P4_Temple[0] < 0.19))
			{
				SetTemples2("Player4", "1.16b");
			}
			else if (FLOAT(MP_P4_Temple[0] < 0.29))
			{
				SetTemples2("Player4", "2.16b");
			}
			else if (FLOAT(MP_P4_Temple[0] < 0.39))
			{
				SetTemples2("Player4", "3.16b");
			}

			//Calculate Colours for Player 4.
			MP_P4_Colour = (DWORD((FinalRedColour)) * 65536) + (DWORD((FinalGreenColour)) * 256) + (DWORD((FinalBlueColour)) * 1);// + DWORD((MyGreen * 256)) + DWORD(MyBlue);
			MP_P4_TempleColour = 0xFF000000 + MP_P4_Colour;
		}
		else
		{
			DWORD* PlayerColors = GetPlayerColours();
			MP_P4_Colour = PlayerColors[3] - 0xFF000000;
			MP_P4_TempleColour = PlayerColors[3];
			SetTemples2("Player4", "3.16b");
		}

		//Remove backup, no longer needed.
		remove(CreatureBackupFilePath);

		//Apply all Player's custom colours.
		SetPlayerColours(MP_P1_TempleColour, MP_P2_TempleColour, MP_P3_TempleColour, MP_P4_TempleColour, YELLOW, BLUE, ORANGE);
		SetPlayerInfluenceColours(MP_P1_Colour, MP_P2_Colour, MP_P3_Colour, MP_P4_Colour, BYELLOW, BBLUE, BORANGE);
	}
}

/////////////////////////////////////////////////////////////////////
















//Ultimate Detours
void* PlayerSymbolSprite__Create_Detour(uint8_t player_id)
{
	PlayerSymbolSprite* ptr = (PlayerSymbolSprite*)PlayerSymbolSprite__Create(player_id);

	printf("PlayerSymbolSprite Created At: %p (Player: %d) Colour: 0x%.8X\n", ptr, player_id, ptr->colour);

	//Only take note of Village Centres once.
	if (UpdateVillageCentre == true)
	{
		//Take note of the ptr.
		VillageCentrePointers[TownCentreSize] = ptr;
		VillageCentrePlayers[TownCentreSize] = player_id;
		TownCentreSize++;
		printf("Updating VC \n");
	}

	return (void*)ptr;
}

void ClearGlobals()
{
	TownCentreSize = 0;
	UpdateVillageCentre = true;
	Player1Templetype = -1;
}

void __fastcall GGame_ProcessKey_Detour(unsigned __int8* _this, int edx, int a2, int a3)
{
	//printf("_this: %d \n", _this);
	//printf("edx: %d \n", edx);
	//printf("a2: %d \n", a2);
	//printf("a3: %d \n", a3);

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* UseOriginalSettings = (FLOAT*)(IsCI() ? PTType1[0] + 16 : PTType1[0] + 20);
	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);

	//If hard mode and original story, 
	if (Difficulty[0] == 1 & UseOriginalSettings[0] == 21)
	{
		//Don't pause for when in Main Menus.
		if (a2 == 25)
		{
			a2 = 0;
		}
	}

	GGame_ProcessKey(_this, a2, a3);
}

int __fastcall Creature__Create_Detour(int* a1, int edx, int a2, int a3, int a4)
{
	int Creature = Creature__Create(a1, a2, a3, a4);
	//printf("a1: %d \n", a1);
	//printf("a2: %d \n", a2);
	//printf("a3: %d \n", a3);
	//printf("a4: %d \n", a4);
	return Creature;
}

void PauseGame_Detour(int num, int edx)
{
	//return 0;
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));

	FLOAT* UseOriginalSettings = (FLOAT*)(IsCI() ? PTType1[0] + 16 : PTType1[0] + 20);
	FLOAT* LandNumber = (FLOAT*)(IsCI() ? PTType1[0] + 76 : PTType1[0] + 80);
	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);

	BYTE* IsGamePaused = (BYTE*)(IsCI() ? 0x00D7B68C : 0x00D7B68C); //0x00F5DC48


	if (Difficulty[0] == 1 && IsGamePaused[0] == 1)
	{
		//Don't pause for when in Main Menus.
		if (GetKeyState('P') & 0x8000)
		{
			printf("Pausing disabled in Hard Mode! (%d)\n", num);
		}
	}
	else
	{
		printf("Paused! (%d)\n", num);
		PauseGame(num);
	}
}

void __fastcall EveCreatureFunction_Detour(char* _this, int edx, const char* a2)
{
	EveCreatureFunction(_this, a2);
	printf("----- \n");
	printf("Eve Creature shit. \n");
	printf("this: %c \n", _this);
	printf("a2: %s \n", a2);
	printf("----- \n");
}

void __fastcall GGame__ClearMap_Detour(void* _this, int edx)
{
	GGame__ClearMap(_this);
	printf("Map Cleared!\n");
	ClearGlobals();

	//Check what .CHL to load.
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LoadCreatureIsle = (FLOAT*)(PTType1[0] + 340);

	if (LoadCreatureIsle[0] == 21)
	{
		char* CHL_ToLoad = (char*)(0x0C7CABC);
		*CHL_ToLoad = 'B';
		printf("Loading CI...\n");

		DWORD* The_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
		DWORD state = The_GGame[0x930B0];

		The_GGame[0x930AF] = 1;
		The_GGame[0x930B0] = 1;
	}
	else
	{
		char* CHL_ToLoad = (char*)(0x0C7CABC);
		*CHL_ToLoad = 'C';
		printf("Loading Ultimate...\n");
	}


	//*************** CREATURE STUFF ***************//

	//Print state of game (Single Player, Skirmish, Multiplayer...)
	DWORD* The_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	DWORD state = The_GGame[0x930B0];
	printf("(Game State): %d \n", state);

	//Init Creature Paths ONCE.
	if (strlen(CreatureFilePath) <= 1)
	{
		getRegString((char*)"file");
	}

	//Restore Creature Backup. Do this when coming back from MP.
	if (state == 0)
	{
		RestoreCreatureBackup();
	}

	//Join Online Game Menu. Create Creature Backup when going into MP lobby.
	else if (state == 3)
	{
		//Create backup of Creature.
		CreateCreatureBackup();

		//Create Creature Backup with adjusted Damage, Hunger + Tiredness.
		AdjustCreatureForMP();

		//Set to default colours + temples for the Multiplayer Lobby.
		SetPlayerColours(RED, GREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
		SetPlayerInfluenceColours(RED, GREEN, PURPLE, SKYBLUE, BYELLOW, BBLUE, BORANGE);
		SetTemples2("Player1", "0.16b");
		SetTemples2("Player2", "1.16b");
		SetTemples2("Player3", "2.16b");
		SetTemples2("Player4", "3.16b");

		//Adjust oos.txt (MP Map) file.
		//Make edit to oos.txt. Change the landscape that is loaded to the landscape we create (oos.lnd).
		std::ifstream original_oos("oos.txt");
		std::ofstream new_oos("newoos.txt");

		std::string str;
		int line = 0;

		//Read in original oos.txt but replace line 3 with new landscape (oos.lnd)
		while (std::getline(original_oos, str))
		{
			line++;
			if (line == 3)
			{
				new_oos << "LOAD_LANDSCAPE(\"oos.lnd\")";
			}
			else
			{
				new_oos << str;
			}
			new_oos << "\n";
		}

		original_oos.close();
		new_oos.close();

		//Clean up
		remove("oos.txt");
		rename("newoos.txt", "oos.txt");

		char c;
		std::ifstream i_file("start.dat");
		std::ofstream o_file("oos.dat");
	}

	//Skirmish Menu.
	else if (state == 5)
	{
		//SetPlayerColours(RED, GREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
		//SetPlayerInfluenceColours(BRED, BGREEN, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);
		//SetTemples2("Player1", "0.1);
		//SetTemples2("Player2", "1.16b");
		//SetTemples2("Player3", "2.16b");
		//SetTemples2("Player4", "3.16b");
	}
	//*************** CREATURE STUFF ***************//
}



void __fastcall ProcessGraphicsEngine(Game* _this, int edx)
{
	IsMultiplayerGame(_this);
}

void __fastcall TownCentre_DrawPSys_Detour(void* _this, int edx)
{
	TownCentre_DrawPSys(_this);

	//Refresh the Town Centres
	for (int i = 0; i < TownCentreSize; i++)
	{
		DWORD* PlayerColors = GetPlayerColours();
		uint8_t PlayerID = VillageCentrePlayers[i];
		VillageCentrePointers[i]->colour = PlayerColors[GetRemapedPlayer(PlayerID)];
	}

}

int __fastcall GetPlayerColour_Detour(void* _this, int edx)
{
	//printf("_this Detour: 0x%.8X\n", _this);
	int Colour = GetPlayerColour(_this);
	//printf("Colour Detour: 0x%.8X\n", Colour);
	return Colour;
}

int GetRemapedPlayer_Detour(int OriginalPlayer)
{

	int Player = GetRemapedPlayer(OriginalPlayer);
	return Player;
	/*

	DWORD* PlayerColours = (DWORD*)(0x00C6FB88);
	DWORD* InfluencePlayerColours = (DWORD*)(0x00F2E85C);

	//Get result of the mapping.
	if (Player == OriginalPlayer)
	{
		return Player;
	}
	//Update the actual AI colours array. This fixes the issue with "SET_LAND_NUMBER(X)" where Town Centres symbols aren't updated, even with the fix.
	else
	{
		//If Khazar
		if (Player == 4)
		{
			PlayerColours[OriginalPlayer] = YELLOW;
			InfluencePlayerColours[OriginalPlayer] = BYELLOW;
		}
		//If Lethys
		else if (Player == 5)
		{
			PlayerColours[OriginalPlayer] = BLUE;
			InfluencePlayerColours[OriginalPlayer] = BBLUE;
		}
		//If Nemesis
		else if (Player == 6)
		{
			PlayerColours[OriginalPlayer] = ORANGE;
			InfluencePlayerColours[OriginalPlayer] = BORANGE;
		}

		printf("Player %d mapped to %d!\n", OriginalPlayer, Player);
	}

	return Player;
	*/
}

void __fastcall ReloadSymbols_Detour(int edx)
{
	ReloadSymbols();
	printf("Symbols Reloaded!\n");
	UpdateVillageCentre = false;
}

void __fastcall GGame__StartGame_Detour(Game* _this, int edx)
{
	//Print stuff.
	printf("Game Started!\n");

	//RestoreCreatureBackup();

	//Start Game
	GGame__StartGame(_this);
}

void __fastcall GGame__Loop_Detour(void* _this, int edx)
{
	//Print stuff.
	printf("GGame::Loop - Starting...\n");

	//TODO Move this from tick method, terrible, terrible solution.
	//ApplyCustomSettings();

	//Normal Loop
	GGame__Loop(_this);

	//ApplyCustomSettings();
}

void __fastcall DrawAll__TownCentre_Detour(int edx)
{
	DrawAll__TownCentre();
	//printf("My Loop!\n");
}

Game* __fastcall GScript__Reset_Detour(DWORD* _this, int edx, int a)
{
	//Print stuff.
	printf("GScript Loop - Started!\n");

	//Start Game
	return  GScript__Reset(_this, a);

}

char __fastcall BWCheckFeatureIsEnabled_Detour(char* _this, int edx)
{

	InitializePlayerSettings();

	//Load the temple style before the game loads up. Get Player's Temple. TODO: Replace this with cast rather than value check.
	DWORD Player1TempleNum = getReg((char*)"PlayerTemple");
	if (Player1TempleNum == 0)
	{
		SetTemples2("Player1", "0.16b");
	}
	else if (Player1TempleNum == 1)
	{
		SetTemples2("Player1", "1.16b");
	}
	else if (Player1TempleNum == 2)
	{
		SetTemples2("Player1", "2.16b");
	}
	else if (Player1TempleNum == 3)
	{
		SetTemples2("Player1", "3.16b");
	}

	//Check if Football is enabled.
	if ((int)_this == 1703516)
	{
		//ApplyCustomSettings();
		printf("Dudeeeee \n");
	}

	return BWCheckFeatureIsEnabled(_this);
}

int __fastcall stop_draw_sprite_to_screen_Detour(int edx)
{
	//ApplyCustomSettings();
	printf("Stopping drawing sprites!");
	return stop_draw_sprite_to_screen();
}

bool __fastcall GSetup__LoadMapScript_Detour(int edx)
{
	printf("Reading Map!\n");
	return GSetup__LoadMapScript();
}


signed int __fastcall GGame__Init_Detour(void* _this, int edx)
{
	printf("GGame_Init!\n");
	int buffer = GGame__Init(_this);
	int buffer2 = edx;
	edx = buffer2;
	return buffer;
}

//Used as the onTick() hook.
int __fastcall Process3DEngine_Detour(BYTE* _this, int edx)
{
	//Apply custom stuff on this onTick() method.
	ApplyCustomSettings();


	//printf("My Loop!\n");
	return Process3DEngine(_this);
}

int __fastcall ControlMap__ProcessActionsPerformed_Detour(DWORD* _this, int edx)
{
	printf("loop\n");
	return ControlMap__ProcessActionsPerformed(_this);
}

////////////////////

void __fastcall Game3DObject_AddForDrawing_Detour(void* _this, int edx)
{
	Game3DObject_AddForDrawing(_this);
	printf("Game3DObject::AddForDrawing %p\n", _this);
}

void __fastcall LH3DMeshedObject_AddDrawing_Detour(void* _this, int edx)
{
	LH3DMeshedObject_AddDrawing(_this);
	printf("LH3DMeshedObject::AddDrawing %p\n", _this);
}


void __fastcall BW_GGame_Process3dEngine_Detour(void *_this, int edx)
{
	BW_GGame_Process3dEngine(_this);

	LH3DColor color{ 0, 0, 0, 180 };
	LH3DColor text_color{ 255, 255, 255, 255 };
	LHRegion region{ 0, 0, LHScreen_Width(), 128 };

	BW_BackgroundBox_Draw(&region, &color, 0);
	BW_GatheringText__DrawText((void*)(*(int*)0x00C4CD2C), L"Hello Kratzean", 32.0f, 71.0f, 71.0f, 332.0f, 111.0f, 111.0f, 71.0f, 0.45f, 32.0f, &text_color, 0, 1, 0);
}

static char MyDocumentsPath[MAX_PATH];
static char MainDirectory[MAX_PATH];
static char ProfileDirectory[MAX_PATH];

void __fastcall BW_PathCreator_Detour(PathCreator *_this, int edx)
{
	// allow the default paths to be set
	BW_PathCreator(_this);

	// XP
	SHGetFolderPathA(0, CSIDL_MYDOCUMENTS, 0, 0, MyDocumentsPath);
	// Vista+
	// HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path); // FOLDERID_SavedGames

		// SHGetFolderPathA(0, CSIDL_MYDOCUMENTS, 0, 0, LHPlayerProfileLib::MyDocumentsPath);
	// HRESULT hr = SHGetKnownFolderPath(FOLDERID_SavedGames, 0, NULL, &path);

	sprintf(MainDirectory, "%s\\Black & White", MyDocumentsPath);
	sprintf(ProfileDirectory, "%s\\Profiles", MainDirectory);

	// WE HAVE TO CREATE DIRECTORIES
	PathCreator_CreateDir(MainDirectory);
	PathCreator_CreateDir(ProfileDirectory);

	printf("PathCreator\n");
	printf("\tGameRootPath: %s\n", _this->GameRootPath);
	printf("\tDevPath: %s\n", _this->DevPath);
}

// Data/Textures/ChooseSymbol(a).raw, Data/Textures/PlayersSymbols(a).raw
// Online Maps/ConditionTemplate.tmp, Online Maps/localmaps.txt
// Scripts/CreatureMind/%s, Scripts/CreatureMind/Physique%s

void __fastcall BW_PathCreator_GetProfilePath_Detour(PathCreator *_this, int edx, char* a2)
{
	sprintf(a2, "%s\\Profiles", MainDirectory);
	printf("PathCreator::GetProfilePath: %s\n", a2);

	/*PWSTR path = NULL;

	HRESULT hr = SHGetKnownFolderPath(FOLDERID_SavedGames, 0, NULL, &path);

	if (SUCCEEDED(hr)) {
		wprintf(L"%ls\n", path);
	}

	CoTaskMemFree(path);
	
	sprintf(a2, "%s\\%s", "C:\\Users\\Matt\\Saved Games\\Black & White", _this->ProfilesPath);

	printf("PathCreator::GetProfilePath: %s\n", a2);*/
}

void LH3DIslandDraw_Detour()
{
	BW_LH3DRender_SetRenderState(8, 2); // D3DFILL_MODE = D3DFILL_WIREFRAME
	BW_LH3DIsland_Draw();
	BW_LH3DRender_SetRenderState(8, 3); // D3DFILL_MODE = D3DFILL_SOLID
}

void BW_Report3D_Detour(char* fmt, ...)
{
	char buf[1024];

	va_list args;
	va_start(args, fmt);
	wvsprintf(buf, fmt, args);
	va_end(args);

	printf("%s", buf);
}

void __cdecl BW_DrawLoading_Detour(float a1, float a2)
{
	printf("DrawLoading(%.2f, %.2f)\n", a1, a2);

	if (a1 <= 0.0)
		return;

	BW_LH3DRender_StartFrame();
	BW_LH3DRender_FinishFrame();

	BW_DrawLoading(a1, a2);
}

void __fastcall LoaderAnon_ReadVariableFile_Detour(void* _this, int edx, char* a2)
{
	printf("LoaderAnon::ReadVariableFile: %s\n", a2);

	LoaderAnon_ReadVariableFile(_this, a2);
}

void __fastcall LoaderAnon_StoreFieldNamesOrInstances_Detour(void* _this, int edx, char* a2)
{
	LoaderAnon_StoreFieldNamesOrInstances(_this, a2);

	printf("LoaderAnon::StoreFieldNamesOrInstances: %s\n", a2);
}

void __fastcall LoaderAnon_StoreInstancesOrFieldNames_Detour(void* _this, int edx, char* a2)
{
	LoaderAnon_StoreInstancesOrFieldNames(_this, a2);

	printf("LoaderAnon::StoreInstancesOrFieldNames: %s\n", a2);
}

// void(__thiscall* LoaderAnon_AddField)(void*, char* a2, int, char, int16_t, int) = (void(__thiscall*)(void*, char* a2, int, char, int16_t, int))(0x005F2C10);

char* __fastcall LoaderAnon_AddField_Detour(void* _this, int edx, char* a2, int a3, char a4, int16_t a5, int a6)
{
	char* val = LoaderAnon_AddField(_this, a2, a3, a4, a5, a6);

	printf("LoaderAnon::AddField: %s (ret: %s)\n", a2, val);

	return val;
}

void __fastcall LH3DWater_DrawSea_Detour(void* _this, int edx)
{
	return;
}

// void(__cdecl* peerStartListingGames)(void* peer, void* listCallback, int a3)
void peerStartListingGames_Detour(void* peer, void* listCallback, int a3)
{
	printf("peerStartListingGames listCallback: %p\n", listCallback);
	peerStartListingGames(peer, listCallback, a3);
}

void BWGameSpy_ListingGamesCallback_Detour(void* peer, void* a2, int a3, void* a4, int a5, int a6)
{
	printf("BWGameSpy_ListingGamesCallback\n");
	BWGameSpy_ListingGamesCallback(peer, a2, a3, a4, a5, a6);
}

void* piNewListingGamesOperation_Detour(int a1, int a2, int a3) {
	printf("piNewListingGamesOperation\n");
	return piNewListingGamesOperation(a1, a2, a3);
}

int piListingGamesEnterChannelCallback_Detour(void* chat, int a2, char* a3, int* a4)
{
	printf("piListingGamesEnterChannelCallback: %s\n", a3);
	return piListingGamesEnterChannelCallback(chat, a2, a3, a4);
}

void* piListingGamesChannelMessage_Detour(int a1, int a2, char* a3, char* a4, int a5, int i) {
	printf("piListingGamesChannelMessage: %s %s\n", a3, a4);
	return piListingGamesChannelMessage(a1, a2, a3, a4, a5, i);
}

int _piCEngineStartListingGames_Detour(void* a1) {
	printf("_piCEngineStartListingGames_Detour\n");
	return _piCEngineStartListingGames(a1);
}

void chatEnterChannel_Detour(void* chat, const char* channel, const char* password, void* callbacks, void* enterChannelCallback, void* param, int blocking) {
	printf("chatEnterChannel - Channel: %s Password: %s Blocking: %d\n", channel, password, blocking);
	// maybe block?
	chatEnterChannel(chat, channel, password, callbacks, enterChannelCallback, param, /*blocking*/ 1);
	printf("chatEnterChannel (finished)\n");
}

int ciProcessServerMessage_Detour(void* chat, ciServerMessage* message) {
	printf("ciProcessServerMessage: '%s' (Command: '%s' Nick: '%s' Middle: '%s' Param: '%s' numParams: %d)\n", message->message, message->command, message->nick, message->middle, message->param, message->numParams);
	return ciProcessServerMessage(chat, message);
}

int ServerListThink_Detour(int* serverlist) {
	switch (*serverlist) {
	case 1:
		printf("Gamespy ServerListThink: Server List xfer\n");
		printf("serverlist[36] = %d\n", serverlist[36]);
		break;
	case 2:
		printf("Gamespy ServerListThink: Server Lan List Query\n");
		break;
	case 3:
		printf("Gamespy ServerListThink: Server List Query Loop\n");
		break;
	}
	return ServerListThink(serverlist);
}

void ServerListAddServer_Detour(void* serverlist, int ip, uint16_t port, int a4) {
	char* ipstr = inet_ntoa(*(struct in_addr*)&ip);
	printf("ServerListAddServer: %s:%d\n", ipstr, port);
	ServerListAddServer(serverlist, ip, port, a4);
}

void peerCreateStagingRoom_Detour(void* PEER, char* name, int maxPlayers, char* password, void* callback, void* param, bool blocking) {
	printf("peerCreateStagingRoom (%p, %s, %d, %s, %p, %p, %d)\n", PEER, name, maxPlayers, password, callback, param, blocking);
	peerCreateStagingRoom(PEER, name, maxPlayers, password, callback, param, /*blocking*/ 1);
	printf("peerCreateStagingRoom (finished)\n");
}

int piNewCreateStagingRoomOperation_Detour(int a1, char* a2, int a3, int a4, int* a5)
{
	printf("piNewCreateStagingRoomOperation(%d, %s, %d, %d, %p)\n", a1, a2, a3, a4, a5);
	return piNewCreateStagingRoomOperation(a1, a2, a3, a4, a5);
}

int piCreateStagingRoomEnterChannelCallback_Detour(int chat, int success, char* channel, int a4)
{
	printf("piCreateStagingRoomEnterChannelCallback(%d, %d, %s, %d)\n", chat, success, channel, a4);
	return piCreateStagingRoomEnterChannelCallback(chat, success, channel, a4);
}

void chatEnumUsers_Detour(void* chat, char* channel, void* callback, void* param, int id)
{
	printf("chatEnumUsers(%s)\n", channel);
	chatEnumUsers(chat, channel, callback, param, id);
}

char RtoS[3][16] = { "TitleRoom", "GroupRoom", "StagingRoom" };

typedef enum
{
	PEERJoinSuccess,     // The room was joined.

	PEERFullRoom,        // The room is full.
	PEERInviteOnlyRoom,  // The room is invite only.
	PEERBannedFromRoom,  // The local user is banned from the room.
	PEERBadPassword,     // An incorrect password (or none) was given for a passworded room.

	PEERAlreadyInRoom,   // The local user is already in or entering a room of the same type.
	PEERNoTitleSet,      // Can't join a room if no title is set.
	PEERNoConnection,    // Can't join a room if there's no chat connection.
	PEERAutoMatching,    // The user can't join a staging room during an AutoMatch attempt.

	PEERJoinFailed       // Generic failure.
} PEERJoinResult;

static const char* ResultToString(int result) {
	switch (result) {
	case PEERJoinSuccess:
		return "Success";
	case PEERFullRoom:
		return "Full";
	case PEERInviteOnlyRoom:
		return "Invite Only";
	case PEERBannedFromRoom:
		return "Banned";
	case PEERBadPassword:
		return "Bad Password";
	case PEERAlreadyInRoom:
		return "Already in room";
	case PEERNoTitleSet:
		return "No Title";
	case PEERNoConnection:
		return "No Connection";
	case PEERAutoMatching:
		return "Auto Matching";
	case PEERJoinFailed:
		return "Join Failed";
	}
	return "<UNKNOWN RESULT>";
}

void BWGameSpy_JoinCallback_Detour(void* peer, int success, int result, int roomType)
{
	if (!success) {
		printf("[BWGameSpy] Join failure: %s\n", ResultToString(result));
	}
	else {
		printf("[BWGameSpy] Joined %s.\n", RtoS[roomType]);
	}

	BWGameSpy_JoinCallback(peer, success, result, roomType);
}

void BWGameSpy_RoomJoined_Detour(int roomType)
{
	printf("[BWGameSpy] Room joined %s.\n", RtoS[roomType]);
	BWGameSpy_RoomJoined(roomType);
}

bool ciInChannel_Detour(void* chat, char* channel)
{
	bool ret = ciInChannel(chat, channel);
	printf("ciInChannel: %s=%s\n", channel, ret ? "true" : "false");
	return ret;
}

struct qr_t
{
	SOCKET querysock;
	SOCKET hbsock;
	char gamename[64];
	char secret_key[128];
	void* qr_basic_callback;
	void* qr_info_callback;
	void* qr_rules_callback;
	void* qr_players_callback;
	unsigned int lastheartbeat;
	int queryid;
	int packetnumber;
	int qport;
	char no_query;
	void* udata;
};

int send_heartbeat_Detour(void* qrec, int statechanged)
{
	qr_t* qrect = (qr_t*)qrec;

	sockaddr_in* addr = (sockaddr_in*)0x00FAAC28;

	printf("\nSEND_HEARTBEAT: %s\n%d.%d.%d.%d:%d\n\n", qrect->gamename, addr->sin_addr.S_un.S_un_b.s_b1, addr->sin_addr.S_un.S_un_b.s_b2, addr->sin_addr.S_un.S_un_b.s_b3, addr->sin_addr.S_un.S_un_b.s_b4, ntohs(addr->sin_port));
	return send_heartbeat(qrec, statechanged);
}

#define DETOUR(NAME) if (DetourAttach((PVOID*)(&NAME), (PVOID)NAME ## _Detour)) { printf("Failed to detour " #NAME); }


void ShaneDetours()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());


	
	DETOUR(TownCentre_DrawPSys);
	DETOUR(DrawAll__TownCentre);
	//DETOUR(GetPlayerColour);
	DETOUR(PlayerSymbolSprite__Create);
	DETOUR(ReloadSymbols);
	DETOUR(GGame__ClearMap);
	DETOUR(GGame__StartGame);
	DETOUR(GGame__Loop);
	//DETOUR(PauseGame);
	DETOUR(BWCheckFeatureIsEnabled);
	DETOUR(GGame_ProcessKey);
	//DETOUR(GGame__Init);
	DETOUR(Process3DEngine);
	//DETOUR(EveCreatureFunction);
	//DETOUR(Creature__Create);
	DETOUR(GetRemapedPlayer);
	//DETOUR(IsMultiplayerGame);
	
	
	//DETOUR(Process3DEngine);
	//DETOUR(ControlMap__ProcessActionsPerformed);

	if (DetourTransactionCommit() == NO_ERROR)
		printf("Detours are good.\n");

}

void DoDetours()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DETOUR(BW_Report3D);
	DETOUR(BW_PathCreator);
	DETOUR(BW_PathCreator_GetProfilePath);

	if (DetourAttach((PVOID*)(&LoaderAnon_ReadVariableFile), (PVOID)LoaderAnon_ReadVariableFile_Detour))
		printf("Failed to detour LoaderAnon::ReadVariableFile");


	DETOUR(TownCentre_DrawPSys);
	if (DetourAttach((PVOID*)(&TownCentre_DrawPSys), (PVOID)TownCentre_DrawPSys_Detour))
	{
		printf("Failed to detour TownCentre_DrawPSys ");
	}
	else
	{
		printf("Detoured TownCentre_DrawPSys ");
	}

	DETOUR(peerStartListingGames);
	DETOUR(BWGameSpy_ListingGamesCallback);
	DETOUR(piNewListingGamesOperation);
	DETOUR(piListingGamesEnterChannelCallback);
	DETOUR(piListingGamesChannelMessage);
	DETOUR(_piCEngineStartListingGames);
	DETOUR(chatEnterChannel);
	DETOUR(ciProcessServerMessage);
	DETOUR(ServerListThink);
	DETOUR(ServerListAddServer);
	DETOUR(peerCreateStagingRoom);
	DETOUR(piNewCreateStagingRoomOperation);
	DETOUR(piCreateStagingRoomEnterChannelCallback);
	DETOUR(BWGameSpy_JoinCallback);
	DETOUR(BWGameSpy_RoomJoined);
	DETOUR(chatEnumUsers);
	DETOUR(ciInChannel);
	DETOUR(send_heartbeat);

	if (DetourTransactionCommit() == NO_ERROR)
		printf("Detours are good.\n");
}

void FreeDetours()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourTransactionCommit();
}

static void inline Fail(const char* a) { MessageBoxA(0, a, "Error", MB_TASKMODAL); ExitProcess(1); }

void init_mods() {
	char filepath[MAX_PATH];
	GetModuleFileName(0, filepath, MAX_PATH);

	std::string file_name = std::filesystem::path(filepath).filename().u8string();

	// if it's not runblack.exe then fuck off
	//if (file_name != "runblack.exe")
	if (file_name != "BWUltimate.exe")
		Fail(file_name.c_str());

	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	//freopen("CONIN$", "r", stdin);

	printf("Attached to process...\n");

	//DoDetours();
	ShaneDetours();
}

void deinit_mods() {
	FreeConsole();
	FreeDetours();
}
