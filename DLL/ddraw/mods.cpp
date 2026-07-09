/*

TODO: Somewhat messy, should be split into other files.

*/

#include "mods.h"
#include "intrin.h"

#include <iostream>
#include <filesystem>

#include <Windows.h>
#include <detours.h>
#include <KnownFolders.h>
#include <ShlObj.h>

#include "bwfuncs.h"
#include <windows.h>

#include <fstream>
#include <string>
#include <regex>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <set>

#pragma comment(lib, "Ws2_32.lib")

// Used to store colours for Multiplayer.
struct PlayerCustomisation
{
	int colour;
	int templeIndex;
	int team;
	int playerIndex;
	int creatureVariant;
	int tykeCreatureVariant;
};
std::unordered_map<std::wstring, PlayerCustomisation> playerCustomisations;


//Village Centre stuff

//Needs to be cleaned vars.
int Player1Templetype = -1;
bool ForceCreatureSize = true;

// Vector with all the TribeNames. This list is based on Ultimate's own custom enums and NOT the official ones!
std::vector<std::string> TribeNames = { "DEFAULT", "NORSE", "CELTIC", "AZTEC", "INDIAN", "GREEK", "JAPANESE", "EGYPTIAN", "TIBETAN", "AFRICAN" };
std::vector<std::string> RandomTribeNames = { "NORSE", "CELTIC", "AZTEC", "INDIAN", "GREEK", "JAPANESE", "EGYPTIAN", "TIBETAN" }; // , "AFRICAN" };


// Colours.
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
	DARKPURPLE = 0xFFA300FF,
	BDARKPURPLE = 0x00A300FF,
	LIMEGREEN = 0xFFA0FF00,
	BLIMEGREEN = 0x00A0FF00,
};

// For Player index lookups, mainly for MP.
DWORD ColoursLookup[] = {
	0xFFFF4646, // RED
	0xFF47FF54, // GREEN
	0xFFE347FF, // PURPLE
	0xFF47F9FF, // SKYBLUE
	0xFFFFFD47, // YELLOW
	0xFF4777FF, // BLUE
	0xFFFFA247, // ORANGE
	0x00FF4646, // BRED
	0x0047FF54, // BGREEN
	0x00E347FF, // BPURPLE
	0x0047F9FF, // BSKYBLUE
	0x00FFFD47, // BYELLOW
	0x004777FF, // BBLUE
	0x00FFA247, // BORANGE
};

enum CREATURE_TYPE
{
	CREATURE_TYPE_APE = 0,
	CREATURE_TYPE_COW = 1,
	CREATURE_TYPE_TIGER = 2,
	CREATURE_TYPE_LEOPARD = 3,
	CREATURE_TYPE_WOLF = 4,
	CREATURE_TYPE_LION = 5,
	CREATURE_TYPE_HORSE = 6,
	CREATURE_TYPE_TORTOISE = 7,
	CREATURE_TYPE_ZEBRA = 8,
	CREATURE_TYPE_BEAR = 9,
	CREATURE_TYPE_POLAR_BEAR = 10,
	CREATURE_TYPE_SHEEP = 11,
	CREATURE_TYPE_CHIMP = 12,
	CREATURE_TYPE_OGRE = 13,
	CREATURE_TYPE_MANDRILL = 14,
	CREATURE_TYPE_RHINO = 15,
	CREATURE_TYPE_GORILLA = 16,
	CREATURE_TYPE_CHICKEN = 17,
	CREATURE_TYPE_CROCODILE = 18,
	CREATURE_TYPE_EVE = 19,
	CREATURE_TYPE_LAST = 20,
};


#pragma region Utility
bool IsCI()
{
	DWORD* PartToCheck = (DWORD*)(0x00800000);
	return (*PartToCheck == 0xD4B95308);
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

// Get all the player colours.
DWORD* GetPlayerColours()
{
	return (DWORD*)(0x00C6FB88);
}

// Get all the player influence colours.
DWORD* GetPlayerInfluenceColours()
{
	return (DWORD*)(0x00F2E85C);
}

// Set colours for all Player 1.
void SetP1Colour(DWORD P1)
{
	DWORD* PlayerColours = GetPlayerColours();
	PlayerColours[0] = P1;
}

//Set influence colours for Player 1.
void SetP1InfluenceColour(DWORD P1)
{
	DWORD* PlayerInfluenceColours = GetPlayerInfluenceColours();
	PlayerInfluenceColours[0] = P1;
}

void SetAISymbolToPlayer(bool setSymbol)
{
	if (setSymbol)
	{
		BYTE* LoadSymbolOnAI = (BYTE*)(0x005F436C);
		LoadSymbolOnAI[0] = 0x8; // 1 loads khazar to ours, 2 is the AI. Anything else fails to load Symbol for AI.

		// Force AI to have Player's symbol.
		BYTE* LoadSymbolOnAI_2 = (BYTE*)(0x005F4418);
		LoadSymbolOnAI_2[0] = 0x90;
		LoadSymbolOnAI_2[1] = 0x90;
		LoadSymbolOnAI_2[2] = 0x90;
		LoadSymbolOnAI_2[3] = 0x90;
		LoadSymbolOnAI_2[4] = 0x90;
		LoadSymbolOnAI_2[5] = 0x90;

		BYTE* LoadSymbolOnAI_3 = (BYTE*)(0x005F4440);
		LoadSymbolOnAI_3[0] = 0x90;
		LoadSymbolOnAI_3[1] = 0x90;
		LoadSymbolOnAI_3[2] = 0x90;
		LoadSymbolOnAI_3[3] = 0x90;
		LoadSymbolOnAI_3[4] = 0x90;
		LoadSymbolOnAI_3[5] = 0x90;
	}
	else
	{
		BYTE* LoadSymbolOnAI = (BYTE*)(0x005F436C);
		LoadSymbolOnAI[0] = 0x2; // 1 loads khazar to Player's, 2 is the AI. Anything else fails to load Symbol for AI.

		// Force AI to have standard symbol.
		BYTE* LoadSymbolOnAI_2 = (BYTE*)(0x005F4418);
		LoadSymbolOnAI_2[0] = 0x0F;
		LoadSymbolOnAI_2[1] = 0x85;
		LoadSymbolOnAI_2[2] = 0xCA;
		LoadSymbolOnAI_2[3] = 0x00;
		LoadSymbolOnAI_2[4] = 0x00;
		LoadSymbolOnAI_2[5] = 0x00;

		BYTE* LoadSymbolOnAI_3 = (BYTE*)(0x005F4440);
		LoadSymbolOnAI_3[0] = 0x0F;
		LoadSymbolOnAI_3[1] = 0x85;
		LoadSymbolOnAI_3[2] = 0xA2;
		LoadSymbolOnAI_3[3] = 0x00;
		LoadSymbolOnAI_3[4] = 0x00;
		LoadSymbolOnAI_3[5] = 0x00;
	}
}

// Returns if Multiplayer.
bool IsMultiplayerGame()
{
	DWORD* g_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	return GGame__IsMultiplayerGame(g_GGame);
}

// Set colour for a specific Player.
void SetPlayerColour(int playerIndex, DWORD colour)
{
	if (playerIndex < 0 or playerIndex > 7)
	{
		return;
	}

	DWORD* PlayerColours = GetPlayerColours();
	PlayerColours[playerIndex] = colour;
}

// Set influence colour for a specific Player.
void SetPlayerInfluenceColour(int playerIndex, DWORD colour)
{
	DWORD* PlayerInfluenceColours = GetPlayerInfluenceColours();
	PlayerInfluenceColours[playerIndex] = colour;
}

// Set colours for all players.
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

// Set influence colours for all players.
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

// Set a Player's temple design.
void SetPlayerTemple(int playerIndex, int templeIndex)
{
	char* Evil = nullptr;
	char* Neutral = nullptr;
	char* Good = nullptr;
	char templeString[32] = {};
	sprintf(templeString, "%d.16b", templeIndex);

	int Skip = 0;
	if (playerIndex == 0)
	{
		Evil = (char*)(IsCI() ? 0x00CAD10B : 0x00CAD116);
		Neutral = (char*)(IsCI() ? 0x00CAD14E : 0x00CAD159);
		Good = (char*)(IsCI() ? 0x00CAD18B : 0x00CAD196);
	}
	else if (playerIndex == 1)
	{
		Evil = (char*)(IsCI() ? 0x00CAD1CB : 0x00CAD1D6);
		Neutral = (char*)(IsCI() ? 0x00CAD20E : 0x00CAD219);
		Good = (char*)(IsCI() ? 0x00CAD24B : 0x00CAD256);
	}
	else if (playerIndex == 2)
	{
		Evil = (char*)(IsCI() ? 0x00CAD28B : 0x00CAD296);
		Neutral = (char*)(IsCI() ? 0x00CAD2CE : 0x00CAD2D9);
		Good = (char*)(IsCI() ? 0x00CAD30B : 0x00CAD316);
	}
	else if (playerIndex == 3)
	{
		Evil = (char*)(IsCI() ? 0x00CAD34B : 0x00CAD356);
		Neutral = (char*)(IsCI() ? 0x00CAD38E : 0x00CAD399);
		Good = (char*)(IsCI() ? 0x00CAD3CB : 0x00CAD3D6);
	}
	else if (playerIndex == 4)
	{
		Evil = (char*)(IsCI() ? 0x00CAD40B : 0x00CAD416);
		Neutral = (char*)(IsCI() ? 0x00CAD44E : 0x00CAD459);
		Good = (char*)(IsCI() ? 0x00CAD48B : 0x00CAD496);
	}
	else if (playerIndex == 5)
	{
		Evil = (char*)(IsCI() ? 0x00CAD4CB : 0x00CAD4D6);
		Neutral = (char*)(IsCI() ? 0x00CAD50E : 0x00CAD519);
		Good = (char*)(IsCI() ? 0x00CAD54B : 0x00CAD556);
	}
	else if (playerIndex == 6)
	{
		Evil = (char*)(IsCI() ? 0x00CAD58B : 0x00CAD596);
		Neutral = (char*)(IsCI() ? 0x00CAD5CE : 0x00CAD5D9);
		Good = (char*)(IsCI() ? 0x00CAD60B : 0x00CAD616);
	}
	else if (playerIndex == 7)
	{
		Evil = (char*)(IsCI() ? 0x00CAD64B : 0x00CAD656);
		Neutral = (char*)(IsCI() ? 0x00CAD68E : 0x00CAD699);
		Good = (char*)(IsCI() ? 0x00CAD6CB : 0x00CAD6D6);
	}
	else
	{
		Skip = 1;
	}

	if (Skip == 0)
	{
		strncpy(Evil, templeString, 10);
		strncpy(Neutral, templeString, 10);
		strncpy(Good, templeString, 10);
	}
}

// Set a Player's temple design.
void SetTemples2(const char* Player, const char* DefaultD) {
	char* Evil = nullptr;
	char* Neutral = nullptr;
	char* Good = nullptr;
	int Skip = 0;
	if (Player == "Player1") {
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
		printf("Temple Updated maybe?");
		Player1Templetype = (int)Player1TempleTypeIG;
		Value = (int)Player1TempleTypeIG;
		_ultoa(Value, Returnvalue, 10);
		strcat(Returnvalue, Returnvalue2);
		strncpy(EvilP1, Returnvalue, 10);
		strncpy(NeutralP1, Returnvalue, 10);
		strncpy(GoodP1, Returnvalue, 10);
		if (PTValue3[0] != 0)
		{
			DWORD* PTValue4 = (DWORD*)(IsCI() ? PTValue3[0] + 0x30 : PTValue3[0] + 0x34);
			DWORD* PTValue5 = (DWORD*)(IsCI() ? PTValue4[0] + 0x40 : PTValue4[0] + 0x44);
			FLOAT* PTValue6 = (FLOAT*)(IsCI() ? PTValue5[0] + 0x88 : PTValue5[0] + 0x98);
			PTValue6 = (FLOAT*)(IsCI() ? PTValue5[0] + 0x88 : PTValue5[0] + 0x98);
			PTValue6[0] = 0;
		}
	}
}

//Creature Variables.
char CreatureFileName[50];
char ApprenticeFileName[50];
char CreatureFilePath[50];
char ApprenticeFilePath[50];
char CreatureBackupFilePath[50];

int GetRegInt(const char* key)
{
	return (int)getReg((char*)key);
}

DWORD getRegString(CHAR* keyName)
{
	// Zero all globals first so repeated calls don't accumulate stale data.
	memset(CreatureFileName, 0, sizeof(CreatureFileName));
	memset(ApprenticeFileName, 0, sizeof(ApprenticeFileName));
	memset(CreatureFilePath, 0, sizeof(CreatureFilePath));
	memset(ApprenticeFilePath, 0, sizeof(ApprenticeFilePath));
	memset(CreatureBackupFilePath, 0, sizeof(CreatureBackupFilePath));

	HKEY hKey;
	DWORD dwType = REG_DWORD;
	DWORD length = sizeof(CreatureFileName);

	DWORD* dllBase = (DWORD*)GetModuleHandleA("LHMultiplayerR.DLL");
	char* profilId = (char*)(IsCI() ? dllBase + (0x0006B288 / 4) : dllBase + (0x0006B378 / 4));

	char regFolder[300];
	sprintf(regFolder, "Software\\Lionhead Studios Ltd\\Black & White\\LHMultiplayer\\ProfilesCI\\%s", profilId);

	RegOpenKey(HKEY_CURRENT_USER, regFolder, &hKey);
	RegQueryValueEx(hKey, keyName, NULL, NULL, (LPBYTE)&CreatureFileName, &length);
	RegCloseKey(hKey);

	const char* CreatureMindPath = "./Scripts/CreatureMind/";
	const char* CreatureBackupSuffix = "-BWUTEMP";

	// Build CreatureFilePath
	strcat(CreatureFilePath, CreatureMindPath);
	strcat(CreatureFilePath, CreatureFileName);

	// Build CreatureBackupFilePath
	strcat(CreatureBackupFilePath, CreatureFilePath);
	strcat(CreatureBackupFilePath, CreatureBackupSuffix);

	// Build ApprenticeFileName (same as creature file but prefixed with "T")
	strcat(ApprenticeFileName, "T");
	strcat(ApprenticeFileName, CreatureFileName);

	// Build ApprenticeFilePath
	strcat(ApprenticeFilePath, CreatureMindPath);
	strcat(ApprenticeFilePath, ApprenticeFileName);

	printf("CreatureFile: %s\n", CreatureFileName);
	printf("ApprenticeFile: %s\n", ApprenticeFileName);
	printf("ApprenticeFilePath: %s\n", ApprenticeFilePath);
	printf("CreatureFilePath: %s\n", CreatureFilePath);
	printf("CreatureBackupFilePath: %s\n", CreatureBackupFilePath);

	return 1;
}



void InitializePlayerSettings()
{


	//DWORD* The_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	//DWORD bound6852 = *(DWORD*)((BYTE*)The_GGame + 6852);
	//DWORD bound6848 = *(DWORD*)((BYTE*)The_GGame + 6848);

	//*(DWORD*)((BYTE*)The_GGame + 6852) = 1024;
	//*(DWORD*)((BYTE*)The_GGame + 6848) = 1024;

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

	// Sort of working...Tyke appears to far on the right.
	BYTE* TykeStatScreen = (BYTE*)(IsCI() ? 0x005834E9 : 0x005834E9);
	TykeStatScreen[0] = 0xEB;

	// Creature Hold Limit increase QoL.
	BYTE* CreatureHold = (BYTE*)(IsCI() ? 0x004F41B5 : 0x004F41B5);
	CreatureHold[0] = 0x90;
	CreatureHold[1] = 0x90;

	// Enable interact check for Ogre.
	BYTE* t2 = (BYTE*)(IsCI() ? 0x004818FC : 0x004818FC);
	t2[0] = 0xFF;

	// Enable give object for Ogre.
	BYTE* t1 = (BYTE*)(IsCI() ? 0x00485015 : 0x00485015);
	t1[0] = 0xFF;


	// Disable Temple Dying.
	//*(BYTE*)0x0065FD3D = 0x00;

	// Remove original bunny.l3d thing for LoadBase + LoadMesh, let's use custom .cbn instead in LoadBinary.
	//*(BYTE*)0x00631267 = 0xEB;
	//*(BYTE*)0x0063164F = 0xEB;

	// Init Creature Paths ONCE.
	if (strlen(CreatureFilePath) <= 1)
	{
		//getRegString((char*)"file");
	}
}

// Check if Tyke exists.
bool CheckApprenticeCreatureExists()
{
	// Check if backup file exists. If not, create one.
	bool result = false;
	std::ifstream Apprentice(ApprenticeFilePath, std::ifstream::binary);
	printf("Checking path for ApprenticeFilePath: %s\n", ApprenticeFilePath);
	if (Apprentice.is_open())
	{
		printf("Tyke Exists!\n");
		result = true;
	}
	else
	{
		printf("Tyke Does Not Exist!\n");
		result = false;
	}
	Apprentice.close();
	return result;
}

void ToggleUnlimitedDrawDistance(int UpdateFlag = -1)
{
	// Registry value.
	bool UnlimitedDrawDistance = getReg((char*)"UnlimitedDrawDistance");

	// Only update registry value. Used by user inputs.
	if (UpdateFlag == 1)
	{
		setReg((char*)"UnlimitedDrawDistance", 1);
	}
	else if (UpdateFlag == 0)
	{
		setReg((char*)"UnlimitedDrawDistance", 0);
	}
	else
	{
		if (UnlimitedDrawDistance)
		{
			setReg((char*)"UnlimitedDrawDistance", 1);
		}
	}

	UnlimitedDrawDistance = getReg((char*)"UnlimitedDrawDistance");

	FLOAT* InfiniteDraw1 = (FLOAT*)(0x00CAA780);
	InfiniteDraw1[0] = (UnlimitedDrawDistance != 0 ? 7000 : 700);

	FLOAT* InfiniteDraw2 = (FLOAT*)(0x00CAB40C);
	InfiniteDraw2[0] = (UnlimitedDrawDistance != 0 ? 2300.333334 : 23.333334);
}
#pragma endregion // Utility

#pragma region GameSettings

// =====================
// Colour values - keep at top, away from slider structs
// =====================
int g_TempleValue = 0;
int g_RedValue = 0;
int g_GreenValue = 0; //(int)getReg((char*)"PlayerGreenColour");
int g_BlueValue = 0;// (int)getReg((char*)"PlayerColour");

//Let's do our own customs shit here, like colours, hard mode and multiplayer management. */

void ApplyCustomColours()
{

	// Globals from the .chl file.
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* MyRed = (FLOAT*)(IsCI() ? PTType1[0] + 40 : PTType1[0] + 44);
	FLOAT* MyGreen = (FLOAT*)(IsCI() ? PTType1[0] + 52 : PTType1[0] + 56);
	FLOAT* MyBlue = (FLOAT*)(IsCI() ? PTType1[0] + 64 : PTType1[0] + 68);
	FLOAT* LandNumber = (FLOAT*)(IsCI() ? PTType1[0] + 76 : PTType1[0] + 80);
	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);

	// Prep the Player's colour.
	DWORD Player1Colour = 0xFF000000 + getReg((char*)"PlayerColour");
	DWORD Player1InfluenceColour = getReg((char*)"PlayerColour");
	DWORD Player1TempleNum = getReg((char*)"PlayerTemple");
	BYTE* IsMultiplayer = (BYTE*)GetModuleHandleA("LHMultiplayerR.DLL") + 0x0069B50;

	/// Colour/Temple stuff.
	DWORD* g_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	if (!GGame__IsMultiplayerGame(g_GGame))
	{
		// Intro Menu.
		if (LandNumber[0] == 99)
		{
			DWORD MyInfluenceColour = (DWORD((MyRed[0])) * 65536) + (DWORD((MyGreen[0])) * 256) + (DWORD((MyBlue[0])) * 1);
			SetPlayerColours(Player1Colour, Player1Colour, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, Player1InfluenceColour, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);
		}

		// Land 2.
		else if (LandNumber[0] == 2)
		{
			SetP1Colour(Player1Colour);
			SetP1InfluenceColour(Player1InfluenceColour);
		}
		// Land 3.
		else if (LandNumber[0] == 3)
		{
			SetP1Colour(Player1Colour);
			SetP1InfluenceColour(Player1InfluenceColour);
		}

		// Land 4.
		else if (LandNumber[0] == 4)
		{
			SetP1Colour(Player1Colour);
			SetP1InfluenceColour(Player1InfluenceColour);
			SetPlayerColour(6, 0xFF000000); // Weird Ogre (Player 7) is neutral colour.
			//SetPlayerColours(Player1Colour, GREEN, Player1Colour, Player1Colour, YELLOW, BLUE, Player1Colour);
			//SetPlayerInfluenceColours(Player1InfluenceColour, BGREEN, Player1InfluenceColour, Player1InfluenceColour, BYELLOW, BBLUE, Player1InfluenceColour);
		}
		// Land 5.
		else if (LandNumber[0] == 5)
		{
			SetP1Colour(Player1Colour);
			SetP1InfluenceColour(Player1InfluenceColour);
			SetPlayerColour(6, ORANGE);
		}

		// Undead Land (just an idea...)
		else if (LandNumber[0] == 6)
		{
			SetPlayerColours(Player1Colour, GREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, BGREEN, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);
			SetPlayerTemple(1, 1); // For some reason, P2 goes back to default temple. Temp fix to manually set the temple ourselves.
		}
		// Skirmish, Land 1, Land 4 + Creature Isle.
		else
		{
			SetPlayerColours(Player1Colour, GREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
			SetPlayerInfluenceColours(Player1InfluenceColour, BGREEN, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);
			SetPlayerTemple(0, Player1TempleNum);
			SetPlayerTemple(1, 1); // For some reason, P2 goes back to default temple. Temp fix to manually set the temple ourselves.
		}
	}
	// Multiplayer!
	else
	{

		SetPlayerTemple(0, 0);
		SetPlayerTemple(1, 1);
		SetPlayerTemple(2, 2);
		SetPlayerTemple(3, 3);

		// Only update temples constantly in Multiplayer.
		std::set<int> seenTeams;
		for (auto& it : playerCustomisations)
		{
			int team = it.second.team;
			int playerIndex = it.second.playerIndex;
			int colour = it.second.colour;
			int templeIndex = it.second.templeIndex;
			int creatureVariant = it.second.creatureVariant;

			// Only process the first player we see for each team
			if (seenTeams.find(team) != seenTeams.end())
				continue;

			seenTeams.insert(team);
			//SetPlayerColour(team - 1, colour);
			//SetPlayerInfluenceColour(team - 1, 0xFF000000 - colour);
			SetPlayerTemple(team - 1, templeIndex);
		}
	}
}
void ApplyCustomSettings()
{
	// Globals from the .chl file.
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* MyRed = (FLOAT*)(IsCI() ? PTType1[0] + 40 : PTType1[0] + 44);
	FLOAT* MyGreen = (FLOAT*)(IsCI() ? PTType1[0] + 52 : PTType1[0] + 56);
	FLOAT* MyBlue = (FLOAT*)(IsCI() ? PTType1[0] + 64 : PTType1[0] + 68);
	FLOAT* SaveAllMap = (FLOAT*)(IsCI() ? PTType1[0] + 112 : PTType1[0] + 0x74);
	FLOAT* LandNumber = (FLOAT*)(IsCI() ? PTType1[0] + 76 : PTType1[0] + 80);
	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);
	FLOAT* MP_Got_Player_Stats = (FLOAT*)(IsCI() ? PTType1[0] + 328 : PTType1[0] + 236);

	// Disable pausing if in Hard Mode.
	BYTE* DisablePause = (BYTE*)(IsCI() ? 0x00655529 : 0x00655529);
	if (Difficulty[0] == 1)
	{
		DisablePause[0] = 0x90;
		DisablePause[1] = 0x90;
		DisablePause[2] = 0x90;
		DisablePause[3] = 0x90;
		DisablePause[4] = 0x90;
	}
	else
	{
		DisablePause[0] = 0xE8;
		DisablePause[1] = 0xB2;
		DisablePause[2] = 0xA2;
		DisablePause[3] = 0xF0;
		DisablePause[4] = 0xFF;
	}

	ApplyCustomColours();


	//Save map/land for Multiplayer.
	if (IsMultiplayerGame())
	{
		if (MP_Got_Player_Stats[0] == 1)
		{
			MP_Got_Player_Stats[0] = 2;
		}


		if (MP_Got_Player_Stats[0] == 2)
		{
			if (SaveAllMap[0] > 0)
			{
				//Save the current map (buildings etc.)
				_SaveAllMap((char*)"oos.txt");
				_SaveLandOnDisk((char*)"oos.lnd");
				SaveAllMap[0] = 0;
			}
		}
	}

}

int __cdecl GetRemapedPlayer_Detour(int a1)
{
	ApplyCustomColours();
	//printf("[BWFix] GetRemapedPlayer: a1=%d\n", a1);
	return GetRemapedPlayer(a1);
}

int generateRandomNumber(int start, int end)
{
	// Create a random number generator
	std::random_device rd;  // Obtain a random number from hardware.
	std::mt19937 gen(rd()); // Seed the generator.
	std::uniform_int_distribution<> distr(start, end); // Define the range between 0 and 6.

	// Generate and return a random number
	return distr(gen);
}

std::string getRandomCreateNewTownSpell()
{
	std::vector<std::string> Spells = {
		"FIRE", "FIRE_PU1", "FIRE_PU2",
		"LIGHTNING_BOLT", "LIGHTNING_BOLT_PU1", "LIGHTNING_BOLT_PU2",
		"BEAM_EXPLOSION", "BEAM_EXPLOSION_PU1", "BEAM_EXPLOSION_PU2",
		"HEAL", "HEAL_PU1",
		"TELEPORT",
		"NATURE",
		"FOOD", "FOOD_PU1",
		"STORM", "STORM_PU1", "STORM_PU2",
		"SHIELD", "PHYSICAL_SHIELD",
		"WOOD",
		"WATER", "WATER_PU1",
		"FLYING_FLOCK", "GROUND_FLOCK",
		"CREATURE_SPELL_FREEZE",
		"CREATURE_SPELL_SMALL",
		"CREATURE_SPELL_BIG",
		"CREATURE_SPELL_WEAK",
		"CREATURE_SPELL_STRONG",
		"CREATURE_SPELL_INVISIBLE",
		"CREATURE_SPELL_COMPASSION",
		"CREATURE_SPELL_ITCHY",
	};

	// Create a random number generator
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator
	std::uniform_int_distribution<> distr(0, Spells.size() - 1); // Define the range

	// Get a random index
	int randomIndex = distr(gen);

	// Return the element at the random index
	return "\"" + Spells[randomIndex] + "\")";// +""\n";
}

bool containsCreateNewTownSpell(const std::string& input)
{
	std::string search = "CREATE_NEW_TOWN_SPELL(";
	bool has_town = input.find("CREATE_NEW_TOWN_SPELL(") != std::string::npos;

	if (has_town)
	{
		return true;
		if (input.length() >= search.length() + 19)
		{
			std::string substring = input.substr(19, 1);

			// If 10 or more, make number 2 digits. Supports up to 99 number town, should be fine for now...
			if (std::isdigit(static_cast<unsigned char>(input[20])))
			{
				substring = input.substr(19, 2);
			}

			std::cout << "Spell: " << input.substr(19, 1) << "\n";
			int number = std::atoi(substring.c_str());
			return number;
		}
	}
	return false;
}

int containsCreateTown(const std::string& input)
{
	std::string search = "CREATE_TOWN(";
	bool has_town = input.find("CREATE_TOWN(") != std::string::npos;

	if (has_town)
	{
		if (input.length() >= search.length() + 12)
		{
			std::string substring = input.substr(12, 1);

			// If 10 or more, make number 2 digits. Supports up to 99 number town, should be fine for now...
			if (std::isdigit(static_cast<unsigned char>(input[13])))
			{
				substring = input.substr(12, 2);
			}

			std::cout << "Town: " << input.substr(12, 1) << "\n";
			int number = std::atoi(substring.c_str());
			return number;
		}
	}
	return -1;
}

std::string getTownTribe(const std::string& input)
{
	size_t lastQuote = input.rfind('"');
	if (lastQuote == std::string::npos)
	{
		return ""; // No double quotes found
	}

	size_t startQuote = input.rfind('"', lastQuote - 1);
	if (startQuote == std::string::npos)
	{
		return ""; // No matching opening double quote found
	}

	// Extract the string between the quotes
	std::string lastString = input.substr(startQuote + 1, lastQuote - startQuote - 1);
	return lastString;
}

// Function to extract values from the input string using regex
std::tuple<std::string, std::string, std::string, std::string> extractVillagerInfo(const std::string& input)
{
	// Use regular string literal with escaped backslashes and quotes
	std::regex pattern("CREATE_VILLAGER_POS\\(\"([^\"]+)\",\\s*\"([^\"]+)\",\\s*\"([^\"]+)\",\\s*(\\d+)\\)");
	std::smatch matches;

	// Match the pattern against the input string
	if (std::regex_search(input, matches, pattern))
	{
		// Extract the captured groups
		std::string location = matches[1].str();
		std::string building = matches[2].str();
		std::string job = matches[3].str();
		std::string age = matches[4].str();
		return std::make_tuple(location, building, job, age);
	}
	else
	{
		throw std::runtime_error("Input string does not match the expected pattern.");
	}
}

bool containsVillagerPos(const std::string& input)
{
	std::string search = "CREATE_VILLAGER_POS(";
	bool has_villager = input.find("CREATE_VILLAGER_POS(") != std::string::npos;

	if (has_villager)
	{
		std::cout << "has_villager: " << input[12] << "\n";
	}
	return has_villager;
}

void replaceStringInFile(const std::string& filename)
{
	int current_town = -1;
	std::string current_tribe = "";

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LandNumber = (FLOAT*)(IsCI() ? PTType1[0] + 76 : PTType1[0] + 80);
	FLOAT* TribeSelected = (FLOAT*)(PTType1[0] + 388);
	FLOAT* isRandomised = (FLOAT*)(PTType1[0] + 400);

	// TO-DO: Read this from the game's .chl file.
	int randomTribe = -1;
	if (isRandomised[0] > 0)
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(RandomTribeNames.begin(), RandomTribeNames.end(), std::default_random_engine(seed));
	}

	// Open the input file
	std::ifstream inFile(filename);
	if (!inFile)
	{
		//std::cout << "Error opening input file." << std::endl;
		return;
	}

	// Create a temporary file to store the modified content
	std::remove("temp.txt");
	std::ofstream outFile("temp.txt");
	if (!outFile)
	{
		return;
	}


	// Read each line from the input.
	int tribe_to_spawn = -1;
	std::string line;
	while (std::getline(inFile, line))
	{
		// Replace occurrences of oldStr with newStr
		size_t pos = 0;

		int check_town = containsCreateTown(line);
		if (check_town != -1)
		{
			tribe_to_spawn = generateRandomNumber(0, 5);
			current_town = check_town;
			std::cout << "Town detected: " << check_town << std::endl;

			// Do extra check for the town type if randomiser is on.
			//if (isRandomised[0] > 0)
			{
				current_tribe = getTownTribe(line);
				if (current_tribe == "")
				{

					isRandomised[0] = 0;
				}

				std::cout << "Town Tribe: " << current_tribe << std::endl;
			}
		}

		//bool do_replacement = false;
		const bool randomiseTribes = ((int)isRandomised[0] & 1) != 0;
		const bool randomiseSpells = ((int)isRandomised[0] & 2) != 0;
		const bool customTribe = (int)TribeSelected[0] != 0;
		bool isStartingTown = false;

		// Starting town.
		if (current_town == 0 and LandNumber[0] != 0)
		{
			isStartingTown = true;
		}

		// Also include town 1 in Land 5 as it is technically also a starting town.
		if (current_town == 1 && LandNumber[0] == 5)
		{
			isStartingTown = true;
		}


		// Replace buildings.
		if (line.find("CREATE_ABODE(") != std::string::npos or
			line.find("CREATE_TOWN_CENTRE(") != std::string::npos or
			line.find("CREATE_PLANNED_ABODE(") != std::string::npos or
			line.find("CREATE_TOWN(") != std::string::npos)
		{
			// By default, starting town is Norse. For Land 3 + CI, change the starting Tribe.
			std::string oldStr = "NORSE";
			if (LandNumber[0] == 3)
			{
				oldStr = "CELTIC";
			}
			else if (LandNumber[0] == 6)
			{
				oldStr = "EGYPTIAN";
			}

			// Decide what the new town should be.
			std::string newStr;
			if (TribeSelected[0] != 0 && isStartingTown)
			{
				newStr = TribeNames[TribeSelected[0]];
				while ((pos = line.find(oldStr, pos)) != std::string::npos)
				{
					line.replace(pos, oldStr.length(), newStr);
					pos += newStr.length();
				}
			}

			// If randomiser is enabled, apply to every town.
			else if (randomiseTribes)
			{
				oldStr = current_tribe;
				newStr = RandomTribeNames[tribe_to_spawn];
				while ((pos = line.find(oldStr, pos)) != std::string::npos)
				{
					line.replace(pos, oldStr.length(), newStr);
					pos += newStr.length();
				}
			}
		}

		if (containsVillagerPos(line))
		{
			// By default, starting town is Norse. For Land 3 + CI, change the starting Tribe.
			std::string oldStr = "NORSE";
			if (LandNumber[0] == 3)
			{
				oldStr = "CELTIC";
			}
			else if (LandNumber[0] == 6)
			{
				oldStr = "EGYPTIAN";
			}

			std::string newStr = TribeNames[TribeSelected[0]];

			// If randomiser is enabled, apply to every town.
			if (isRandomised[0] > 0)
			{
				oldStr = current_tribe;
				newStr = RandomTribeNames[tribe_to_spawn];
			}

			while ((pos = line.find(oldStr, pos)) != std::string::npos)
			{
				line.replace(pos, oldStr.length(), newStr);
				pos += newStr.length();
			}

			// Convert "CREATE_VILLAGER_POS" -> "CREATE_TOWN_VILLAGER". Fixes issue where Villager will not live in a town and occasionally, Vagrant Start.
			auto [location, building, job, age] = extractVillagerInfo(line);
			outFile << "CREATE_TOWN_VILLAGER(" << current_town << ", " << "\"" << location << "\", " << job << ", " << age << ")" << std::endl;

		}
		else if (containsCreateNewTownSpell(line) && randomiseSpells)
		{
			outFile << "CREATE_NEW_TOWN_SPELL(" << current_town << ", " << getRandomCreateNewTownSpell();
		}

		else if (line.find("CREATE_WORSHIP_SITE(") != std::string::npos && isRandomised[0] >= 0)
		{
			// Don't spawn premade Worship Sites if tribes are random. Could lead to situations where more than 6 Tribes exist!
		}
		else
		{
			// Write the modified line to the temporary file
			outFile << line << std::endl;
		}
	}

	// Close the files
	inFile.close();
	outFile.close();
}

#pragma endregion // GameSettings

#pragma region Detours
#pragma region MapDetours
int GSetup__LoadMapFeatures_Detour(const char* a1)
{
	// Load main map or Skirmish.
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LandNumber = (FLOAT*)(IsCI() ? PTType1[0] + 76 : PTType1[0] + 80);
	FLOAT* TribeSelected = (FLOAT*)(PTType1[0] + 388);
	FLOAT* isRandomised = (FLOAT*)(PTType1[0] + 400);

	bool do_custom_load = false;
	std::string mapToLoad = std::string(a1);
	std::cout << "Current Tribe: " << TribeSelected[0] << std::endl;

	// Temporary solution for Randomiser in Skirmish.
	if (GetAsyncKeyState('R') & 0x8000 && LandNumber[0] == 0)
	{
		isRandomised[0] = 3;
	}

	// Load default map if no Tribe or Randomiser selected.
	if (TribeSelected[0] == 0 and isRandomised[0] == 0)
	{
		std::cout << "No Tribe or Randomiser set! Loading default script." << std::endl;
		// Do nothing.
	}
	else if (mapToLoad.find("scripts\\playgrounds") != std::string::npos)
	{
		do_custom_load = true;
		std::cout << "Found Skirmish..." << std::endl;
		mapToLoad = "..\\" + std::string(a1);
	}
	else if (mapToLoad.find("UltimateIntro.txt") != std::string::npos)
	{
		std::cout << "Found UltimateIntro..." << std::endl;
		mapToLoad = ".\\Scripts\\CreatureIsles\\UltimateIntro.txt";
	}
	else if (mapToLoad.find("Isle01.txt") != std::string::npos)
	{
		do_custom_load = true;
		std::cout << "Found Isle01..." << std::endl;
		mapToLoad = ".\\Scripts\\CreatureIsles\\Isle01.txt";
	}
	else
	{
		std::cout << "Found Vanilla Story!" << std::endl;
		do_custom_load = true;
	}

	// Manually do stuff.
	std::cout << "a1:  Loading Map Features: " << a1 << std::endl;
	std::cout << "MTL: Loading Map Features: " << mapToLoad << std::endl;
	int MapFeatures = -1;
	if (do_custom_load)
	{
		replaceStringInFile(mapToLoad);
		int MapFeatures = GSetup__LoadMapFeatures("temp.txt");
	}
	else
	{
		int MapFeatures = GSetup__LoadMapFeatures(a1);
	}

	ApplyCustomSettings();

	DWORD* GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	int land = GGame[526021];

	// Make AI symbol the Player's for the intro menu. Allows Player to see their own icon.
	if (land == 99)
	{
		SetAISymbolToPlayer(true);
	}
	else
	{
		SetAISymbolToPlayer(false);
	}
	ReloadSymbols();

	return MapFeatures;
}

int GGame__GetCreature(int Player)
{
	int Creature = *(DWORD*)(Player + 620);
	return Creature;
}

int GGame__GetPlayer(int PlayerNum)
{
	int Player;
	DWORD* g_GGame = (DWORD*)(*(DWORD*)0x0D7B614);

	if (PlayerNum < 8)
		Player = (int)g_GGame + 640 * PlayerNum + 24;
	else
		Player = 0;
	return Player;
}



#pragma endregion // MapDetours
#pragma region CreatureDetours


// Load/save functions for CreatureMind. Need to detour these to keep our flag overrides saved.
int(__thiscall* LHOSFile__Read)(HANDLE* _this, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, int a4) = (int(__thiscall*)(HANDLE * _this, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, int a4))(0x007D50D0);
int(__thiscall* PathCreator__GetSaveGamePath)(const char** _this, char* a2) = (int(__thiscall*)(const char**, char*))(0x007A74D0);
int(__thiscall* CreatureMental__SaveMind)(int a1, const char* a2, int a3) = (int(__thiscall*)(int, const char*, int))(0x004F8840);
int(__cdecl* Creature__LoadFullyQualified)(LPCSTR lpFileName, int a2, int a3, int a4, int a5) = (int(__cdecl*)(LPCSTR, int, int, int, int))(0x004F9230);
int(__cdecl* Creature__Load)(const char* a1, int a2, int a3, int a4, int a5) = (int(__cdecl*)(const char*, int, int, int, int))(0x004F91E0);
DWORD* (__thiscall* CreaturePhysical__CreaturePhysical)(DWORD* _this, int a2, __int16 a3, int a4, int a5) = (DWORD * (__thiscall*)(DWORD*, int, __int16, int, int))(0x00500910);
int(__thiscall* Creature__Save_Orig)(unsigned __int8* _this, int a2) = (int(__thiscall*)(unsigned __int8*, int))(0x004F5730);
DWORD* (__thiscall* CreatureMental__LoadMind)(DWORD* _this, char* a2) = (DWORD * (__thiscall*)(DWORD*, char*))(0x004F9710);

DWORD* g_FemaleFlag = (DWORD*)(0x00D25B60);
DWORD* g_SaveSlot = (DWORD*)(0x00E8E8E0);
DWORD  g_overrideFemaleFlag = 0;

std::unordered_map<DWORD, std::pair<BYTE, BYTE>> g_creatureVariantMap;

BYTE  g_pendingCreatureIndex = 0;
DWORD g_pendingCreatureThis = 0;
char  g_pendingCreaturePath[260] = {};

void StoreVariantInMap(DWORD key, BYTE flag1, BYTE flag2)
{
	g_creatureVariantMap[key] = { flag1, flag2 };
}

BYTE ReadVariantFromCreatureFile(const char* path)
{
	BYTE variant = 0;
	FILE* f = fopen(path, "rb");
	if (f)
	{
		DWORD version = 0, creatureInfo = 0;
		fread(&version, 4, 1, f);
		fread(&creatureInfo, 4, 1, f);
		if (version >= 0x28)
			fread(&variant, 1, 1, f);
		fclose(f);
	}
	return variant;
}

BYTE ReadFlag2FromCreatureFile(const char* path)
{
	BYTE flag2 = 0;
	FILE* f = fopen(path, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		if (size >= 17)
		{
			fseek(f, size - 17, SEEK_SET);
			fread(&flag2, 1, 1, f);
		}
		fclose(f);
	}
	return flag2;
}

void WriteCreatureFlags(const char* path, BYTE flag1, BYTE flag2)
{
	FILE* f = fopen(path, "r+b");
	if (!f)
		return;

	fseek(f, 8, SEEK_SET);
	fwrite(&flag1, 1, 1, f);

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	if (size >= 17)
	{
		fseek(f, size - 17, SEEK_SET);
		fwrite(&flag2, 1, 1, f);
	}

	fclose(f);
}


int __cdecl Creature__Load_Detour(const char* a1, int a2, int a3, int a4, int a5)
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* FemaleCreatureOverride = (FLOAT*)(PTType1[0] + 352);
	FLOAT* LoadApprentice = (FLOAT*)(PTType1[0] + 364);
	BYTE flag1 = 0, flag2 = 0;
	int Tyke = 0;
	bool isTyke = false;

	printf("[Creature::Load] called a1=%s a2=0x%08X a3=0x%08X a4=0x%08X a5=0x%08X\n", a1 ? a1 : "(null)", a2, a3, a4, a5);
	if (a1)
	{
		if (a1[0] == 'T' || a1[0] == 't')
		{
			isTyke = true;
		}

		char mindPath[260];
		sprintf(mindPath, ".\\scripts\\CreatureMind\\%s", a1);
		printf("[Creature::Load] mindPath=%s\n", mindPath);
		fflush(stdout);

		flag1 = ReadVariantFromCreatureFile(mindPath);
		printf("[Creature::Load] flag1=0x%02X from file\n", flag1);
		fflush(stdout);

		if (a1[0] == 'T' || a1[0] == 't')
		{
			flag2 = ReadFlag2FromCreatureFile(ApprenticeFilePath);
			printf("[Creature::Load] flag2=0x%02X from ApprenticeFilePath=%s\n", flag2, ApprenticeFilePath);
		}
		else
		{
			flag2 = ReadFlag2FromCreatureFile(CreatureFilePath);
			printf("[Creature::Load] flag2=0x%02X from CreatureFilePath=%s\n", flag2, CreatureFilePath);
		}
		fflush(stdout);
	}
	else
	{
		printf("[Creature::Load] a1 is null, skipping file reads\n");
		fflush(stdout);
	}

	// Multiplayer variant support.
	if (IsMultiplayerGame())
	{
		for (int i = 0; i < 8; i++)
		{
			printf("[Creature::Load] playerIndex: %d\n", i);
			for (auto& it : playerCustomisations)
			{
				if (it.second.playerIndex - 1 == i)
				{
					flag1 = it.second.creatureVariant;
					printf("[Creature::Load] Checking Multiplayer lobby flags for Player %d's Creature! Setting flag1 to %d\n", i, flag1);
					if (isTyke)
					{
						flag1 = it.second.tykeCreatureVariant;
						printf("[Creature::Load] Checking Multiplayer lobby flags for Player %d's Tyke! Setting flag1 to %d\n", i, flag1);
					}
					g_overrideFemaleFlag = flag1;
					break;
				}
			}
			break;
		}
		g_overrideFemaleFlag = 0; // TODO - check if needed
	}

	g_overrideFemaleFlag = (DWORD)flag1;
	printf("[Creature::Load] g_overrideFemaleFlag set to 0x%02X\n", g_overrideFemaleFlag);
	fflush(stdout);

	int result = Creature__Load(a1, a2, a3, a4, a5);
	printf("[Creature::Load] result=0x%08X\n", result);
	fflush(stdout);

	if (result)
	{
		*(BYTE*)(result + 4228) |= flag2;
		StoreVariantInMap((DWORD)result, flag1, flag2);
		printf("[Creature::Load] name=%s flag1=0x%02X flag2=0x%02X creature=0x%08X\n", a1, flag1, flag2, result);
		fflush(stdout);
	}

	if (LoadApprentice[0] == 1)
	{
		printf("[Creature::Load] LoadApprentice triggered\n");
		fflush(stdout);
		flag1 = ReadVariantFromCreatureFile(ApprenticeFilePath);
		flag2 = ReadFlag2FromCreatureFile(ApprenticeFilePath);
		g_overrideFemaleFlag = flag1;

		Tyke = Creature__Load(ApprenticeFileName, a2, 0, a4, 1);
		if (Tyke)
		{
			*(BYTE*)(Tyke + 4228) |= flag2;
			StoreVariantInMap((DWORD)Tyke, flag1, flag2);
			printf("[Creature::Load] Tyke name=%s flag1=0x%02X flag2=0x%02X creature=0x%08X\n", ApprenticeFileName, flag1, flag2, Tyke);
			fflush(stdout);
			Creature__SetTyke((DWORD*)Tyke, (DWORD*)GGame__GetCreature(GGame__GetPlayer(0)));
		}

		printf("[Creature::Load] Apprentice loaded\n");
		fflush(stdout);
		LoadApprentice[0] = 0;

	}
	g_overrideFemaleFlag = 0;

	return result;
}

int __cdecl Creature__LoadFullyQualified_Detour(LPCSTR lpFileName, int a2, int a3, int a4, int a5)
{
	BYTE flag1 = 0, flag2 = 0;

	if (lpFileName)
	{
		flag1 = ReadVariantFromCreatureFile(lpFileName);

		const char* name = strrchr(lpFileName, '\\');
		if (!name) name = strrchr(lpFileName, '/');
		name = name ? name + 1 : lpFileName;

		if (name[0] == 'T' || name[0] == 't')
			flag2 = ReadFlag2FromCreatureFile(ApprenticeFilePath);
		else
			flag2 = ReadFlag2FromCreatureFile(CreatureFilePath);
	}

	printf("[Creature::LoadFullyQualified] path=%s flag1=0x%02X flag2=0x%02X\n", lpFileName, flag1, flag2);
	int result = Creature__LoadFullyQualified(lpFileName, a2, a3, a4, a5);

	if (result)
	{
		*(BYTE*)(result + 4228) |= flag2;
		StoreVariantInMap((DWORD)result, flag1, flag2);
		printf("[Creature::LoadFullyQualified] creature=0x%08X\n", result);
	}

	return result;
}

int __fastcall LHOSFile__Read_Detour(HANDLE* _this, int edx, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, int a4)
{
	// Do normal call and record the call address.
	int result = LHOSFile__Read(_this, lpBuffer, nNumberOfBytesToRead, a4);
	DWORD ret = (DWORD)_ReturnAddress();

	// Only apply CreatureMind overrides from specific calls in "Creature::Load".
	switch (ret)
	{
#define CREATURE_LOAD_READ1_RET  0x004F6B45
#define CREATURE_LOAD_READ3_RET  0x004F6BC3
#define CREATURE_LOAD_READ4_RET  0x004F6BFA
#define CREATURE_LOAD_P1_BUF_RET 0x004F6D17

	case CREATURE_LOAD_READ1_RET:
		g_overrideFemaleFlag = 0;
		g_pendingCreatureThis = 0;
		memset(g_pendingCreaturePath, 0, sizeof(g_pendingCreaturePath));
		break;

	case CREATURE_LOAD_READ3_RET:
		g_pendingCreatureIndex = *(BYTE*)lpBuffer;
		break;

	case CREATURE_LOAD_READ4_RET:
	{
		BYTE  playerIndex = *(BYTE*)lpBuffer;
		BYTE* creatureThis = (BYTE*)lpBuffer - 4337;
		g_pendingCreatureThis = (DWORD)creatureThis;

		DWORD gameInstance = *(DWORD*)0x0D7B614;
		if (!gameInstance)
			break;

		char basePath[256];
		PathCreator__GetSaveGamePath((const char**)(gameInstance + 2409552), basePath);

		DWORD saveSlot = *g_SaveSlot;
		sprintf(basePath, "%s\\%d", basePath, saveSlot);

		// Some weird hardcoded save thing, I think portal slot?
		if (saveSlot >= 15 && saveSlot <= 17)
		{
			sprintf(basePath, "%s\\%d", basePath, *(DWORD*)(gameInstance + 2104084));
		}

		char fullPath[260];
		sprintf(fullPath, "%s\\SavedCreature%d", basePath, g_pendingCreatureIndex);
		strncpy(g_pendingCreaturePath, fullPath, sizeof(g_pendingCreaturePath) - 1);

		// Skip neutral (is this needed)? Neutral might be 255...
		if (playerIndex == 0x00)
			break;

		// Store the female flags and add variant override for this Creature.
		BYTE flag1 = ReadVariantFromCreatureFile(fullPath);
		BYTE flag2 = ReadFlag2FromCreatureFile(fullPath);
		g_overrideFemaleFlag = (DWORD)flag1;
		StoreVariantInMap((DWORD)creatureThis, flag1, flag2);
		printf("[LHOSFile::Read (CREATURE_LOAD_READ4_RET)] SavedCreature%d flag1=0x%02X flag2=0x%02X creature=0x%08X\n", g_pendingCreatureIndex, flag1, flag2, (DWORD)creatureThis);

		break;
	}

	// This is only used by Player 1! Do extra setup for this.
	case CREATURE_LOAD_P1_BUF_RET:
	{
		BYTE* buf = (BYTE*)lpBuffer;
		DWORD version = *(DWORD*)(buf + 0);
		BYTE flag1 = 0, flag2 = 0;

		// TODO: Do we need this version check? Game does it so...
		if (version >= 0x28)
		{
			flag1 = buf[8];
			g_overrideFemaleFlag = (DWORD)flag1;
			flag2 = ReadFlag2FromCreatureFile(CreatureFilePath);

			if (g_pendingCreatureThis)
			{
				StoreVariantInMap(g_pendingCreatureThis, flag1, flag2);
				printf("[LHOSFile::Read] P1 flag1=0x%02X flag2=0x%02X creature=0x%08X\n", flag1, flag2, g_pendingCreatureThis);
			}
		}
		else
		{
			//*g_FemaleFlag = 0;
			g_overrideFemaleFlag = 0;
		}

		break;
	}

	default:
		break;
	}

	return result;
}


DWORD* __fastcall CreatureMental__LoadMind_Detour(DWORD* _this, int edx, char* a2)
{
	printf("[CreatureMental::LoadMind] path=%s\n", a2 ? a2 : "(null)");
	fflush(stdout);

	// Do some preprocessing with flags.
	DWORD creaturePtr = _this[13329];
	if (creaturePtr)
	{
		DWORD physicalPtr = *(DWORD*)(creaturePtr + 352);

		auto it = g_creatureVariantMap.find(physicalPtr);
		if (it == g_creatureVariantMap.end())
			it = g_creatureVariantMap.find(creaturePtr);

		if (it != g_creatureVariantMap.end())
		{
			BYTE flag2 = it->second.second;
			*(BYTE*)(creaturePtr + 4228) |= flag2;
			printf("[CreatureMental::LoadMind] LoadMind flag1=0x%02X flag2=0x%02X creaturePtr=0x%08X\n", it->second.first, flag2, creaturePtr);
		}
	}

	// Load Creature.
	DWORD* result = CreatureMental__LoadMind(_this, a2);
	if (creaturePtr)
	{
		printf("[CreatureMental::LoadMind] post-load creature+4228=0x%02X\n",
			*(BYTE*)(creaturePtr + 4228));
		fflush(stdout);
	}

	return result;
}


void BackupCreatureMind(const char* sourcePath)
{
	if (!sourcePath || sourcePath[0] == '\0')
		return;

	// Extract just the filename from the path
	const char* fileName = strrchr(sourcePath, '\\');
	if (!fileName) fileName = strrchr(sourcePath, '/');
	fileName = fileName ? fileName + 1 : sourcePath;

	// Strip .erc extension to use as folder name
	char folderName[260];
	strncpy(folderName, fileName, sizeof(folderName) - 1);
	char* dot = strrchr(folderName, '.');
	if (dot) *dot = '\0';

	// Build backup directory path
	char backupDir[260];
	sprintf(backupDir, ".\\Scripts\\CreatureMind\\%s", folderName);

	// Create directories if they don't exist
	CreateDirectoryA(".", NULL);
	CreateDirectoryA(".\\Scripts", NULL);
	CreateDirectoryA(".\\Scripts\\CreatureMind", NULL);
	CreateDirectoryA(backupDir, NULL);

	// Find the next available backup index (0-99)
	int backupIndex = -1;
	for (int i = 0; i < 100; i++)
	{
		char testPath[260];
		sprintf(testPath, "%s\\%03d_%s", backupDir, i, fileName);
		if (GetFileAttributesA(testPath) == INVALID_FILE_ATTRIBUTES)
		{
			backupIndex = i;
			break;
		}
	}

	// If all 100 slots used, find the oldest file by date and overwrite it
	if (backupIndex == -1)
	{
		FILETIME oldest;
		oldest.dwLowDateTime = 0xFFFFFFFF;
		oldest.dwHighDateTime = 0xFFFFFFFF;
		int oldestIndex = 0;

		for (int i = 0; i < 100; i++)
		{
			char testPath[260];
			sprintf(testPath, "%s\\%03d_%s", backupDir, i, fileName);

			WIN32_FILE_ATTRIBUTE_DATA fileInfo;
			if (GetFileAttributesExA(testPath, GetFileExInfoStandard, &fileInfo))
			{
				if (CompareFileTime(&fileInfo.ftLastWriteTime, &oldest) < 0)
				{
					oldest = fileInfo.ftLastWriteTime;
					oldestIndex = i;
				}
			}
		}

		backupIndex = oldestIndex;
		printf("[Backup] All slots full, overwriting oldest slot %03d\n", oldestIndex);
		fflush(stdout);
	}

	// Copy the source file to the backup slot
	char destPath[260];
	sprintf(destPath, "%s\\%03d_%s", backupDir, backupIndex, fileName);

	if (CopyFileA(sourcePath, destPath, FALSE))
	{
		printf("[Backup] Saved backup %d to %s\n", backupIndex, destPath);
		fflush(stdout);
	}
	else
	{
		printf("[Backup] Failed to copy %s to %s (err=%d)\n", sourcePath, destPath, GetLastError());
		fflush(stdout);
	}
}


DWORD GetPlayerCreaturePtr(int playerIndex)
{
	int player = (int)GGame__GetPlayer(playerIndex);
	if (!player) return 0;

	int creature = (int)GGame__GetCreature(player);
	if (!creature) return 0;

	return (DWORD)creature;
}

int(__cdecl* GScript__CreateYoungCreatureWithKnowledge)() = (int(__cdecl*)())(0x007098A0);
int __cdecl GScript__CreateYoungCreatureWithKnowledge_Detour()
{
	DWORD playerCreaturePtr = GetPlayerCreaturePtr(0);
	if (playerCreaturePtr)
	{
		auto it = g_creatureVariantMap.find(playerCreaturePtr);
		if (it != g_creatureVariantMap.end())
		{
			DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
			DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
			FLOAT* FemaleCreatureOverride = (FLOAT*)(PTType1[0] + 352); // Use .chl as if it exists in map already (which is does), it will update to the new value.
			FemaleCreatureOverride[0] = (DWORD)it->second.first;
			printf("[GScript::CreateYoungCreatureWithKnowledge] flag1=0x%02X\n", it->second.first);
			fflush(stdout);
		}
	}

	return GScript__CreateYoungCreatureWithKnowledge();
}

int BackupCreatureCounter = 3;
int BackupTykeCounter = 3;
int __fastcall CreatureMental__SaveMind_Detour(int _this, int edx, const char* a4, int a5)
{
	int result = CreatureMental__SaveMind(_this, a4, a5);
	if (!a4)
		return result;

	DWORD creaturePtr = *(DWORD*)(_this + 53316);
	DWORD physicalPtr = *(DWORD*)(creaturePtr + 352);

	printf("[CreatureMental::SaveMind] creaturePtr=0x%08X physicalPtr=0x%08X\n", creaturePtr, physicalPtr);

	auto it = g_creatureVariantMap.find(physicalPtr);
	if (it == g_creatureVariantMap.end())
		it = g_creatureVariantMap.find(creaturePtr);

	if (it != g_creatureVariantMap.end())
	{
		BYTE flag1 = it->second.first;
		BYTE flag2 = it->second.second;

		WriteCreatureFlags(a4, flag1, flag2);

		printf("[CreatureMental::SaveMind] path=%s flag1=0x%02X flag2=0x%02X creaturePtr=0x%08X\n", a4, flag1, flag2, it->first);
		bool isTyke = false;
		if (strlen(a4) >= 23 && strncmp(a4, ".\\Scripts\\CreatureMind\\", 23) == 0)
		{
			if (strlen(a4) >= 24 && strncmp(a4, ".\\Scripts\\CreatureMind\\T", 24) == 0)
			{
				setReg((char*)"TykeCreatureVariant", (DWORD)flag1);
				printf("[CreatureMental::SaveMind] Setting 'TykeCreatureVariant' to %d\n", flag1);
				if (BackupTykeCounter <= 0)
				{
					BackupCreatureMind(a4);
					BackupTykeCounter = 5;
				}
				else
				{
					BackupTykeCounter--;
				}
			}
			else
			{
				setReg((char*)"CreatureVariant", (DWORD)flag1);
				printf("[CreatureMental::SaveMind] Setting 'CreatureVariant' to %d\n", flag1);

				// Backup after flags are written
				if (BackupCreatureCounter <= 0)
				{
					BackupCreatureMind(a4);
					BackupCreatureCounter = 5;
				}
				else
				{
					BackupCreatureCounter--;
				}
			}
		}
	}


	return result;
}

// Rework some of the SwapMind functionality to support storing custom Creature flags.
int(__cdecl* Creature__SwapMinds)(int a2, int a3) = (int(__cdecl*)(int, int))(0x00486C30);
int __cdecl Creature__SwapMinds_Detour(int a2, int a3)
{
	// Store flag2 (Creature ring etc.) for both creatures before swap.
	BYTE flag2_a2 = 0, flag2_a3 = 0;
	auto it_a2 = g_creatureVariantMap.find((DWORD)a2);
	if (it_a2 != g_creatureVariantMap.end())
		flag2_a2 = it_a2->second.second;

	auto it_a3 = g_creatureVariantMap.find((DWORD)a3);
	if (it_a3 != g_creatureVariantMap.end())
		flag2_a3 = it_a3->second.second;

	printf("[Creature::SwapMinds] pre-swap a2=0x%08X flag2=0x%02X a3=0x%08X flag2=0x%02X\n", a2, flag2_a2, a3, flag2_a3);
	int result = Creature__SwapMinds(a2, a3);

	// Both female flags were swapped. We only want to swap flag1 so restore flag2 back to the original state before swap.
	auto it_a2_post = g_creatureVariantMap.find((DWORD)a2);
	if (it_a2_post != g_creatureVariantMap.end())
	{
		it_a2_post->second.second = flag2_a3;
		*(BYTE*)(a2 + 4228) = (*(BYTE*)(a2 + 4228) & 0xF8) | flag2_a3;
		printf("[Creature::SwapMinds] applied flag2=0x%02X to a2=0x%08X\n", flag2_a3, a2);
	}

	auto it_a3_post = g_creatureVariantMap.find((DWORD)a3);
	if (it_a3_post != g_creatureVariantMap.end())
	{
		it_a3_post->second.second = flag2_a2;
		*(BYTE*)(a3 + 4228) = (*(BYTE*)(a3 + 4228) & 0xF8) | flag2_a2;
		printf("[Creature::SwapMinds] applied flag2=0x%02X to a3=0x%08X\n", flag2_a2, a3);
	}

	printf("[Creature::SwapMinds] post-swap a2=0x%08X flag2=0x%02X a3=0x%08X flag2=0x%02X\n", a2, flag2_a2, a3, flag2_a3);

	// Save P1 creature
	char savePath[260];
	sprintf(savePath, ".\\Scripts\\CreatureMind\\%s", CreatureFileName);

	DWORD gameInstance = *(DWORD*)0x0D7B614;
	if (!gameInstance)
		return result;

	DWORD player0 = (DWORD)GGame__GetPlayer(0);
	if (!player0)
		return result;

	DWORD creature = (DWORD)GGame__GetCreature(player0);
	if (!creature)
		return result;

	DWORD mentalPtr = *(DWORD*)(creature + 356);
	if (!mentalPtr)
		return result;

	CreatureMental__SaveMind(mentalPtr, savePath, 0);

	DWORD creaturePtr = *(DWORD*)(mentalPtr + 53316);

	auto it = g_creatureVariantMap.find(creaturePtr);
	if (it != g_creatureVariantMap.end())
	{
		WriteCreatureFlags(savePath, it->second.first, it->second.second);
		printf("[Creature::SwapMinds] saved P1 creature to %s flag1=0x%02X flag2=0x%02X\n",
			savePath, it->second.first, it->second.second);
	}
	else
	{
		printf("[Creature::SwapMinds] saved P1 creature to %s (no flags in map)\n", savePath);
	}

	return result;
}

// If giving Creature a brotherhood ring, keep track of it.
char(__cdecl* GScript__SetObjectTattoo)() = (char(__cdecl*)())(0x0070CBD0);
char __cdecl GScript__SetObjectTattoo_Detour()
{
	char result = GScript__SetObjectTattoo();
	for (auto& entry : g_creatureVariantMap)
	{
		DWORD creaturePtr = entry.first;
		BYTE current4228 = *(BYTE*)(creaturePtr + 4228);
		if (current4228 != entry.second.second)
		{
			printf("[GScript::SetObjectTattoo] flag2 changed for creature=0x%08X old=0x%02X new=0x%02X\n",
				creaturePtr, entry.second.second, current4228);
			entry.second.second = current4228;
		}
	}

	return result;
}

void ClearGlobals()
{
	Player1Templetype = -1;
}

void __fastcall  GGame__ClearMap_Detour(void* _this)
{
	// GGame - Skirmish Flag.
	DWORD* The_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	GGame__ClearMap(_this);

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	printf("Map Cleared!\n");

	// Print state of game (Single Player, Skirmish, Multiplayer...)
	The_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	DWORD state = The_GGame[0x930B0];
	printf("(Game State): %d \n", state);

	// Join Online Game Menu. Set default colours initially.
	if (state == 3)
	{
		// Reset tracked player colours.
		//playerCustomisations.clear();

		// Set to default colours + temples for the Multiplayer Lobby.
		SetPlayerColours(RED, GREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
		SetPlayerInfluenceColours(RED, GREEN, PURPLE, SKYBLUE, BYELLOW, BBLUE, BORANGE);
		SetTemples2("Player1", "0.16b");
		SetTemples2("Player2", "1.16b");
		SetTemples2("Player3", "2.16b");
		SetTemples2("Player4", "3.16b");

		// Adjust oos.txt (MP Map) file. Change the landscape that is loaded to the landscape we create (oos.lnd).
		std::ifstream original_oos("oos.txt");
		std::ofstream new_oos("newoos.txt");

		std::string str;
		int line = 0;

		// Read in original oos.txt but replace line 3 with new landscape (oos.lnd)
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

	// Skirmish Menu.
	else if (state == 5)
	{
		// Nothing for now.
	}


	ClearGlobals();
}

int __fastcall Creature__Initialise_Detour(DWORD* _this, int edx, DWORD* a2, int a3, int a4, int a5)
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* FemaleCreatureOverride = (FLOAT*)(PTType1[0] + 352);
	int chlOverride = (int)FemaleCreatureOverride[0];
	DWORD creaturePtr = (DWORD)_this;

	// If already exists, don't add it. This is only ever hit by non P1 Creatures.
	auto it = g_creatureVariantMap.find(creaturePtr);
	if (it == g_creatureVariantMap.end())
	{
		StoreVariantInMap(creaturePtr, chlOverride, 0);
		printf("[Creature::Initialise] new entry creaturePtr=0x%08X\n", creaturePtr);
	}
	else
	{
		printf("[Creature::Initialise] already in map creaturePtr=0x%08X flag1=0x%02X flag2=0x%02X\n", creaturePtr, it->second.first, it->second.second);
	}


	if (g_overrideFemaleFlag == 1)
		printf("[Creature::Initialise] Override Female Flag Found!\n");
	else
		printf("[Creature::Initialise]\n");

	int result = Creature__Initialise(_this, a2, a3, a4, a5);

	// Reset the override flags to ensure it doesn't apply to next Creature.
	g_overrideFemaleFlag = 0;
	FemaleCreatureOverride[0] = 0;
	return result;
}

#pragma endregion // CreatureDetours



/* Custom Menu Shit. This is a bit of a mess but...works. */
#pragma region CustomMenu
#include "SetupControlCustom.h"
#include "SetupControlCustom.cpp"
#include "SetupBoxCustom.h"
#include <vector>
#include <string>
#include <set>

#define SetupSliderCi 0x008D6364

void* (__thiscall* SetupButton__SetupButton)(void* _this, int id, int x, int y, int w, int h, wchar_t* font, int a8) =
(void* (__thiscall*)(void*, int, int, int, int, int, wchar_t*, int))(0x00409840);

void* (__thiscall* SetupSlider__ctor)(void* _this, int id, int left, int top, int width, int height, int value, wchar_t* label) =
(void* (__thiscall*)(void*, int, int, int, int, int, int, wchar_t*))(0x00409B80);

void* (__thiscall* sub_7F8920)(int size) =
(void* (__thiscall*)(int))(0x007F8920);

wchar_t* g_MenuFont = (wchar_t*)(0x00CC0740);

// =====================
// Shared positions
// =====================
#define PG_LABEL_LEFT  200
#define PG_LABEL_RIGHT 600
#define PG_COL1        290
#define PG_COL2        430

SetupBoxCustom g_MyMenuBox;

// =====================
// Tab buttons
// =====================
SetupControlCustom g_TabPlayGame;
SetupControlCustom g_TabAppearance;
SetupControlCustom g_TabSettings;

wchar_t g_TabPlayGameLabel[] = L"Play Game";
wchar_t g_TabAppearanceLabel[] = L"Appearance";
wchar_t g_TabSettingsLabel[] = L"Settings";

// =====================
// Reset button
// =====================
SetupControlCustom g_ResetBtn;
wchar_t g_ResetBtnStr[] = L"Reset All";

// =====================
// Play Game tab
// =====================
SetupControlCustom g_PlayGame_StoryLabel;
SetupControlCustom g_PlayGame_Vanilla;
SetupControlCustom g_PlayGame_CreatureIsle;
SetupControlCustom g_PlayGame_DiffLabel;
SetupControlCustom g_PlayGame_Normal;
SetupControlCustom g_PlayGame_HardMode;
SetupControlCustom g_PlayGame_TribeLabel;
SetupControlCustom g_PlayGame_TribeLeft;
SetupControlCustom g_PlayGame_TribeValue;
SetupControlCustom g_PlayGame_TribeRight;
SetupControlCustom g_PlayGame_VanillaOptionsLabel;
SetupControlCustom g_PlayGame_VanillaOptionsLeft;
SetupControlCustom g_PlayGame_VanillaOptionsValue;
SetupControlCustom g_PlayGame_VanillaOptionsRight;
SetupControlCustom g_PlayGame_RandomiserLabel;
SetupControlCustom g_PlayGame_RandomiserTribes;
SetupControlCustom g_PlayGame_RandomiserSpells;
SetupControlCustom g_PlayGame_StartBtn;
SetupControlCustom g_PlayGame_NotUsableLabel;

SetupControlCustom g_Appearance_RedValue;
SetupControlCustom g_Appearance_GreenValue;
SetupControlCustom g_Appearance_BlueValue;
SetupControlCustom g_Settings_EternalVillagers;
SetupControlCustom g_Settings_CreatureBuilding;

wchar_t g_PlayGame_StoryLabelStr[] = L"Story Type";
wchar_t g_PlayGame_VanillaLabel[] = L"Vanilla";
wchar_t g_PlayGame_CreatureIsleLabel[] = L"Creature Isle";
wchar_t g_PlayGame_DiffLabelStr[] = L"Difficulty";
wchar_t g_PlayGame_NormalLabel[] = L"Normal";
wchar_t g_PlayGame_HardModeLabel[] = L"Hard Mode";
wchar_t g_PlayGame_TribeLabelStr[] = L"Tribe";
wchar_t g_PlayGame_VanillaOptionsLabelStr[] = L"Game Start";
wchar_t g_PlayGame_VanillaOptionsLeftStr[] = L"";
wchar_t g_PlayGame_VanillaOptionsRightStr[] = L"";
wchar_t g_PlayGame_VanillaOptionsValueStr[64] = L"";
wchar_t g_PlayGame_TribeLeftStr[] = L"";
wchar_t g_PlayGame_TribeRightStr[] = L"";
wchar_t g_PlayGame_TribeValueStr[32] = L"Default";
wchar_t g_PlayGame_RandomiserLabelStr[] = L"Randomiser";
wchar_t g_PlayGame_RandomiserTribesStr[] = L"Tribes";
wchar_t g_PlayGame_RandomiserSpellsStr[] = L"Spells";
wchar_t g_PlayGame_StartBtnLabel[] = L"Start Game";
wchar_t g_PlayGame_NotUsableLabelStr[] = L"Start a new game to change settings!";
wchar_t g_Appearance_RedValueStr[8] = L"255";
wchar_t g_Appearance_GreenValueStr[8] = L"128";
wchar_t g_Appearance_BlueValueStr[8] = L"128";
wchar_t g_Settings_EternalVillagersStr[] = L"Eternal Special Villagers";
wchar_t g_Settings_CreatureBuildingStr[] = L"Creature Building";

int g_Difficulty = 0;
int g_PlayGameTribeValue = 0;
int g_RandomiserTribes = 0;
int g_RandomiserSpells = 0;
int g_VanillaOptionsValue = 0;
bool g_IsStartMenuUsable = false;

std::vector<std::wstring> g_VanillaOptions = {
	L"Default (New Game)",
	L"Start at Creature Select (New Creature)",
	L"Start at Creature Tutorial (Keep Creature)",
	L"Skip Land 1 (Keep Creature)"
};

std::vector<std::wstring> g_TribeNames = {
	L"Default", L"Norse", L"Celtic", L"Aztec", L"Indian",
	L"Greek", L"Japanese", L"Egyptian", L"Tibetan", L"African"
};

// =====================
// Appearance tab
// =====================
SetupControlCustom g_Appearance_ColourLabel;
SetupSliderControl g_Appearance_RedSlider;
SetupSliderControl g_Appearance_GreenSlider;
SetupSliderControl g_Appearance_BlueSlider;
SetupControlCustom g_Appearance_TempleLabel;
SetupControlCustom g_Appearance_TempleLeft;
SetupControlCustom g_Appearance_TempleValue;
SetupControlCustom g_Appearance_TempleRight;
SetupControlCustom g_Appearance_CreatureLabel;
SetupControlCustom g_Appearance_CreatureIsleRing;

wchar_t g_Appearance_ColourLabelStr[] = L"Colour";
wchar_t g_Appearance_RedStr[] = L"Red";
wchar_t g_Appearance_GreenStr[] = L"Green";
wchar_t g_Appearance_BlueStr[] = L"Blue";
wchar_t g_Appearance_TempleLabelStr[] = L"Temple";
wchar_t g_Appearance_TempleLeftStr[] = L"";
wchar_t g_Appearance_TempleRightStr[] = L"";
wchar_t g_Appearance_TempleValueStr[32] = L"1";
wchar_t g_Appearance_CreatureLabelStr[] = L"Creature";
wchar_t g_Appearance_CreatureIsleRingStr[] = L"Creature Isle Ring";
int g_CreatureIsleRing = 0;

// =====================
// Settings tab
// =====================
SetupControlCustom g_Settings_DrawDistLabel;
SetupControlCustom g_Settings_DrawDist;

wchar_t g_Settings_DrawDistLabelStr[] = L"Settings";
wchar_t g_Settings_DrawDistStr[] = L"Unlimited Draw Distance";

int g_DrawDist = (int)getReg((char*)"UnlimitedDrawDistance");

// =====================
// Slider helper
// =====================
SetupSliderControl CreateSetupSliderCi(int left, int top, int right, int bottom, wchar_t* label, DWORD next, DWORD setupBox, void* callback, float initialValue)
{
	SetupSliderControl ctrl = {};
	SetupSlider__ctor(&ctrl, 0, left, top, right - left, bottom - top, 0, label);
	ctrl.next = next;
	ctrl.setup_box = setupBox;
	ctrl.continue_button_callback = callback;
	ctrl.field_0x22a = 1;
	ctrl.text_size = 16;
	ctrl.value = initialValue;
	ctrl.value_copy = initialValue;
	return ctrl;
}

void FixControlLink(void* ctrl, DWORD nextPtr)
{
	*(DWORD*)((BYTE*)ctrl + 0x230) = nextPtr;
}

// =====================
// Game functions
// =====================
void SetStoryType(int type)
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LoadCreatureIsle = (FLOAT*)(PTType1[0] + 340);
	LoadCreatureIsle[0] = type;
	printf("[Game] SetStoryType=%d\n", type);
	fflush(stdout);
}

void SetDifficulty(int difficulty)
{
	printf("[Game] SetDifficulty=%d\n", difficulty);
	fflush(stdout);
}

void SetColour(int r, int g, int b)
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));

	FLOAT* UseOriginalSettings = (FLOAT*)(IsCI() ? PTType1[0] + 16 : PTType1[0] + 20);
	FLOAT* MyTemple = (FLOAT*)(IsCI() ? PTType1[0] + 28 : PTType1[0] + 32);
	FLOAT* MyRed = (FLOAT*)(IsCI() ? PTType1[0] + 40 : PTType1[0] + 44);
	FLOAT* MyGreen = (FLOAT*)(IsCI() ? PTType1[0] + 52 : PTType1[0] + 56);
	FLOAT* MyBlue = (FLOAT*)(IsCI() ? PTType1[0] + 64 : PTType1[0] + 68);

	setReg((char*)"PlayerRedColour", r);
	setReg((char*)"PlayerGreenColour", g);
	setReg((char*)"PlayerBlueColour", b);
	DWORD newColour = (r * 65536) + (g * 256) + b;
	setReg((char*)"PlayerColour", newColour);

	DWORD Player1Colour = 0xFF000000 + getReg((char*)"PlayerColour");
	DWORD Player1InfluenceColour = getReg((char*)"PlayerColour");

	SetPlayerColour(0, Player1Colour);
	SetPlayerInfluenceColour(0, Player1InfluenceColour);

	printf("[Game] New Colours! SetColour=%d,%d,%d\n", r, g, b);
	fflush(stdout);
}

void SetTemple()
{
	setReg((char*)"PlayerTemple", DWORD(g_TempleValue));
	printf("[Game] SetTemple=%d\n", DWORD(g_TempleValue));
	SetPlayer1TempleType(g_TempleValue);
	fflush(stdout);
}

void StartGame()
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LoadCreatureIsle = (FLOAT*)(PTType1[0] + 340);
	FLOAT* StartGameFlag = (FLOAT*)(PTType1[0] + 412);
	FLOAT* IsSkippingToCreatureSelect = (FLOAT*)(PTType1[0] + 424);
	FLOAT* IsSkippingCreatureGuide = (FLOAT*)(PTType1[0] + 436);
	FLOAT* IsKeepingOldCreature = (FLOAT*)(PTType1[0] + 448);
	FLOAT* GameStartOption = (FLOAT*)(PTType1[0] + 460);

	// Start at Creature Select.
	if (g_VanillaOptionsValue == 1)
	{
		printf("[Game] StartGame (1)\n");
		IsSkippingToCreatureSelect[0] = 1;
		IsKeepingOldCreature[0] = 0;
		GameStartOption[0] = 1;
	}
	// Start at Creature tutorial.
	else if (g_VanillaOptionsValue == 2)
	{
		printf("[Game] StartGame (2)\n");
		IsSkippingToCreatureSelect[0] = 1;
		IsSkippingCreatureGuide[0] = 0;
		IsKeepingOldCreature[0] = 1;
		GameStartOption[0] = 2;
		//IsKeepingOldCreature[0] = 1;
	}
	// Skip Land 1 (Keep Creature)
	else if (g_VanillaOptionsValue == 3)
	{
		printf("[Game] StartGame (3)\n");
		IsSkippingToCreatureSelect[0] = 1;
		IsSkippingCreatureGuide[0] = 1;
		IsKeepingOldCreature[0] = 1;
		GameStartOption[0] = 3;
		//IsKeepingOldCreature[0] = 1;
	}
	StartGameFlag[0] = 1;

	printf("[Game] StartGame\n");
	fflush(stdout);
}

void SetDrawDistance()
{
	DWORD isOn = getReg((char*)"UnlimitedDrawDistance");

	FLOAT* InfiniteDraw1 = (FLOAT*)(0x00CAA780);
	InfiniteDraw1[0] = (isOn != 0 ? 7000 : 700);

	FLOAT* InfiniteDraw2 = (FLOAT*)(0x00CAB40C);
	InfiniteDraw2[0] = (isOn != 0 ? 2300.333334f : 23.333334f);

	printf("[Game] SetDrawDistance=%d\n", isOn);
	fflush(stdout);
}

void HideMyMenu();
void SwitchToPlayGame();
void SwitchToAppearance();
void SwitchToSettings();

// =====================
// Tribe / VanillaOptions helpers
// =====================
void UpdateTribeLabel()
{
	wcscpy(g_PlayGame_TribeValue.label, g_TribeNames[g_PlayGameTribeValue].c_str());
}

void UpdateVanillaOptionsLabel()
{
	wcscpy(g_PlayGame_VanillaOptionsValue.label, g_VanillaOptions[g_VanillaOptionsValue].c_str());
}

// =====================
// Play Game callbacks
// =====================
void OnCreatureIsle(int a2, int a3)
{
	if (a2 == 1)
	{

	}
	else if (!g_IsStartMenuUsable)
	{
		g_PlayGame_CreatureIsle.checked = !g_PlayGame_CreatureIsle.checked;
		return;
	}

	g_PlayGame_Vanilla.checked = 0;
	g_PlayGame_CreatureIsle.checked = 1;
	g_PlayGame_HardMode.hidden = 1;
	g_PlayGame_HardMode.checked = 0;
	g_PlayGame_Normal.checked = 1;
	g_Difficulty = 0;
	g_VanillaOptionsValue = 0;
	UpdateVanillaOptionsLabel();
	SetStoryType(1);
	SetDifficulty(g_Difficulty);
}

void OnVanilla(int a2, int a3)
{
	if (a2 == 1)
	{

	}
	else if (!g_IsStartMenuUsable)
	{
		g_PlayGame_Vanilla.checked = !g_PlayGame_Vanilla.checked;
		return;
	}

	g_PlayGame_Vanilla.checked = 1;
	g_PlayGame_CreatureIsle.checked = 0;
	g_PlayGame_HardMode.hidden = 0;
	SetStoryType(0);
}

void OnNormal(int a2, int a3)
{
	if (a2 == 1)
	{

	}
	else if (!g_IsStartMenuUsable)
	{
		g_PlayGame_Normal.checked = !g_PlayGame_Normal.checked;
		return;
	}

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);
	Difficulty[0] = 0;
	g_Difficulty = 0;
	g_PlayGame_Normal.checked = 1;
	g_PlayGame_HardMode.checked = 0;
	SetDifficulty(g_Difficulty);
}

void OnHardMode(int a2, int a3)
{
	if (a2 == 1)
	{

	}
	else if (!g_IsStartMenuUsable)
	{
		g_PlayGame_HardMode.checked = !g_PlayGame_HardMode.checked;
		return;
	}

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);
	Difficulty[0] = 1;
	g_Difficulty = 1;
	g_PlayGame_Normal.checked = 0;
	g_PlayGame_HardMode.checked = 1;
	SetDifficulty(g_Difficulty);
}

void OnVanillaOptionsLeft(int a2, int a3)
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LoadCreatureIsle = (FLOAT*)(PTType1[0] + 340);

	if (!g_IsStartMenuUsable) return;
	if (LoadCreatureIsle[0] == 1) return;
	if (g_VanillaOptionsValue == 0)
		g_VanillaOptionsValue = g_VanillaOptions.size() - 1;
	else
		g_VanillaOptionsValue--;
	UpdateVanillaOptionsLabel();
}

void OnVanillaOptionsRight(int a2, int a3)
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LoadCreatureIsle = (FLOAT*)(PTType1[0] + 340);

	if (!g_IsStartMenuUsable) return;
	if (LoadCreatureIsle[0] == 1) return;
	g_VanillaOptionsValue = (g_VanillaOptionsValue + 1) % g_VanillaOptions.size();
	UpdateVanillaOptionsLabel();
}

void OnPlayGameTribeLeft(int a2, int a3)
{

	if (!g_IsStartMenuUsable) return;
	if (g_PlayGameTribeValue == 0)
		g_PlayGameTribeValue = g_TribeNames.size() - 1;
	else
		g_PlayGameTribeValue--;
	UpdateTribeLabel();

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* TribeSelected = (FLOAT*)(PTType1[0] + 388);
	TribeSelected[0] = g_PlayGameTribeValue;
}

void OnPlayGameTribeRight(int a2, int a3)
{
	if (!g_IsStartMenuUsable) return;
	g_PlayGameTribeValue = (g_PlayGameTribeValue + 1) % g_TribeNames.size();
	UpdateTribeLabel();

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* TribeSelected = (FLOAT*)(PTType1[0] + 388);
	TribeSelected[0] = g_PlayGameTribeValue;
}

void OnRandomiserTribes(int a2, int a3)
{

	if (a2 == 1)
	{

	}
	else if (!g_IsStartMenuUsable)
	{
		g_PlayGame_RandomiserTribes.checked = !g_PlayGame_RandomiserTribes.checked;
		return;
	}

	g_RandomiserTribes = !g_RandomiserTribes;
	g_PlayGame_RandomiserTribes.checked = g_RandomiserTribes;

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* isRandomised = (FLOAT*)(PTType1[0] + 400);


	// Hardcode outcome, whatever. I dunno why the fuck I'm sharing with tribe + spell for randomiser.
	if (isRandomised[0] == 0)
	{
		isRandomised[0] = 1;
		g_PlayGame_RandomiserTribes.checked = true;
	}
	else if (isRandomised[0] == 1) // If already set.
	{
		g_PlayGame_RandomiserTribes.checked = false;
		isRandomised[0] = 0;
	}

	else if (isRandomised[0] == 2) // If spell set and not tribe.
	{
		g_PlayGame_RandomiserTribes.checked = true;
		isRandomised[0] = 3;
	}
	else if (isRandomised[0] == 3) // If both set.
	{
		g_PlayGame_RandomiserTribes.checked = false;
		isRandomised[0] = 2;
	}

	
	UpdateTribeLabel();

	fflush(stdout);
}

void OnRandomiserSpells(int a2, int a3)
{
	if (a2 == 1)
	{

	}
	else if (!g_IsStartMenuUsable)
	{
		g_PlayGame_RandomiserSpells.checked = !g_PlayGame_RandomiserSpells.checked;
		return;
	}

	g_RandomiserSpells = !g_RandomiserSpells;
	g_PlayGame_RandomiserSpells.checked = g_RandomiserSpells;

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* isRandomised = (FLOAT*)(PTType1[0] + 400);

	// Hardcode outcome, whatever. I dunno why the fuck I'm sharing with tribe + spell for randomiser.
	if (isRandomised[0] == 0)
	{
		isRandomised[0] = 2;
		g_PlayGame_RandomiserSpells.checked = true;
	}
	else if (isRandomised[0] == 2) // If already set.
	{
		g_PlayGame_RandomiserSpells.checked = false;
		isRandomised[0] = 0;
	}

	else if (isRandomised[0] == 1) // If tribe set and not us.
	{
		g_PlayGame_RandomiserSpells.checked = true;
		isRandomised[0] = 3;
	}
	else if (isRandomised[0] == 3) // If both set.
	{
		g_PlayGame_RandomiserSpells.checked = false;
		isRandomised[0] = 1;
	}


	fflush(stdout);
}

void OnStartGame(int a2, int a3)
{
	if (!g_IsStartMenuUsable) return;
	StartGame();
	HideMyMenu();
}

// =====================
// Appearance callbacks
// =====================
void OnTempleLeft(int a2, int a3)
{
	if (g_TempleValue <= 0) g_TempleValue = 3;
	else g_TempleValue--;
	swprintf_s(g_Appearance_TempleValueStr, 32, L"%d", g_TempleValue + 1);
	wcscpy(g_Appearance_TempleValue.label, g_Appearance_TempleValueStr);
	setReg((char*)"PlayerTemple", (DWORD)g_TempleValue);
	SetPlayer1TempleType(DWORD(g_TempleValue));
}

void OnTempleRight(int a2, int a3)
{
	g_TempleValue = (g_TempleValue + 1) % 4;
	swprintf_s(g_Appearance_TempleValueStr, 32, L"%d", g_TempleValue + 1);
	wcscpy(g_Appearance_TempleValue.label, g_Appearance_TempleValueStr);
	setReg((char*)"PlayerTemple", (DWORD)g_TempleValue);
	SetPlayer1TempleType(DWORD(g_TempleValue));
}

bool g_MyMenuInitialised = false;

void OnRedSlider(int a2, int a3)
{
	if (!g_MyMenuInitialised) return;
	SetupSliderControl* ctrl = (SetupSliderControl*)a3;
	g_RedValue = (int)(ctrl->value * 255.0f);
	swprintf_s(g_Appearance_RedValueStr, 8, L"%d", g_RedValue);
	wcscpy(g_Appearance_RedValue.label, g_Appearance_RedValueStr);
	SetColour(g_RedValue, g_GreenValue, g_BlueValue);
}

void OnGreenSlider(int a2, int a3)
{
	if (!g_MyMenuInitialised) return;
	SetupSliderControl* ctrl = (SetupSliderControl*)a3;
	g_GreenValue = (int)(ctrl->value * 255.0f);
	swprintf_s(g_Appearance_GreenValueStr, 8, L"%d", g_GreenValue);
	wcscpy(g_Appearance_GreenValue.label, g_Appearance_GreenValueStr);
	SetColour(g_RedValue, g_GreenValue, g_BlueValue);
}

void OnBlueSlider(int a2, int a3)
{
	if (!g_MyMenuInitialised) return;
	SetupSliderControl* ctrl = (SetupSliderControl*)a3;
	g_BlueValue = (int)(ctrl->value * 255.0f);
	swprintf_s(g_Appearance_BlueValueStr, 8, L"%d", g_BlueValue);
	wcscpy(g_Appearance_BlueValue.label, g_Appearance_BlueValueStr);
	SetColour(g_RedValue, g_GreenValue, g_BlueValue);
}


void OnCreatureIsleRing()
{
	g_CreatureIsleRing = !g_CreatureIsleRing;
	g_Appearance_CreatureIsleRing.checked = g_CreatureIsleRing;
	printf("[MyMenu] CreatureIsleRing=%d\n", g_CreatureIsleRing);
	fflush(stdout);

	DWORD creaturePtr = GetPlayerCreaturePtr(0);
	if (creaturePtr)
	{
		auto it = g_creatureVariantMap.find(creaturePtr);
		if (it != g_creatureVariantMap.end())
		{
			BYTE flag2 = it->second.second;

			if (g_CreatureIsleRing)
				flag2 |= 0x02;   // set ring bit
			else
				flag2 &= ~0x02;  // clear ring bit

			it->second.second = flag2;
			*(BYTE*)(creaturePtr + 4228) = (*(BYTE*)(creaturePtr + 4228) & 0xF8) | flag2;

			printf("[MyMenu] Updated P1 creature ring flag2=0x%02X\n", flag2);
			fflush(stdout);
		}
	}
}

// =====================
// Settings callbacks
// =====================
void OnDrawDist(int a2, int a3)
{
	DWORD isOn = getReg((char*)"UnlimitedDrawDistance");
	printf("[OnDrawDist] Old state=%d\n", isOn);
	setReg((char*)"UnlimitedDrawDistance", !isOn);
	isOn = getReg((char*)"UnlimitedDrawDistance");
	printf("[OnDrawDist] New state=%d\n", isOn);
	g_Settings_DrawDist.checked = isOn;
	SetDrawDistance();

	FLOAT* InfiniteDraw1 = (FLOAT*)(0x00CAA780);
	InfiniteDraw1[0] = (isOn != 0 ? 7000 : 700);
	FLOAT* InfiniteDraw2 = (FLOAT*)(0x00CAB40C);
	InfiniteDraw2[0] = (isOn != 0 ? 2300.333334f : 23.333334f);
}

void OnEternalVillagers(int a2, int a3)
{
	int EternalVillagers = GetRegInt("EternalSpecialVillagers");
	EternalVillagers = !EternalVillagers;
	g_Settings_EternalVillagers.checked = EternalVillagers;
	setReg((char*)"EternalSpecialVillagers", EternalVillagers);
	printf("[MyMenu] EternalVillagers=%d\n", EternalVillagers);
	fflush(stdout);
}

void OnCreatureBuilding(int a2, int a3)
{
	int CreatureBuilding = GetRegInt("CreatureBuilding");
	CreatureBuilding = !CreatureBuilding;
	g_Settings_CreatureBuilding.checked = CreatureBuilding;
	setReg((char*)"CreatureBuilding", CreatureBuilding);
	printf("[MyMenu] CreatureBuilding=%d\n", CreatureBuilding);
	fflush(stdout);
}

// =====================
// Tab callbacks
// =====================
void OnTabPlayGame()
{
	//if (!g_IsStartMenuUsable) return;
	SwitchToPlayGame();
}
void OnTabAppearance() { SwitchToAppearance(); }
void OnTabSettings() { SwitchToSettings(); }

// =====================
// Tab switch helpers
// =====================
void SetTabFocus(int playGame, int appearance, int settings)
{
	g_TabPlayGame.tabButtonFocus = playGame;
	g_TabAppearance.tabButtonFocus = appearance;
	g_TabSettings.tabButtonFocus = settings;
}

void SetWidgetList(SetupControlCustom* last)
{
	DWORD* box = (DWORD*)&g_MyMenuBox;
	box[27] = (DWORD)last;
}

void SwitchToPlayGame()
{
	SetTabFocus(1, 0, 0);
	g_TabPlayGame.next = (DWORD)&g_PlayGame_NotUsableLabel;
	SetWidgetList(&g_ResetBtn);
	printf("[MyMenu] Switched to PlayGame\n"); fflush(stdout);
}

void SwitchToAppearance()
{
	SetTabFocus(0, 1, 0);
	g_TabPlayGame.next = (DWORD)&g_Appearance_CreatureIsleRing;
	SetWidgetList(&g_ResetBtn);
}

void SwitchToSettings()
{
	SetTabFocus(0, 0, 1);
	g_TabPlayGame.next = (DWORD)&g_Settings_CreatureBuilding;
	SetWidgetList(&g_ResetBtn);
}

// =====================
// Reset all callback
// =====================
void OnResetAll(int a2, int a3)
{
	if (g_IsStartMenuUsable)
	{
		// Play Game defaults
		g_PlayGame_Vanilla.checked = 1;
		g_PlayGame_CreatureIsle.checked = 0;
		g_PlayGame_HardMode.hidden = 0;
		g_Difficulty = 0;
		g_PlayGame_Normal.checked = 1;
		g_PlayGame_HardMode.checked = 0;
		g_PlayGameTribeValue = 0;
		UpdateTribeLabel();
		g_VanillaOptionsValue = 0;
		UpdateVanillaOptionsLabel();
		g_RandomiserTribes = 0;
		g_RandomiserSpells = 0;
		g_PlayGame_RandomiserTribes.checked = 0;
		g_PlayGame_RandomiserSpells.checked = 0;


		DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
		DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
		FLOAT* TribeSelected = (FLOAT*)(PTType1[0] + 388);
		TribeSelected[0] = 0;
		FLOAT* isRandomised = (FLOAT*)(PTType1[0] + 400);
		isRandomised[0] = 0;

		SetStoryType(0);
		SetDifficulty(0);
	}

	// Appearance defaults
	g_RedValue = 255;
	g_GreenValue = 70;
	g_BlueValue = 70;
	SetColour(g_RedValue, g_GreenValue, g_BlueValue);

	g_TempleValue = 0;
	swprintf_s(g_Appearance_TempleValueStr, 32, L"%d", g_TempleValue + 1);
	wcscpy(g_Appearance_TempleValue.label, g_Appearance_TempleValueStr);
	setReg((char*)"PlayerTemple", 0);
	SetPlayer1TempleType(0);

	g_Appearance_RedSlider.value = g_RedValue / 255.0f;
	g_Appearance_RedSlider.value_copy = g_RedValue / 255.0f;
	g_Appearance_GreenSlider.value = g_GreenValue / 255.0f;
	g_Appearance_GreenSlider.value_copy = g_GreenValue / 255.0f;
	g_Appearance_BlueSlider.value = g_BlueValue / 255.0f;
	g_Appearance_BlueSlider.value_copy = g_BlueValue / 255.0f;

	// Ring
	g_CreatureIsleRing = 1;
	g_Appearance_CreatureIsleRing.checked = 0;
	OnCreatureIsleRing();

	swprintf_s(g_Appearance_RedValueStr, 8, L"%d", g_RedValue);
	swprintf_s(g_Appearance_GreenValueStr, 8, L"%d", g_GreenValue);
	swprintf_s(g_Appearance_BlueValueStr, 8, L"%d", g_BlueValue);
	wcscpy(g_Appearance_RedValue.label, g_Appearance_RedValueStr);
	wcscpy(g_Appearance_GreenValue.label, g_Appearance_GreenValueStr);
	wcscpy(g_Appearance_BlueValue.label, g_Appearance_BlueValueStr);

	// Settings defaults
	g_DrawDist = 1;
	g_Settings_DrawDist.checked = 1;
	setReg((char*)"UnlimitedDrawDistance", 1);
	SetDrawDistance();

	g_Settings_EternalVillagers.checked = false;
	setReg((char*)"EternalSpecialVillagers", 0);

	g_Settings_CreatureBuilding.checked = true;
	setReg((char*)"CreatureBuilding", true);

	printf("[MyMenu] Reset all to defaults\n");
	fflush(stdout);
}


// =====================
// Callback dispatcher
// =====================
int __stdcall MyMenuCallback(int a1, int a2, int a3, int a4, int a5)
{
	int result = 0;

	if (a1 == 13 && g_TabAppearance.tabButtonFocus == 1)
	{
		void (*sub_413D90)(int, int, int, int, int, int, int, int, int, int) =
			(void(*)(int, int, int, int, int, int, int, int, int, int))(0x00413D90);

		int red = 0xFF000000 | (g_RedValue << 16);
		int green = 0xFF000000 | (g_GreenValue << 8);
		int blue = 0xFF000000 | (g_BlueValue);
		int final = 0xFF000000 | (g_RedValue << 16) | (g_GreenValue << 8) | g_BlueValue;

		sub_413D90(253, 65, 546, 91, red, red, red, red, 1, 1);
		sub_413D90(253, 100, 546, 126, green, green, green, green, 1, 1);
		sub_413D90(253, 135, 546, 161, blue, blue, blue, blue, 1, 1);
		sub_413D90(253, 175, 546, 225, final, final, final, final, 1, 1);
	}

	if (a1 == 1 && a3)
	{
		void(__cdecl * callback)(int, int) = *(void(__cdecl**)(int, int))(a3 + 568);
		if (callback)
			callback(a2, a3);
	}

	return result;
}

DWORD g_OriginalListHead = 0;
void InitMyMenu()
{
	if (g_MyMenuInitialised)
		return;

	g_OriginalListHead = *(DWORD*)(*(DWORD*)0x00CC072C + 108);

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* EternalSpecialVillagers = (FLOAT*)(PTType1[0] + 460);

	printf("[MyMenu] InitMyMenu called! r=%d g=%d b=%d\n", g_RedValue, g_GreenValue, g_BlueValue);
	fflush(stdout);

	DWORD* box = (DWORD*)&g_MyMenuBox;
	box[0] = 0x00906AA8;
	box[1] = 0x00000000;
	box[2] = 0x00000000;
	box[3] = 0x00000000;
	box[4] = 0x00000000;
	box[5] = 0x00000000;
	box[6] = 0x3E4CCCCD;
	box[7] = 0x3E4CCCCD;
	box[8] = 0x3F800000;
	box[9] = 0x00000000;
	box[10] = 0xC395FFF8;
	box[11] = 0x45BB7FF5;
	box[12] = 0xC72FC7F7;
	box[13] = 0x00000000;
	box[14] = 0x00000000;
	box[15] = 0x00000000;
	box[16] = 0x00000000;
	box[17] = 0x00000000;
	box[18] = 0x3E4CCCCD;
	box[19] = 0x3E4CCCCD;
	box[20] = 0x00000000;
	box[21] = 0x00000000;
	box[22] = 0x00000000;
	box[23] = 0x00000000;
	box[24] = 0x00000000;
	box[25] = 0x00000000;
	box[26] = 0x00000000;
	box[27] = 0x00000000;
	box[28] = 0x00000000;
	box[29] = 0x00000000;
	box[30] = 0x00000001;
	box[31] = 0x00000190;
	box[32] = 0x00000140;
	box[33] = 0x3F800000;
	box[34] = 0x00000000;
	box[35] = 0x00000000;
	box[36] = 0x00000000;
	box[37] = 0x00000001;
	box[38] = 0x00000000;
	box[39] = 0x000001AC;
	box[40] = 0x00000258;
	box[41] = 0x00000258;
	box[42] = 0x000000C8;
	box[43] = 0x00000023;
	box[44] = (DWORD)MyMenuCallback;
	box[45] = 0x00000000;
	box[46] = 0x00000000;
	box[47] = 0x00000000;
	box[48] = 0xFFFFFFFF;
	box[49] = 0x3F800000;
	box[50] = 0x00000000;
	box[51] = 0x00000000;

	// =====================
	// Play Game tab controls
	// =====================
	g_PlayGame_StoryLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 25, PG_LABEL_RIGHT, 55,
		g_PlayGame_StoryLabelStr,
		0x00000000, (DWORD)&g_MyMenuBox,
		nullptr, 1, 991, 30
	);

	g_PlayGame_Vanilla = CreateSetupCheckBoxCi(
		PG_COL1, 60, PG_COL1 + 25, 85,
		g_PlayGame_VanillaLabel,
		(DWORD)&g_PlayGame_StoryLabel, (DWORD)&g_MyMenuBox,
		OnVanilla, 0, 1
	);

	g_PlayGame_CreatureIsle = CreateSetupCheckBoxCi(
		PG_COL2, 60, PG_COL2 + 25, 85,
		g_PlayGame_CreatureIsleLabel,
		(DWORD)&g_PlayGame_Vanilla, (DWORD)&g_MyMenuBox,
		OnCreatureIsle, 0, 0
	);

	g_PlayGame_DiffLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 115, PG_LABEL_RIGHT, 145,
		g_PlayGame_DiffLabelStr,
		(DWORD)&g_PlayGame_CreatureIsle, (DWORD)&g_MyMenuBox,
		nullptr, 1, 992, 30
	);

	g_PlayGame_Normal = CreateSetupCheckBoxCi(
		PG_COL1, 150, PG_COL1 + 25, 175,
		g_PlayGame_NormalLabel,
		(DWORD)&g_PlayGame_DiffLabel, (DWORD)&g_MyMenuBox,
		OnNormal, 0, 1
	);

	g_PlayGame_HardMode = CreateSetupCheckBoxCi(
		PG_COL2, 150, PG_COL2 + 25, 175,
		g_PlayGame_HardModeLabel,
		(DWORD)&g_PlayGame_Normal, (DWORD)&g_MyMenuBox,
		OnHardMode, 0, 0
	);

	g_PlayGame_VanillaOptionsLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 205, PG_LABEL_RIGHT, 235,
		g_PlayGame_VanillaOptionsLabelStr,
		(DWORD)&g_PlayGame_HardMode, (DWORD)&g_MyMenuBox,
		nullptr, 1, 996, 30
	);

	g_PlayGame_VanillaOptionsLeft = CreateSetupBigButtonCi(
		270, 240, 300, 270,
		g_PlayGame_VanillaOptionsLeftStr,
		(DWORD)&g_PlayGame_VanillaOptionsLabel, (DWORD)&g_MyMenuBox,
		OnVanillaOptionsLeft, 0, _typeLeftArrow
	);

	wcscpy(g_PlayGame_VanillaOptionsValueStr, g_VanillaOptions[g_VanillaOptionsValue].c_str());
	g_PlayGame_VanillaOptionsValue = CreateSetupStaticTextCi(
		300, 243, 500, 268,
		g_PlayGame_VanillaOptionsValueStr,
		(DWORD)&g_PlayGame_VanillaOptionsLeft, (DWORD)&g_MyMenuBox,
		nullptr, 1, 997, 25
	);
	UpdateVanillaOptionsLabel();

	g_PlayGame_VanillaOptionsRight = CreateSetupBigButtonCi(
		500, 240, 530, 270,
		g_PlayGame_VanillaOptionsRightStr,
		(DWORD)&g_PlayGame_VanillaOptionsValue, (DWORD)&g_MyMenuBox,
		OnVanillaOptionsRight, 0, _typeRightArrow
	);

	g_PlayGame_TribeLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 300, PG_LABEL_RIGHT, 330,
		g_PlayGame_TribeLabelStr,
		(DWORD)&g_PlayGame_VanillaOptionsRight, (DWORD)&g_MyMenuBox,
		nullptr, 1, 993, 30
	);

	g_PlayGame_TribeLeft = CreateSetupBigButtonCi(
		310, 335, 340, 365,
		g_PlayGame_TribeLeftStr,
		(DWORD)&g_PlayGame_TribeLabel, (DWORD)&g_MyMenuBox,
		OnPlayGameTribeLeft, 0, _typeLeftArrow
	);

	wcscpy(g_PlayGame_TribeValueStr, g_TribeNames[g_PlayGameTribeValue].c_str());
	g_PlayGame_TribeValue = CreateSetupStaticTextCi(
		340, 338, 460, 363,
		g_PlayGame_TribeValueStr,
		(DWORD)&g_PlayGame_TribeLeft, (DWORD)&g_MyMenuBox,
		nullptr, 1, 994, 25
	);
	UpdateTribeLabel();

	g_PlayGame_TribeRight = CreateSetupBigButtonCi(
		460, 335, 490, 365,
		g_PlayGame_TribeRightStr,
		(DWORD)&g_PlayGame_TribeValue, (DWORD)&g_MyMenuBox,
		OnPlayGameTribeRight, 0, _typeRightArrow
	);

	g_PlayGame_RandomiserLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 395, PG_LABEL_RIGHT, 425,
		g_PlayGame_RandomiserLabelStr,
		(DWORD)&g_PlayGame_TribeRight, (DWORD)&g_MyMenuBox,
		nullptr, 1, 995, 30
	);

	g_PlayGame_RandomiserTribes = CreateSetupCheckBoxCi(
		PG_COL1, 430, PG_COL1 + 25, 455,
		g_PlayGame_RandomiserTribesStr,
		(DWORD)&g_PlayGame_RandomiserLabel, (DWORD)&g_MyMenuBox,
		OnRandomiserTribes, 0, 0
	);

	g_PlayGame_RandomiserSpells = CreateSetupCheckBoxCi(
		PG_COL2, 430, PG_COL2 + 25, 455,
		g_PlayGame_RandomiserSpellsStr,
		(DWORD)&g_PlayGame_RandomiserTribes, (DWORD)&g_MyMenuBox,
		OnRandomiserSpells, 0, 0
	);

	g_PlayGame_StartBtn = CreateSetupBigButtonCi(
		260, 490, 540, 550,
		g_PlayGame_StartBtnLabel,
		(DWORD)&g_PlayGame_RandomiserSpells, (DWORD)&g_MyMenuBox,
		OnStartGame, 0, _typeUnchecked
	);
	g_PlayGame_StartBtn.setupControlTypCi = SetupButtonCi;
	g_PlayGame_StartBtn.text_size = 0;

	g_PlayGame_NotUsableLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 500, PG_LABEL_RIGHT, 530,
		g_PlayGame_NotUsableLabelStr,
		(DWORD)&g_PlayGame_StartBtn, (DWORD)&g_MyMenuBox,
		nullptr, 1, 998, 22
	);

	// =====================
	// Appearance tab controls
	// =====================
	g_Appearance_ColourLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 25, PG_LABEL_RIGHT, 55,
		g_Appearance_ColourLabelStr,
		0x00000000, (DWORD)&g_MyMenuBox,
		nullptr, 1, 981, 30
	);

	swprintf_s(g_Appearance_RedValueStr, 8, L"%d", g_RedValue);
	g_Appearance_RedValue = CreateSetupStaticTextCi(
		200, 65, 248, 95,
		g_Appearance_RedValueStr,
		(DWORD)&g_Appearance_ColourLabel, (DWORD)&g_MyMenuBox,
		nullptr, 2, 988, 16
	);

	g_Appearance_RedSlider = CreateSetupSliderCi(
		253, 65, 546, 95,
		g_Appearance_RedStr,
		(DWORD)&g_Appearance_RedValue, (DWORD)&g_MyMenuBox,
		OnRedSlider, g_RedValue / 255.0f
	);

	swprintf_s(g_Appearance_GreenValueStr, 8, L"%d", g_GreenValue);
	g_Appearance_GreenValue = CreateSetupStaticTextCi(
		200, 100, 248, 130,
		g_Appearance_GreenValueStr,
		(DWORD)&g_Appearance_RedSlider, (DWORD)&g_MyMenuBox,
		nullptr, 2, 989, 16
	);

	g_Appearance_GreenSlider = CreateSetupSliderCi(
		253, 100, 546, 130,
		g_Appearance_GreenStr,
		(DWORD)&g_Appearance_GreenValue, (DWORD)&g_MyMenuBox,
		OnGreenSlider, g_GreenValue / 255.0f
	);

	swprintf_s(g_Appearance_BlueValueStr, 8, L"%d", g_BlueValue);
	g_Appearance_BlueValue = CreateSetupStaticTextCi(
		200, 135, 248, 165,
		g_Appearance_BlueValueStr,
		(DWORD)&g_Appearance_GreenSlider, (DWORD)&g_MyMenuBox,
		nullptr, 2, 990, 16
	);

	g_Appearance_BlueSlider = CreateSetupSliderCi(
		253, 135, 546, 165,
		g_Appearance_BlueStr,
		(DWORD)&g_Appearance_BlueValue, (DWORD)&g_MyMenuBox,
		OnBlueSlider, g_BlueValue / 255.0f
	);

	g_Appearance_TempleLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 235, PG_LABEL_RIGHT, 265,
		g_Appearance_TempleLabelStr,
		(DWORD)&g_Appearance_BlueSlider, (DWORD)&g_MyMenuBox,
		nullptr, 1, 982, 30
	);

	g_Appearance_TempleLeft = CreateSetupBigButtonCi(
		310, 272, 340, 302,
		g_Appearance_TempleLeftStr,
		(DWORD)&g_Appearance_TempleLabel, (DWORD)&g_MyMenuBox,
		OnTempleLeft, 0, _typeLeftArrow
	);

	g_TempleValue = getReg((char*)"PlayerTemple");
	swprintf_s(g_Appearance_TempleValueStr, 32, L"%d", g_TempleValue + 1);
	g_Appearance_TempleValue = CreateSetupStaticTextCi(
		340, 275, 460, 300,
		g_Appearance_TempleValueStr,
		(DWORD)&g_Appearance_TempleLeft, (DWORD)&g_MyMenuBox,
		nullptr, 1, 983, 25
	);
	wcscpy(g_Appearance_TempleValue.label, g_Appearance_TempleValueStr);

	g_Appearance_TempleRight = CreateSetupBigButtonCi(
		460, 272, 490, 302,
		g_Appearance_TempleRightStr,
		(DWORD)&g_Appearance_TempleValue, (DWORD)&g_MyMenuBox,
		OnTempleRight, 0, _typeRightArrow
	);

	g_Appearance_CreatureLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 320, PG_LABEL_RIGHT, 350,
		g_Appearance_CreatureLabelStr,
		(DWORD)&g_Appearance_TempleRight, (DWORD)&g_MyMenuBox,
		nullptr, 1, 984, 30
	);

	g_Appearance_CreatureIsleRing = CreateSetupCheckBoxCi(
		PG_COL1, 355, PG_COL1 + 25, 380,
		g_Appearance_CreatureIsleRingStr,
		(DWORD)&g_Appearance_CreatureLabel, (DWORD)&g_MyMenuBox,
		OnCreatureIsleRing, 0, g_CreatureIsleRing
	);

	// =====================
	// Settings tab controls
	// =====================
	g_Settings_DrawDistLabel = CreateSetupStaticTextCi(
		PG_LABEL_LEFT, 25, PG_LABEL_RIGHT, 55,
		g_Settings_DrawDistLabelStr,
		0x00000000, (DWORD)&g_MyMenuBox,
		nullptr, 1, 961, 30
	);

	g_Settings_DrawDist = CreateSetupCheckBoxCi(
		PG_COL1, 65, PG_COL1 + 25, 90,
		g_Settings_DrawDistStr,
		(DWORD)&g_Settings_DrawDistLabel, (DWORD)&g_MyMenuBox,
		OnDrawDist, 0, g_DrawDist
	);

	g_Settings_EternalVillagers = CreateSetupCheckBoxCi(
		PG_COL1, 100, PG_COL1 + 25, 125,
		g_Settings_EternalVillagersStr,
		(DWORD)&g_Settings_DrawDist, (DWORD)&g_MyMenuBox,
		OnEternalVillagers, 0, GetRegInt("EternalSpecialVillagers")
	);
	g_Settings_EternalVillagers.checked = GetRegInt("EternalSpecialVillagers");

	g_Settings_CreatureBuilding = CreateSetupCheckBoxCi(
		PG_COL1, 135, PG_COL1 + 25, 160,
		g_Settings_CreatureBuildingStr,
		(DWORD)&g_Settings_EternalVillagers, (DWORD)&g_MyMenuBox,
		OnCreatureBuilding, 0, GetRegInt("CreatureBuilding")
	);

	// =====================
	// Tab buttons
	// =====================
	g_TabPlayGame = CreateSetupTabButtonCi(
		190, -40, 333, 0,
		g_TabPlayGameLabel,
		(DWORD)&g_PlayGame_StartBtn, (DWORD)&g_MyMenuBox,
		OnTabPlayGame, 1, 0xFFFFFFFF
	);

	g_TabAppearance = CreateSetupTabButtonCi(
		333, -40, 476, 0,
		g_TabAppearanceLabel,
		(DWORD)&g_TabPlayGame, (DWORD)&g_MyMenuBox,
		OnTabAppearance, 0, 0xFFFFFFFF
	);

	g_TabSettings = CreateSetupTabButtonCi(
		476, -40, 618, 0,
		g_TabSettingsLabel,
		(DWORD)&g_TabAppearance, (DWORD)&g_MyMenuBox,
		OnTabSettings, 0, 0xFFFFFFFF
	);

	// =====================
	// Reset button
	// =====================
	g_ResetBtn = CreateSetupBigButtonCi(
		330, 560, 460, 585,
		g_ResetBtnStr,
		(DWORD)&g_TabSettings, (DWORD)&g_MyMenuBox,
		OnResetAll, 0, _typeUnchecked
	);
	g_ResetBtn.setupControlTypCi = SetupButtonCi;
	g_ResetBtn.text_size = 14;

	box[27] = (DWORD)&g_ResetBtn;

	g_MyMenuInitialised = true;
	g_Settings_DrawDist.checked = int(getReg((char*)"UnlimitedDrawDistance"));

	printf("[MyMenu] InitMyMenu done! r=%d g=%d b=%d\n", g_RedValue, g_GreenValue, g_BlueValue);
	fflush(stdout);
}

void LoadColourFromRegistry()
{
	HKEY hKey;
	DWORD* dllBase = (DWORD*)GetModuleHandleA("LHMultiplayerR.DLL");
	char* profilId = (char*)(dllBase + (0x0006B288 / 4));
	char regFolder[300];
	sprintf(regFolder, "Software\\Lionhead Studios Ltd\\Black & White\\LHMultiplayer\\ProfilesCI\\%s", profilId);

	if (RegOpenKeyEx(HKEY_CURRENT_USER, regFolder, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD value, len = sizeof(DWORD), type = REG_DWORD;

		if (RegQueryValueEx(hKey, "PlayerRedColour", NULL, &type, (LPBYTE)&value, &len) == ERROR_SUCCESS)
			g_RedValue = (int)value;
		else
			g_RedValue = 255;

		type = REG_DWORD; len = sizeof(DWORD);
		if (RegQueryValueEx(hKey, "PlayerGreenColour", NULL, &type, (LPBYTE)&value, &len) == ERROR_SUCCESS)
			g_GreenValue = (int)value;
		else
			g_GreenValue = 70;

		type = REG_DWORD; len = sizeof(DWORD);
		if (RegQueryValueEx(hKey, "PlayerBlueColour", NULL, &type, (LPBYTE)&value, &len) == ERROR_SUCCESS)
			g_BlueValue = (int)value;
		else
			g_BlueValue = 70;

		RegCloseKey(hKey);
	}
	else
	{
		g_RedValue = 255;
		g_GreenValue = 70;
		g_BlueValue = 70;
	}
}


bool IsMenuVisible()
{
	DWORD* menuPointer = (DWORD*)(0x00CC0730);
	return menuPointer[0] == (DWORD)&g_MyMenuBox;
}

void LoadCHLVariableValues()
{
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));

	FLOAT* LoadCreatureIsle = (FLOAT*)(PTType1[0] + 340);
	if (LoadCreatureIsle[0] == 1)
	{
		OnCreatureIsle(1, 0);
	}
	else
	{
		OnVanilla(1, 0);
	}


	FLOAT* Difficulty = (FLOAT*)(IsCI() ? PTType1[0] + 88 : PTType1[0] + 92);
	if (Difficulty[0] == 1)
	{
		OnHardMode(1, 0);
	}
	else
	{
		OnNormal(1, 0);
	}

	
	if (LoadCreatureIsle[0] == 1)
	{
		OnCreatureIsle(1, 0);
	}
	else
	{
		OnVanilla(1, 0);
	}

	// Tribe Selected.
	FLOAT* TribeSelected = (FLOAT*)(PTType1[0] + 388);
	g_PlayGameTribeValue = TribeSelected[0];
	wcscpy(g_PlayGame_TribeValue.label, g_TribeNames[g_PlayGameTribeValue].c_str());

	// Game Start Option.
	FLOAT* GameStartOption = (FLOAT*)(PTType1[0] + 460);
	g_VanillaOptionsValue = GameStartOption[0];
	wcscpy(g_PlayGame_VanillaOptionsValue.label, g_VanillaOptions[g_VanillaOptionsValue].c_str());


	FLOAT* LoadApprentice = (FLOAT*)(PTType1[0] + 364);
	FLOAT* isRandomised = (FLOAT*)(PTType1[0] + 400);


	// Randomiser.
	g_PlayGame_RandomiserSpells.checked = false;
	g_PlayGame_RandomiserTribes.checked = false;
	if (isRandomised[0] == 3)
	{
		g_PlayGame_RandomiserSpells.checked = true;
		g_PlayGame_RandomiserTribes.checked = true;
	}

	else if (isRandomised[0] == 2)
	{
		g_PlayGame_RandomiserSpells.checked = true;
		g_PlayGame_RandomiserTribes.checked = false;
	}

	else if (isRandomised[0] == 1)
	{
		g_PlayGame_RandomiserSpells.checked = false;
		g_PlayGame_RandomiserSpells.checked = true;
	}

}

void ShowMyMenu()
{
	// Don't allow on Multiplayer.
	if (IsMultiplayerGame())
		return;


	printf("[MyMenu] ShowMyMenu called!\n");
	fflush(stdout);


	LoadColourFromRegistry();
	InitMyMenu();
	if (g_IsStartMenuUsable)
	{
		g_PlayGame_StartBtn.hidden = 0;
		g_PlayGame_NotUsableLabel.hidden = 1;
	}
	else
	{
		g_PlayGame_StartBtn.hidden = 1;
		g_PlayGame_NotUsableLabel.hidden = 0;
	}
	SwitchToPlayGame();
	LoadCHLVariableValues();

	DWORD* box = (DWORD*)&g_MyMenuBox;
	box[2] = 0x3F800000;

	DWORD* menuPointer = (DWORD*)(0x00CC0730);
	menuPointer[0] = (DWORD)&g_MyMenuBox;

	printf("[MyMenu] Shown\n");
	fflush(stdout);
}

void HideMyMenu()
{

	if (IsMultiplayerGame())
		return;
	if (!g_MyMenuInitialised) return;

	printf("[MyMenu] HideMyMenu called!\n");
	fflush(stdout);

	DWORD* menuPointer = (DWORD*)(0x00CC0730);
	menuPointer[0] = 0x00000000;

	*(DWORD*)(*(DWORD*)0x00CC072C + 108) = g_OriginalListHead;

	// Unregister all controls from game's global linked list
	auto UnregisterControl = [](void* ctrl) {
		*(DWORD*)((BYTE*)ctrl + 0x230) = 0;
		*(DWORD*)((BYTE*)ctrl + 0x234) = 0;
		};

	UnregisterControl(&g_PlayGame_StoryLabel);
	UnregisterControl(&g_PlayGame_Vanilla);
	UnregisterControl(&g_PlayGame_CreatureIsle);
	UnregisterControl(&g_PlayGame_DiffLabel);
	UnregisterControl(&g_PlayGame_Normal);
	UnregisterControl(&g_PlayGame_HardMode);
	UnregisterControl(&g_PlayGame_VanillaOptionsLabel);
	UnregisterControl(&g_PlayGame_VanillaOptionsLeft);
	UnregisterControl(&g_PlayGame_VanillaOptionsValue);
	UnregisterControl(&g_PlayGame_VanillaOptionsRight);
	UnregisterControl(&g_PlayGame_TribeLabel);
	UnregisterControl(&g_PlayGame_TribeLeft);
	UnregisterControl(&g_PlayGame_TribeValue);
	UnregisterControl(&g_PlayGame_TribeRight);
	UnregisterControl(&g_PlayGame_RandomiserLabel);
	UnregisterControl(&g_PlayGame_RandomiserTribes);
	UnregisterControl(&g_PlayGame_RandomiserSpells);
	UnregisterControl(&g_PlayGame_StartBtn);
	UnregisterControl(&g_PlayGame_NotUsableLabel);
	UnregisterControl(&g_Appearance_ColourLabel);
	UnregisterControl(&g_Appearance_RedSlider);
	UnregisterControl(&g_Appearance_GreenSlider);
	UnregisterControl(&g_Appearance_BlueSlider);
	UnregisterControl(&g_Appearance_TempleLabel);
	UnregisterControl(&g_Appearance_TempleLeft);
	UnregisterControl(&g_Appearance_TempleValue);
	UnregisterControl(&g_Appearance_TempleRight);
	UnregisterControl(&g_Settings_DrawDistLabel);
	UnregisterControl(&g_Settings_DrawDist);
	UnregisterControl(&g_TabPlayGame);
	UnregisterControl(&g_TabAppearance);
	UnregisterControl(&g_TabSettings);
	UnregisterControl(&g_ResetBtn);
	UnregisterControl(&g_Appearance_RedValue);
	UnregisterControl(&g_Appearance_GreenValue);
	UnregisterControl(&g_Appearance_BlueValue);
	UnregisterControl(&g_Appearance_CreatureLabel);
	UnregisterControl(&g_Appearance_CreatureIsleRing);
	UnregisterControl(&g_Settings_EternalVillagers);
	UnregisterControl(&g_Settings_CreatureBuilding);

	g_MyMenuInitialised = false;

	printf("[MyMenu] Hidden\n");
	fflush(stdout);
}
#pragma endregion


#pragma region GameLoopDetours
void __fastcall GGame_ProcessKey_Detour(unsigned __int8* _this, int edx, int a2, int a3)
{
	//Print Key being pressed.
	printf("Pressed Key: %d \n", a2);

	if (a2 == 13)
	{
		ShowMyMenu();
	}
	else if (a2 == 12)
	{
		HideMyMenu();
	}

	if (a2 == 26)
	{
		//SnapshotSpecialVillagers();//PrintLiveSpecialVillagers();
	}

	if (a2 == 27)
	{
		//CheckSpecialVillagers();
	}

	GGame_ProcessKey(_this, a2, a3);
}


DWORD* __fastcall GGame__Loop_Detour(char* _this, int edx)
{
	//Print stuff.
	printf("GGame::Loop - Starting...\n");

	//Normal Loop.
	ApplyCustomSettings();
	ToggleUnlimitedDrawDistance();

	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LoadApprentice = (FLOAT*)(PTType1[0] + 364);
	if (!CheckApprenticeCreatureExists())
	{
		LoadApprentice[0] = -1;
	}
	else
	{
		LoadApprentice[0] = 0;
	}

	return GGame__Loop(_this);
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
		printf("Football Addon is enabled. \n");
	}


	DWORD Player1Colour = 0xFF000000 + getReg((char*)"PlayerColour");
	DWORD Player1InfluenceColour = getReg((char*)"PlayerColour");
	
	//SetPlayerColours(Player1Colour, BGREEN, PURPLE, SKYBLUE, YELLOW, BLUE, ORANGE);
	//SetPlayerInfluenceColours(Player1InfluenceColour, BGREEN, BPURPLE, BSKYBLUE, BYELLOW, BBLUE, BORANGE);

	printf("[BWCheckFeatureIsEnabled] Called!\n");
	return BWCheckFeatureIsEnabled(_this);
}


/*
UNUSED HARDCODED SEEDS
std::vector<long long> seeds = {
	3278705849, 1387893725, 3068197897, 1413822335, 1039522479, 1198805754,
	3771358875, 2195104923, 706981165, 291776083, 1485843199,
	1509410232, 3971704074, 2516594991, 2071128141, 2321964920,
	2059107960, 1924951452, 534593368, 3652794474, 1011862364,
	3453564479, 4092594656, 3589888718, 3278705849, 2311189537,
	2258493795, 2165762022, 2754297890, 302686658, 4147735297,
	2751754564, 2233349575, 2553789567, 3441313085, 568462898,
	3532179792, 24978096, 2893825181, 1462148109, 3898730960,
	2239714219, 2069292734, 3538833615, 3521048035, 1695724861,
	2715914757, 1881422637, 2170878173, 2574150701, 157297988,
	1696292561, 1384348769, 2832660295, 607342525, 4125613022,
	468323813, 2938356024, 3147105999, 285871647, 317786747,
	1104574600, 2470217384, 237547338, 1397555686, 1948892565,
	2628216650, 323518068, 3734672127, 2123320663, 615905280,
	3875367560, 591348442, 3217588606, 650452592, 1249432064,
	3070139425, 3439796391, 1329041561, 3368358460, 3390922310,
	925494738, 4018976385, 2147715400, 3644329020, 886828994,
	2936627273, 3057857407, 502307851, 2656391710, 3278705849
};

std::vector<float> randFloats = {
	88.1971f, 354.253f, 796.49f, 5.58147f, 46.9072f, 5.49327f, 124.903f, 4.44587f, 0.228183f, 71.7052f,
	2.86076f, 4.56472f, 75.1415f, 13.0922f, 3.43042f, 7.70153f, 12.065f, 247.654f, 154.086f, 452.872f,
	0.77774f, 17.8882f, 2.00705f, 35.3919f, 544.121f, 436.439f, 71.7174f, 1.18003f, 9.13184f, 0.887901f,
	28.2541f, 3.01604f, 3.76645f, 15.4116f, 569.284f, 443.763f, 5.37525f, 4.00128f, 896.315f, 143.038f,
	59.8611f, 876.356f, 371.908f, 70.8629f, 0.188299f, 0.480201f, 6.14896f, 17.5575f, 15.3487f, 0.522424f,
	13.1412f, 0.941802f, 148.348f, 897.23f, 749.065f, 0.645816f, 6.0824f, 26.7686f, 5.07794f, 3.80636f,
	0.635557f, 9.29787f, 318.456f, 398.825f, 254.994f, 5.58147f, 12.5644f, 96.2692f, 603.25f, 247.654f,
	0.968148f, 6.79309f, 0.77774f, 17.8882f, 2.00705f, 35.3919f, 0.100573f, 17.8465f, 606.729f, 421.302f,
	28.5191f, 5.9691f, 12.3163f, 5.33009f, 5.74708f, 3.82437f, 13.6683f, 3.24557f, 29.284f, 29.4517f,
	472.801f, 88.1971f, 354.253f, 5.0045f, 1.68231f, 5.83363f, 26.2284f, 4.07566f, 1.08949f, 4.11305f,
	24.7366f, 37.1368f, 163.317f, 438.91f, 679.652f, 3.82437f, 11.3902f, 1.83131f, 6.81639f, 222.293f,
	265.324f, 618.158f, 0.99135f, 4.33417f, 25.9464f, 5.73737f, 21.7949f, 5.28934f, 48.4043f, 383.139f,
	139.62f, 946.807f, 0.932099f, 13.4585f, 516.548f, 813.443f, 736.294f, 654.612f, 466.728f, 928.42f,
	0.604877f, 33.782f, 569.284f, 443.763f, 472.801f, 0.554159f, 5.3138f, 545.968f, 96.2692f, 603.25f,
	1.55606f, 2.31128f, 0.232114f, 8.9485f, 5.50631f, 6.69435f, 978.637f, 331.273f, 383.719f, 1.27389f,
	12.0459f, 0.0477778f, 0.774182f, 0.625467f, 728.008f, 101.152f, 516.548f, 5.11101f, 11.0444f, 1.66708f,
	11.1268f, 545.968f, 96.2692f, 603.25f, 0.100573f, 17.8465f, 891.814f, 702.342f, 978.637f, 194.324f,
	15.4116f, 569.284f, 96.2692f, 603.25f, 247.654f, 1.55606f, 4.62257f, 1.65078f, 18.4411f, 6.11636f,
	26.2907f, 6.22959f, 5.71664f, 383.139f, 139.62f, 946.807f, 888.319f, 837.629f, 9.33852f, 5.26298f,
	0.280156f, 0.376119f, 27.3939f, 3.43042f, 1.73285f, 0.513909f, 0.123781f, 973.449f, 876.356f, 371.908f,
	0.445245f, 0.449531f, 848.31f, 383.139f, 139.62f, 5.94896f, 4.45045f, 35.8892f, 4.70652f, 1.54177f,
	4.80872f, 24.2454f, 1.83131f, 6.45122f, 4.59724f, 648.661f, 36.3165f, 896.315f, 1.91335f, 1.9469f,
	0.876356f, 16.0067f, 991.47f, 190.555f, 0.00765332f, 0.00161636f, 0.00126879f, 4.56245e-05f, 0.000774182f,
	625.467f, 452.888f, 819.44f, 1.02615f, 6.58366f, 222.293f, 265.324f, 618.158f, 5.63171f, 8.01013f,
	0.232114f, 8.9485f, 5.50631f, 6.69435f, 544.121f, 436.439f, 71.7174f, 1.18003f, 9.13184f, 0.932099f,
	50.2449f, 4.80872f, 64.6543f, 2.53758f, 318.456f, 398.825f, 254.994f, 876.356f, 371.908f, 70.8629f,
	3.81219f, 7.58343f, 0.179191f, 50.3506f, 32.8435f, 0.209247f, 0.765332f, 679.652f, 608.667f, 759.35f,
	1.22097f, 0.909285f, 22.7713f, 1.66708f, 11.1268f, 5.73737f, 26.1539f, 37.5708f, 0.654612f, 319.432f,
	631.998f, 262.73f, 2.06908f, 11.6711f, 606.729f, 421.302f, 28.5191f, 5.07794f, 3.80636f, 837.629f,
	9.33852f, 270.344f, 0.698835f, 4.87068f, 2.84548f, 0.664958f
};
*/


// COOP Stuff


// Multiplayer Lobby Stuff


// Multiplayer Lobby Stuff
int GetMessageID(wchar_t firstChar)
{
	int result = firstChar - 65;
	if (result > 26 && firstChar < 0x41)
		result = -1;
	return result;
}


void(__cdecl* MPFEMessageObject__ProcessIncomingMessage)(int a1, wchar_t* a2) = (void(__cdecl*)(int a1, wchar_t* a2))(0x0063DAB0);
int(__cdecl* sub_63DA80)(unsigned short a1) = (int(__cdecl*)(unsigned short a1))(0x0063DA80); // Get Message ID function.
#pragma endregion // GameLoopDetours
#pragma region MultiplayerDetours
void MPFEMessageObject__ProcessIncomingMessage_Detour(int a1, wchar_t* a2)
{
	int msgID = sub_63DA80(*(WORD*)a1);
	wchar_t* msg = (wchar_t*)(a1 + 2);

	int playerIndex = *((int*)((BYTE*)a2 + 260));
	int team = *((int*)((BYTE*)a2 + 272));

	printf("MPFEMessageObject__ProcessIncomingMessage_Detour: msgID=%d msg=%ls playerIndex=%d team=%d\n", msgID, msg, playerIndex, team);

	if (msgID == 26)
	{
		int colour = 0;
		int templeIndex = 0;
		int creatureVariant = 0;
		int tykeCreatureVariant = 0;

		wchar_t token[32] = {};
		int tokenPos = 0;
		int part = 0;
		int pos = 0;

		while (msg[pos] != L'\0')
		{
			if (msg[pos] == L'-')
			{
				token[tokenPos] = L'\0';
				if (part == 0) colour = wcstol(token, nullptr, 16);
				if (part == 1) templeIndex = wcstol(token, nullptr, 10);
				if (part == 2) creatureVariant = wcstol(token, nullptr, 10);
				if (part == 3) tykeCreatureVariant = wcstol(token, nullptr, 10);
				tokenPos = 0;
				part++;
			}
			else
			{
				token[tokenPos++] = msg[pos];
			}
			pos++;
		}

		DWORD* playerArray = (DWORD*)0x00DC0304;

		// Flush last token
		token[tokenPos] = L'\0';
		if (part == 0) colour = wcstol(token, nullptr, 16);
		if (part == 1) templeIndex = wcstol(token, nullptr, 10);
		if (part == 2) creatureVariant = wcstol(token, nullptr, 10);
		if (part == 3) tykeCreatureVariant = wcstol(token, nullptr, 10);

		PlayerCustomisation customisation = {};
		customisation.colour = colour;
		customisation.templeIndex = templeIndex;
		customisation.team = team;
		customisation.playerIndex = playerIndex;
		customisation.creatureVariant = creatureVariant;
		customisation.tykeCreatureVariant = tykeCreatureVariant;

		playerCustomisations[std::wstring(a2)] = customisation;

		printf("[MPFEMessageObject::ProcessIncomingMessage]: stored name=%ls colour=0x%06X templeIndex=%d team=%d playerIndex=%d creatureVariant=%d team=%d tykeCreatureVariant=%d\n",
			a2, customisation.colour, customisation.templeIndex, customisation.team, customisation.playerIndex, customisation.creatureVariant, customisation.tykeCreatureVariant);


		return;
	}

	MPFEMessageObject__ProcessIncomingMessage(a1, a2);
}


int GetFirstPlayerOnTeam(int team)
{
	int* teamList = (int*)0x00DC0300;
	int node = teamList[2 * team];

	if (!node)
		return 0;

	return *(int*)node;
}


char* (__thiscall* sub_63C330)(char* _this) = (char* (__thiscall*)(char* _this))(0x0063C330); // Get Player Name thing?
void UpdateTeamColours()
{
	for (int team = 0; team < 5; team++)
	{
		int playerObj = GetFirstPlayerOnTeam(team);
		if (!playerObj)
		{
			SetPlayerColour(team - 1, ColoursLookup[team - 1]);
			SetPlayerInfluenceColour(team - 1, ColoursLookup[team - 1 + 7]);
			//printf("[UpdateTeamColours] No Players found for team=%d\n", team);
			continue;
		}
		int playerIndex = *((int*)(playerObj + 260));

		//printf("[UpdateTeamColours] team=%d playerIndex=%d\n", team, playerIndex);

		for (auto& it : playerCustomisations)
		{
			if (it.second.playerIndex == playerIndex)
			{
				SetPlayerColour(team - 1, 0xFF000000 + it.second.colour);
				SetPlayerInfluenceColour(team - 1, it.second.colour);
				SetPlayerTemple(team - 1, it.second.templeIndex);
				//printf("[UpdateTeamColours] Found team=%d name=%ls colour=0x%06X templeIndex=%d\n", team, it.first.c_str(), it.second.colour, it.second.templeIndex);
				break;
			}
		}
	}
}
int* (__thiscall* MPFEData__GetLocalPlayer)(DWORD* _this) = (int* (__thiscall*)(DWORD * _this))(0x0063A0E0);
int* __fastcall MPFEData__GetLocalPlayer_Detour(DWORD* _this, int edx)
{
	int* localPlayer = MPFEData__GetLocalPlayer(_this);
	if (!localPlayer)
		return localPlayer;

	int      localIndex = *((int*)((BYTE*)localPlayer + 260));
	int      localTeam = *((int*)((BYTE*)localPlayer + 272));
	wchar_t* localName = (wchar_t*)sub_63C330((char*)_this);

	if (localTeam == -1 || !localName)
		return localPlayer;

	PlayerCustomisation customisation = {};
	customisation.colour = (int)getReg((char*)"PlayerColour");
	customisation.templeIndex = (int)getReg((char*)"PlayerTemple");
	customisation.team = localTeam;
	customisation.playerIndex = localIndex;
	customisation.creatureVariant = (int)getReg((char*)"CreatureVariant");
	customisation.tykeCreatureVariant = (int)getReg((char*)"TykeCreatureVariant");

	playerCustomisations[std::wstring(localName)] = customisation;

	DWORD lastPrint = 0;
	DWORD now = GetTickCount();
	if (now - lastPrint >= 3000)
	{
		lastPrint = now;
		
		printf("[MPFEData::GetLocalPlayer] Customisation hashmap contents (%d entries):\n", (int)playerCustomisations.size());
		for (auto& it : playerCustomisations)
			printf("  name=%ls colour=0x%06X templeIndex=%d team=%d playerIndex=%d, creatureVariant=%d, tykeCreatureVariant=%d\n",
				it.first.c_str(), it.second.colour, it.second.templeIndex, it.second.team, it.second.playerIndex, it.second.creatureVariant, it.second.tykeCreatureVariant);
				
	}
	UpdateTeamColours();

	return localPlayer;
}

int(__thiscall* MPFEMessageObject__Send)(DWORD* _this, int a2) = (int(__thiscall*)(DWORD * _this, int a2))(0x0063DA60);
int __fastcall MPFEMessageObject__Send_Detour(DWORD* _this, int edx, int a2)
{
	wchar_t* name = *((wchar_t**)_this + 1);
	int msgID = GetMessageID(name[0]);

	printf("MPFEMessageObject::Send: id(%d), a2:(%d)\n", msgID, a2);
	int result = MPFEMessageObject__Send(_this, a2);

	// If chat, do an extra colour message and for certain object messages!
	if (name[0] < L'L')// or name[0] == L'A')
	{
		//int myColour = (int)getReg((char*)"PlayerColour");
		//int myTemple = (int)getReg((char*)"PlayerTemple");
		//wcscpy(name, L" FFFF00-1-");
		swprintf(name, 64, L" %06X-%d-%d-%d", (int)getReg((char*)"PlayerColour"), (int)getReg((char*)"PlayerTemple"), (int)getReg((char*)"CreatureVariant"), (int)getReg((char*)"TykeCreatureVariant"));
		wchar_t* name = *((wchar_t**)_this + 1);
		name[0] = '['; // Set message ID to 26.
		int result2 = MPFEMessageObject__Send(_this, 0);
		//std::wcout << L"Contents: " << name << std::endl;
	}

	return result;
}


void CustomSaveTyke()
{
	// Ripped from "GGame::ClearMap"
	DWORD* GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	char v27[256];

	int v13 = 160 * *((unsigned __int8*)GGame + 2104165);
	int v14 = GGame[v13 + 161];
	int v7 = (int)&GGame[v13];
	if (*(DWORD*)(v14 + 4828))
	{
		int v15 = *(DWORD*)(v14 + 4824);
		if (v15)
		{
			int v16 = *(DWORD*)(v15 + 4);
			if (v16)
			{
				DWORD* v17 = (DWORD*)GPlayer__GetLeaderInterfaceStatus((DWORD*)GGame__GetPlayer(0));
				int v18 = GInterfaceStatus__GetInterface(v17);
				sprintf(v27, ".\\Scripts\\CreatureMind\\T%s", (const char*)(*(DWORD*)(v18 + 444) + 4));
				CreatureMental__SaveMind(*(DWORD*)(v16 + 356), v27, 0);
			}
		}
	}
}

bool g_WasMenuVisible = false;

void CheckMenuVisibility()
{
	DWORD* menuPointer = (DWORD*)(0x00CC0730);
	bool isVisible = (menuPointer[0] == (DWORD)&g_MyMenuBox);

	if (g_WasMenuVisible && !isVisible)
	{
		printf("[MyMenu] Menu closed externally, calling HideMyMenu\n");
		fflush(stdout);
		HideMyMenu();
	}

	g_WasMenuVisible = isVisible;
}

// Used as the onTick() hook.
int __fastcall Process3DEngine_Detour(BYTE* _this, int edx)
{
	int result = Process3DEngine(_this);


	// 
	BYTE* IsMultiplayer = (BYTE*)GetModuleHandleA("LHMultiplayerR.DLL") + 0x0069B50;

	//TODO: Move this from tick method, terrible, terrible solution.
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* LandNumber = (FLOAT*)(IsCI() ? PTType1[0] + 76 : PTType1[0] + 80);
	FLOAT* LoadApprentice = (FLOAT*)(PTType1[0] + 364);
	FLOAT* StartGame = (FLOAT*)(PTType1[0] + 412);

	DWORD* The_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	DWORD state = The_GGame[0x930B0];


	FLOAT* isRandomised = (FLOAT*)(PTType1[0] + 400);
	//printf("[MyMenu] isRandomised=%d\n", (int)isRandomised[0]);


	// Menu stuff. If 2 (set from .chl), open it.
	if (StartGame[0] == 2)
	{
		ShowMyMenu();
		StartGame[0] = 0;
	}

	// Ensure we properly hide/deinit menu if custom menu is closed by something else in game (such as pressing ESC).
	CheckMenuVisibility();


	// GGame - Skirmish Flag.
	//The_GGame[0x806C6] = 0;


	// If Player is deleting Tyke, ensure we save him.
	if (LoadApprentice[0] == 2)
	{
		CustomSaveTyke();
		LoadApprentice[0] = 0;
	}

	// Constantly apply in custom intro menu to give real time colour updates.
	if (LandNumber[0] == 99)
	{
		g_IsStartMenuUsable = true;
	}
	else
	{
		g_IsStartMenuUsable = false;
	}

	ApplyCustomSettings();


	return result;
}


// Original function pointer

// --- Expand Creature slots ---
DWORD* newCreatures = new DWORD[100]();
int* newCreatureInfoTable = new int[10050]();
int* newCreatureBalanceTable = new int[2100];

// New Creature entries.
struct NewCreatureEntry
{
	const char* bname;
	const char* name;
	int         helpStart;
	int         helpEnd;
	int         baseSlot; // Inherits from whatever original Creature is at that block (0 = Ape etc.).
	int			totemMesh;

	static NewCreatureEntry newCreatureEntries[];
	static const int        count;
};

NewCreatureEntry NewCreatureEntry::newCreatureEntries[] = {
	{ "beve", "Eve",				1111,	1222,	CREATURE_TYPE_APE,	476},
	{ "bundeadape", "UndeadApe",	1111,	1222,	CREATURE_TYPE_APE,	476},
	{ "bfox",		"Fox",			8854,	8855,	CREATURE_TYPE_WOLF, -1},
};
const int NewCreatureEntry::count = ARRAYSIZE(NewCreatureEntry::newCreatureEntries);

int __fastcall GBalance__LoadVariables_Detour(void* _this, int edx, int a2)
{
	// --- Original load call. ---
	int result = GBalance__LoadVariables(_this, a2);

	// --- Size for Creature data tables and counts. ---
	const int ORIGINAL_CREATURES = 19;
	const int CREATURE_INFO_SIZE = 230;
	const int CREATURE_BALANCE_SIZE = 27;
	const int TOTAL_CREATURES = ORIGINAL_CREATURES + NewCreatureEntry::count;

	// --- Copy original "info.dat" information to our new "info.dat". ---
	memcpy(&newCreatureInfoTable[0], (void*)(0xCD6A88), 920 * ORIGINAL_CREATURES);
	memcpy(&newCreatureBalanceTable[0], (void*)(0xCD6280), 108 * ORIGINAL_CREATURES);

	// --- Redirect CBN tables ---
	DWORD* oldCreatures = (DWORD*)(0x00A3F2AC);
	for (int i = 0; i < ORIGINAL_CREATURES * 2; i++)
	{
		newCreatures[i] = oldCreatures[i];
	}
	((DWORD*)(0x004F847C))[0] = (DWORD)&newCreatures[0];
	((DWORD*)(0x005009BF))[0] = (DWORD)&newCreatures[0];
	((DWORD*)(0x005574FF))[0] = (DWORD)&newCreatures[0];
	((DWORD*)(0x005092C5))[0] = (DWORD)&newCreatures[0];

	// --- Redirect Creature Info tables ---
	DWORD* infoPointers[] = {
		(DWORD*)(0x0042D9D6), (DWORD*)(0x0042DA37), (DWORD*)(0x00474F3D),
		(DWORD*)(0x0047C7A3), (DWORD*)(0x0047C7DB), (DWORD*)(0x00482B05),
		(DWORD*)(0x005F7953), (DWORD*)(0x004F6DDC), (DWORD*)(0x004F9495),
		(DWORD*)(0x005082A8), (DWORD*)(0x0056A0BD), (DWORD*)(0x0065250D),
		(DWORD*)(0x00652680), (DWORD*)(0x006526C5), (DWORD*)(0x006539AF),
		(DWORD*)(0x00670DEB), (DWORD*)(0x00707006), (DWORD*)(0x00707036),
		(DWORD*)(0x007099A7), (DWORD*)(0x0073035B), (DWORD*)(0x007324A6),
	};
	for (auto site : infoPointers)
	{
		site[0] = (DWORD)&newCreatureInfoTable[0];
	}
	((DWORD*)(0x00501942))[0] = (DWORD)&newCreatureInfoTable[76];
	((DWORD*)(0x0070704F))[0] = (DWORD)&newCreatureInfoTable[460];
	((DWORD*)(0x0042D9EE))[0] = (DWORD)&newCreatureInfoTable[ORIGINAL_CREATURES * CREATURE_INFO_SIZE];
	((DWORD*)(0x0042DAB3))[0] = (DWORD)&newCreatureInfoTable[ORIGINAL_CREATURES * CREATURE_INFO_SIZE];
	((DWORD*)(0x0047C817))[0] = (DWORD)&newCreatureInfoTable[TOTAL_CREATURES * CREATURE_INFO_SIZE];
	((int*)(0x0047C81C))[0] = TOTAL_CREATURES;
	((BYTE*)(0x004F9447))[0] = TOTAL_CREATURES;

	// --- Redirect Creature Balance tables. ---
	((DWORD*)(0x0042DAD1))[0] = (DWORD)&newCreatureBalanceTable[0];
	((DWORD*)(0x0042DB2A))[0] = (DWORD)&newCreatureBalanceTable[0];
	((DWORD*)(0x0047C883))[0] = (DWORD)&newCreatureBalanceTable[0];
	((DWORD*)(0x0047C8BB))[0] = (DWORD)&newCreatureBalanceTable[0];
	((DWORD*)(0x00487586))[0] = (DWORD)&newCreatureBalanceTable[5];
	((DWORD*)(0x0042DB3F))[0] = (DWORD)&newCreatureBalanceTable[ORIGINAL_CREATURES * CREATURE_BALANCE_SIZE];
	((DWORD*)(0x0042DB65))[0] = (DWORD)&newCreatureBalanceTable[ORIGINAL_CREATURES * CREATURE_BALANCE_SIZE];


	// --- Original Creature fixes ---
	// Fix Chicken help texts.
	newCreatureInfoTable[17 * CREATURE_INFO_SIZE + 20] = 8850;
	newCreatureInfoTable[17 * CREATURE_INFO_SIZE + 21] = 8851;

	// Fix Crocodile help texts.
	newCreatureInfoTable[18 * CREATURE_INFO_SIZE + 20] = 8852;
	newCreatureInfoTable[18 * CREATURE_INFO_SIZE + 21] = 8853;

	// Fix Polar Bear to use correct totem mesh.
	newCreatureInfoTable[10 * CREATURE_INFO_SIZE + 210] = 268;


	/*int infoBase = 13 * CREATURE_INFO_SIZE;
	int balanceBase = 13 * CREATURE_BALANCE_SIZE;
	memcpy(&newCreatureInfoTable[infoBase], &newCreatureInfoTable[0], CREATURE_INFO_SIZE * 4);
	memcpy(&newCreatureBalanceTable[balanceBase], &newCreatureBalanceTable[0], CREATURE_BALANCE_SIZE * 4);
	newCreatureInfoTable[infoBase + 73] = 19;			// CreatureType
	newCreatureInfoTable[infoBase + 126] = 19;			// ENUM_CREATURE_TYPE
	*/


	// --- New Creatures. ---
	if (1 == 2) // Let's disable extra Creatures (for now).
	{
		for (int n = 0; n < NewCreatureEntry::count; n++)
		{
			NewCreatureEntry& e = NewCreatureEntry::newCreatureEntries[n];
			int i = ORIGINAL_CREATURES + n;
			int infoBase = i * CREATURE_INFO_SIZE;
			int balanceBase = i * CREATURE_BALANCE_SIZE;
			int cbnBase = i * 2;
			int srcInfo = e.baseSlot * CREATURE_INFO_SIZE;
			int srcBalance = e.baseSlot * CREATURE_BALANCE_SIZE;

			memcpy(&newCreatureInfoTable[infoBase], &newCreatureInfoTable[srcInfo], CREATURE_INFO_SIZE * 4);
			memcpy(&newCreatureBalanceTable[balanceBase], &newCreatureBalanceTable[srcBalance], CREATURE_BALANCE_SIZE * 4);

			newCreatures[cbnBase] = (DWORD)e.bname;
			newCreatures[cbnBase + 1] = (DWORD)e.name;

			newCreatureInfoTable[infoBase + 73] = i;			// CreatureType
			newCreatureInfoTable[infoBase + 126] = i;			// ENUM_CREATURE_TYPE
			newCreatureInfoTable[infoBase + 20] = e.helpStart;
			newCreatureInfoTable[infoBase + 21] = e.helpEnd;
			if (e.totemMesh != -1)
				newCreatureInfoTable[infoBase + 210] = e.totemMesh;
			newCreatureBalanceTable[balanceBase + 3] = 32000 - i;
		}
	}

	return result;
}


// Fix Pathing
signed int __fastcall MobileWallHug__MoveToCircleHugLinearSquareSweep_Detour(DWORD* _this, int edx, int a2)
{
	// Get Villager Info.
	Living* Villager = (Living*)_this;
	float Y_Cord = LH3DIsland__GetAltitude(&Villager->MapCord);

	// Locations where wallhug occurs.
	BYTE* PathFix1 = (BYTE*)(0x006257B8);
	BYTE* PathFix2 = (BYTE*)(0x006257C9);
	signed int result = 0;

	// If Altitude is above water, skip wallhug.
	if (Y_Cord > 1)
	{
		// Apply patch to skip "jmp" wall hug.
		PathFix1[0] = 0x90;
		PathFix1[1] = 0x90;

		PathFix2[0] = 0x90;
		PathFix2[1] = 0x90;

		result = MobileWallHug__MoveToCircleHugLinearSquareSweep(_this, a2);

		// Restore default pathing.
		PathFix1[0] = 0x74;
		PathFix1[1] = 0x1B;

		PathFix2[0] = 0x74;
		PathFix2[1] = 0x0A;

	}
	else
	{
		result = MobileWallHug__MoveToCircleHugLinearSquareSweep(_this, a2);
	}

	return result;
}

/*
Experimental bug fix
When map gets large, buildings can occasionally get a corrupt villager linked list. Add extra code to validate the list before saving. I think this is enough?
*/

int(__cdecl* SpecialVillager_Create)(int a1, int a2, int a3, int a4, int a5) = (int(__cdecl*)(int, int, int, int, int))(0x0073A7C0);
int __cdecl SpecialVillager_Create_Detour(int a1, int a2, int a3, int a4, int a5)
{
	printf("[BWFix] SpecialVillager::Create blocked\n");
	return 0;
}

bool IsValidGameObject(DWORD ptr)
{
	if (!ptr) return false;
	__try
	{
		// vftables are static in memory. If Villager is invalid, it will have a very large vftable. 
		DWORD vftable = *(DWORD*)ptr;
		if (vftable < 0x00400000 || vftable > 0x00FFFFFF)
			return false;
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool IsValidPtr(DWORD ptr)
{
	__try
	{
		volatile DWORD test = *(DWORD*)ptr;
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}


void RebuildBuildingSiteList(unsigned __int8* _this, int listOffset, int countOffset)
{
	if (!IsValidPtr((DWORD)(_this + listOffset))) return;
	if (!IsValidPtr((DWORD)(_this + countOffset))) return;

	// Rebuild the linked list of Villagers.
	DWORD* head = (DWORD*)(_this + listOffset);
	DWORD* count = (DWORD*)(_this + countOffset);
	DWORD node = *head;

	DWORD newHead = 0;
	DWORD* newTail = &newHead;
	int newCount = 0;

	while (node)
	{
		__try
		{
			if (!IsValidPtr(node))
				break;

			DWORD* nodePtr = (DWORD*)node;
			DWORD next = nodePtr[0];
			DWORD objPtr = nodePtr[1];

			if (IsValidGameObject(objPtr))
			{
				*newTail = node;
				newTail = (DWORD*)node;
				nodePtr[0] = 0;
				newCount++;
			}
			else
			{
				printf("[BuildingSite::Save] Rebuilding linked list of Villagers. Dropping invalid obj=0x%08X offset=%d\n", objPtr, listOffset);
			}

			node = next;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			break;
		}
	}

	*head = newHead;
	*count = newCount;
}
int(__thiscall* BuildingSite_Save)(unsigned __int8* _this, DWORD* a2) = (int(__thiscall*)(unsigned __int8*, DWORD*))(0x00442AB0);
int __fastcall BuildingSite_Save_Detour(unsigned __int8* _this, int edx, DWORD* a2)
{
	// Check the linked lists in the building object.
	RebuildBuildingSiteList(_this, 24, 28);
	RebuildBuildingSiteList(_this, 32, 36);
	RebuildBuildingSiteList(_this, 40, 44);

	// Do the save.
	return BuildingSite_Save(_this, a2);
}



void* (__thiscall* Morphable__GetAnim)(float* _this, float slot, int a3) = (void* (__thiscall*)(float*, float, int))0x00632590;

void* __fastcall Morphable__GetAnim_Detour(float* _this, int edx, float slot, int a3)
{
	printf("[GetAnim] slot=%d  a3=%d  this=0x%08X\n", (int)slot, a3, (DWORD)_this);
	fflush(stdout);
	return Morphable__GetAnim(_this, slot, a3);
}




// Stub this call and always return false. It is only used to check if CreatureCurse is active and it's buggy as fuck.
BOOL(__stdcall* GScript__IsScriptRunning)(int a1) = (BOOL(__stdcall*)(int))(0x00700740);
BOOL __stdcall GScript__IsScriptRunning_Detour(int a1)
{
	return false;
}

// CBN
#pragma once

enum CreatureAnim
{
	// move
	ANIM_MOVE_CSTAND = 0,
	ANIM_MOVE_WWALK = 1,
	ANIM_MOVE_WRUN = 2,
	ANIM_MOVE_NR_SPIN_0 = 3,
	ANIM_MOVE_NR_SPIN_90 = 4,
	ANIM_MOVE_NR_SPIN_180 = 5,
	ANIM_MOVE_NL_SPIN_0 = 6,
	ANIM_MOVE_NL_SPIN_90 = 7,
	ANIM_MOVE_NL_SPIN_180 = 8,
	ANIM_MOVE_NR_JUMP_L = 9,
	ANIM_MOVE_NR_STEP_0 = 10,
	ANIM_MOVE_NR_STEP_90 = 11,
	ANIM_MOVE_NR_STEP_180 = 12,
	ANIM_MOVE_NL_STEP_0 = 13,
	ANIM_MOVE_NL_STEP_90 = 14,
	ANIM_MOVE_NL_STEP_180 = 15,

	// face
	ANIM_FACE_NSMILE = 16,
	ANIM_FACE_NGRIMACE = 17,
	ANIM_FACE_NGROWL = 18,
	ANIM_FACE_NSCARED = 19,
	ANIM_FACE_NSAD = 20,
	ANIM_FACE_NAMAZED = 21,
	ANIM_FACE_NPUZZLED = 22,
	ANIM_FACE_NLAUGH = 23,
	ANIM_FACE_NOOH = 24,
	ANIM_FACE_NAAH = 25,
	ANIM_FACE_NSPARE1 = 26,
	ANIM_FACE_NSPARE2 = 27,

	// static
	ANIM_STATIC_NSTART_SLEEP = 28,
	ANIM_STATIC_CSLEEP = 29,
	ANIM_STATIC_NEND_SLEEP = 30,
	ANIM_STATIC_NSTART_POO = 31,
	ANIM_STATIC_CPOO = 32,
	ANIM_STATIC_NEND_POO = 33,
	ANIM_STATIC_NSTART_PUKE = 34,
	ANIM_STATIC_CPUKE = 35,
	ANIM_STATIC_NEND_PUKE = 36,
	ANIM_STATIC_NSTART_SIT = 37,
	ANIM_STATIC_CSIT = 38,
	ANIM_STATIC_NEND_SIT = 39,
	ANIM_STATIC_NSTART_CAST = 40,
	ANIM_STATIC_CCAST = 41,
	ANIM_STATIC_NEND_CAST = 42,
	ANIM_STATIC_NSTART_CAST_UP = 43,
	ANIM_STATIC_CCAST_UP = 44,
	ANIM_STATIC_NEND_CAST_UP = 45,
	ANIM_STATIC_NSTART_SCATTER = 46,
	ANIM_STATIC_CSCATTER = 47,
	ANIM_STATIC_NEND_SCATTER = 48,
	ANIM_STATIC_NSTART_SPARE = 49,
	ANIM_STATIC_CSPARE = 50,
	ANIM_STATIC_NEND_SPARE = 51,

	// individual
	ANIM_INDIVIDUAL_CSUMMON = 52,
	ANIM_INDIVIDUAL_CANGRY = 53,
	ANIM_INDIVIDUAL_CHUNGRY = 54,
	ANIM_INDIVIDUAL_CHAPPY = 55,
	ANIM_INDIVIDUAL_CSAD = 56,
	ANIM_INDIVIDUAL_CTIRED = 57,
	ANIM_INDIVIDUAL_CHOT = 58,
	ANIM_INDIVIDUAL_CCOLD = 59,
	ANIM_INDIVIDUAL_CSCRATCH = 60,
	ANIM_INDIVIDUAL_CFRIGHTENED = 61,
	ANIM_INDIVIDUAL_CSNEEZE = 62,
	ANIM_INDIVIDUAL_CCONFUSED = 63,
	ANIM_INDIVIDUAL_CFEELING_NICE = 64,
	ANIM_INDIVIDUAL_CIMPRESS = 65,
	ANIM_INDIVIDUAL_CNEED_A_POO = 66,
	ANIM_INDIVIDUAL_CFEEL_PLAYFUL = 67,
	ANIM_INDIVIDUAL_CPLAY_ACTION = 68,
	ANIM_INDIVIDUAL_CLOOK_AT_ME = 69,
	ANIM_INDIVIDUAL_CTAUNT = 70,
	ANIM_INDIVIDUAL_CDRINK = 71,
	ANIM_INDIVIDUAL_CFRIENDLY_WAVE = 72,
	ANIM_INDIVIDUAL_CEMBARRASSED = 73,
	ANIM_INDIVIDUAL_CPICK_ME = 74,

	// look
	ANIM_LOOK_LRIGHT_LEFT = 75,
	ANIM_LOOK_LDOWN_UP = 76,
	ANIM_LOOK_LSIT_R_L = 77,
	ANIM_LOOK_LSIT_D_U = 78,
	ANIM_LOOK_LHATE_LOVE = 79,
	ANIM_LOOK_NSEEK_ARM = 80,

	// pickup
	ANIM_PICKUP_NFRONT_RIGHT = 81,
	ANIM_PICKUP_NFRONT_LEFT = 82,
	ANIM_PICKUP_NBACK_RIGHT = 83,
	ANIM_PICKUP_NBACK_LEFT = 84,
	ANIM_PICKUP_NHOLD = 85,
	ANIM_PICKUP_NFROM_HAND = 86,
	ANIM_PICKUP_NSPARE0 = 87,
	ANIM_PICKUP_NSPARE1 = 88,
	ANIM_PICKUP_NSPARE2 = 89,
	ANIM_PICKUP_NSPARE3 = 90,

	// throw
	ANIM_THROW_NHURL_FLAT = 91,
	ANIM_THROW_NHURL_HIGH = 92,
	ANIM_THROW_NBOWL = 93,
	// slot 94 = -spare2

	// object_remove
	ANIM_OBJECT_REMOVE_NDISCARD = 95,
	ANIM_OBJECT_REMOVE_NEAT = 96,
	ANIM_OBJECT_REMOVE_NPUT_DOWN = 97,
	ANIM_OBJECT_REMOVE_NGENTLE_LOB = 98,
	ANIM_OBJECT_REMOVE_NSPARE = 99,

	// object_keep
	ANIM_OBJECT_KEEP_CSTROKE = 100,
	ANIM_OBJECT_KEEP_CSHAKE = 101,
	ANIM_OBJECT_KEEP_CSMELL = 102,
	ANIM_OBJECT_KEEP_CEXAMINE = 103,
	ANIM_OBJECT_KEEP_CSPARE1 = 104,
	ANIM_OBJECT_KEEP_CSPARE2 = 105,

	// fight_extra
	ANIM_FIGHT_EXTRA_NFAINT = 106,
	ANIM_FIGHT_EXTRA_NGET_UP = 107,
	ANIM_FIGHT_EXTRA_CPOWER_UP = 108,
	ANIM_FIGHT_EXTRA_NCREATION = 109,
	ANIM_FIGHT_EXTRA_NSTART_CAST = 110,
	ANIM_FIGHT_EXTRA_CCAST = 111,
	ANIM_FIGHT_EXTRA_NEND_CAST = 112,

	// destroy
	ANIM_DESTROY_CFRONT_RIGHT = 113,
	ANIM_DESTROY_CFRONT_LEFT = 114,
	ANIM_DESTROY_CBACK_RIGHT = 115,
	ANIM_DESTROY_CBACK_LEFT = 116,
	ANIM_DESTROY_CKICK_LOW = 117,
	// slot 118 = -spare0

	// extra_stands
	ANIM_EXTRA_STANDS_CRELAXED = 119,
	ANIM_EXTRA_STANDS_CSPARE1 = 120,
	ANIM_EXTRA_STANDS_CSPARE2 = 121,

	// fight
	ANIM_FIGHT_NSTART = 122,
	ANIM_FIGHT_CSTANCE = 123,
	ANIM_FIGHT_NFINISH = 124,
	ANIM_FIGHT_CATTACK_HIGH = 125,
	ANIM_FIGHT_CATTACK_EYE = 126,
	ANIM_FIGHT_CATTACK_BELLY = 127,
	ANIM_FIGHT_CATTACK_PELVIS = 128,
	ANIM_FIGHT_CATTACK_KNEE = 129,
	ANIM_FIGHT_CATTACK_SPECIAL = 130,
	ANIM_FIGHT_CATTACK_HIGH2 = 131,
	ANIM_FIGHT_CATTACK_EYE2 = 132,
	ANIM_FIGHT_CATTACK_BELLY2 = 133,
	ANIM_FIGHT_CATTACK_PELVIS2 = 134,
	ANIM_FIGHT_CATTACK_KNEE2 = 135,
	ANIM_FIGHT_CATTACK_SPECIAL2 = 136,
	ANIM_FIGHT_CSTEP_FORWARD = 137,
	ANIM_FIGHT_CSTEP_BACK = 138,
	ANIM_FIGHT_CSTEP_RIGHT = 139,
	ANIM_FIGHT_CSTEP_LEFT = 140,
	ANIM_FIGHT_CSPARE3 = 141,
	ANIM_FIGHT_NSTART_BLOCK = 142,
	ANIM_FIGHT_CBLOCK = 143,
	ANIM_FIGHT_NEND_BLOCK = 144,

	// dance
	ANIM_DANCE_NSTART = 145,
	ANIM_DANCE_NFINISH = 146,
	ANIM_DANCE_CA = 147,
	ANIM_DANCE_CB = 148,
	ANIM_DANCE_CC = 149,
	ANIM_DANCE_CD = 150,
	ANIM_DANCE_CE = 151,
	ANIM_DANCE_CSPARE1 = 152,
	ANIM_DANCE_CSPARE2 = 153,

	// reward
	ANIM_REWARD_CHEAD = 154,
	ANIM_REWARD_CARMPIT_R = 155,
	ANIM_REWARD_DNO_1 = 156,
	ANIM_REWARD_CBELLY = 157,
	ANIM_REWARD_CGROIN = 158,
	ANIM_REWARD_CFOOT_R = 159,
	ANIM_REWARD_DNO_2 = 160,
	ANIM_REWARD_CHAND_R = 161,
	ANIM_REWARD_DNO_3 = 162,
	ANIM_REWARD_CSPARE1 = 163,
	ANIM_REWARD_CSPARE2 = 164,

	// punish
	ANIM_PUNISH_CHEAD_SLAP_R = 165,
	ANIM_PUNISH_DNO_1 = 166,
	ANIM_PUNISH_CWAIST_SLAP_R = 167,
	ANIM_PUNISH_DNO_2 = 168,
	ANIM_PUNISH_CFEET_SLAP_R = 169,
	ANIM_PUNISH_DNO_3 = 170,
	ANIM_PUNISH_CHEAD_GENTLE_R = 171,
	ANIM_PUNISH_DNO_4 = 172,
	ANIM_PUNISH_CWAIST_GENTLE_R = 173,
	ANIM_PUNISH_DNO_5 = 174,
	ANIM_PUNISH_CFEET_GENTLE_R = 175,
	ANIM_PUNISH_DNO_6 = 176,
	ANIM_PUNISH_CSPARE1 = 177,
	ANIM_PUNISH_CSPARE2 = 178,

	// recoil
	ANIM_RECOIL_CHI = 179,
	ANIM_RECOIL_CHI_R = 180,
	ANIM_RECOIL_CHI_L = 181,
	ANIM_RECOIL_CHI_T = 182,
	ANIM_RECOIL_CHI_B = 183,
	ANIM_RECOIL_CMI = 184,
	ANIM_RECOIL_CMI_R = 185,
	ANIM_RECOIL_CMI_L = 186,
	ANIM_RECOIL_CMI_T = 187,
	ANIM_RECOIL_CMI_B = 188,
	ANIM_RECOIL_CLO = 189,
	ANIM_RECOIL_CLO_R = 190,
	ANIM_RECOIL_CLO_L = 191,
	ANIM_RECOIL_CLO_T = 192,
	ANIM_RECOIL_CLO_B = 193,
	ANIM_RECOIL_NHI_R_L = 194,
	ANIM_RECOIL_NHI_F_B = 195,
	ANIM_RECOIL_NLO_R_L = 196,
	ANIM_RECOIL_NLO_F_B = 197,
	ANIM_RECOIL_CBLOCK = 198,
	ANIM_RECOIL_CSPARE = 199,

	// concurrent
	ANIM_CONCURRENT_CNOD = 200,
	ANIM_CONCURRENT_CSHAKE = 201,
	ANIM_CONCURRENT_CYAWN = 202,
	ANIM_CONCURRENT_CTHIRSTY = 203,
	ANIM_CONCURRENT_CSQUIRT_WATER = 204,
	ANIM_CONCURRENT_CTALK = 205,
	ANIM_CONCURRENT_CSPARE1 = 206,
	ANIM_CONCURRENT_CSPARE2 = 207,

	// point
	ANIM_POINT_CLO_LEFT = 208,
	ANIM_POINT_CLO_RIGHT = 209,
	ANIM_POINT_CHI_LEFT = 210,
	ANIM_POINT_CHI_RIGHT = 211,

	// misc
	ANIM_MISC_CKISS_HI = 212,
	ANIM_MISC_CKISS_LO = 213,
	ANIM_MISC_NSCRIPT1 = 214,
	ANIM_MISC_NSCRIPT2 = 215,
	ANIM_MISC_NSCRIPT3 = 216,
	ANIM_MISC_CHOWL = 217,
	ANIM_MISC_CLAUGH = 218,
	ANIM_MISC_CCRY = 219,
	ANIM_MISC_CAHA = 220,
	ANIM_MISC_CPRAY = 221,
	ANIM_MISC_CLUCK_YOU = 222,
	ANIM_MISC_CSPARE1 = 223,
	ANIM_MISC_CSPARE2 = 224,

	// catch
	ANIM_CATCH_CSTEP_RIGHT = 225,
	ANIM_CATCH_CSTEP_BACK = 226,
	ANIM_CATCH_CSTEP_FRONT = 227,
	ANIM_CATCH_NHI_R = 228,
	ANIM_CATCH_NHI_L = 229,
	ANIM_CATCH_NLO_R = 230,
	ANIM_CATCH_NLO_L = 231,
};

int(__thiscall* LH3DCreature__LoadBinary)(char* _this, const char* filename, int a3) = (int(__thiscall*)(char*, const char*, int))0x004FC7E0;

#define SLOT_BASE   0x13F4
#define SLOT(n)     (SLOT_BASE + (n) * 4)
#define STATUS(s,m) (4 * ((s) + 696*(m)) + 1396)
#define CACHE(s,m)  (4 * ((s) + 696*(m)) + 468)
#define OGRE_ENDPTR 0x002A047C

static void CopySlotFull(char* _this, int dst, int src)
{
	*(DWORD*)(_this + SLOT(dst)) = *(DWORD*)(_this + SLOT(src));
	for (int m = 1; m <= 5; m++)
	{
		*(DWORD*)(_this + STATUS(dst, m)) = *(DWORD*)(_this + STATUS(src, m));
		*(DWORD*)(_this + CACHE(dst, m)) = *(DWORD*)(_this + CACHE(src, m));
	}
}


#pragma endregion // MultiplayerDetours
#pragma region CBNDetours



int __fastcall LH3DCreature__LoadBinary_Detour(char* _this, int edx, const char* filename, int a3)
{
	printf("[LH3DCreature::LoadBinary] _this=0x%08X filename=%s internal_name=%s ret=0x%08X\n",
		(DWORD)_this, filename, _this + 22472, (DWORD)_ReturnAddress());
	fflush(stdout);

	// Read from active .chl for female override flag.
	DWORD* dllBase = (DWORD*)GetModuleHandleA("ScriptLibraryR.DLL");
	DWORD* PTType1 = (DWORD*)(dllBase + (0x4493C / 4));
	FLOAT* FemaleCreatureOverride = (FLOAT*)(PTType1[0] + 352);

	// Check if .chl has defined an override.
	const char* originalFilename = filename;
	int chlOverride = FemaleCreatureOverride[0];
	if (chlOverride > 0)
	{
		g_overrideFemaleFlag = 0;
		printf("[LH3DCreature::LoadBinary] Found CHL Variant Override!: %d \n", chlOverride);

		if (chlOverride == 1)
		{
			filename = "bgreek1";
			printf("[LH3DCreature::LoadBinary] CHL Variant Override to: %s \n", filename);
		}
		else if(chlOverride == 2)
		{
			filename = "bgreek2";
			printf("[LH3DCreature::LoadBinary] CHL Variant Override to: %s \n", filename);
		}
		else if (chlOverride == 3)
		{
			filename = "bgreek3";
			printf("[LH3DCreature::LoadBinary] CHL Variant Override to: %s \n", filename);
		}
	}

	printf("[LH3DCreature::LoadBinary] Female/Variant: %d \n", g_overrideFemaleFlag);
	if (g_overrideFemaleFlag > 0)
	{
		if (_stricmp(filename, "bgreek") == 0)
		{
			if (g_overrideFemaleFlag == 1)
			{
				filename = "bgreek1";
				printf("[LH3DCreature::LoadBinary] Overriding to %s\n", filename);
			}
			else if (g_overrideFemaleFlag == 2)
			{
				filename = "bgreek2";
				printf("[LH3DCreature::LoadBinary] Overriding to %s\n", filename);
			}
			else if (g_overrideFemaleFlag == 3)
			{
				filename = "bgreek3";
				printf("[LH3DCreature::LoadBinary] Overriding to %s\n", filename);
			}
		}
	}

	int result = LH3DCreature__LoadBinary(_this, filename, a3);
	printf("[LoadBinary]\n");

	// Debug printing.
	if (_stricmp(originalFilename, "bape") == 0)
	{
		/*
		printf("[SlotDump] APE all slots for %s:\n", filename);
		for (int s = 0; s < 232; s++)
		{
			DWORD slotVal = *(DWORD*)(_this + SLOT(s));
			if (slotVal == 0)
				printf("[SlotDump] slot %3d = EMPTY\n", s);
			else
				printf("[SlotDump] slot %3d = 0x%08X\n", s, slotVal);
		}
		fflush(stdout);
		*/
	}

	if (result)
	{
		if (_stricmp(originalFilename, "bgreek") == 0)
		{
			printf("[LoadBinary] Patching Ogre...%s \n", filename);
			
			// --- Hand-Object Interaction Fixes ---
			// Slots 24/25/26 (Nstart_pickup/Ccarry/Nend_putdown) are missing from the Ogre CBN.
			// Slot 23 is unique to the Ogre (not present in Ape) and has a matching
			CopySlotFull(_this, 24, 23);  // Nstart_pickup <- Ogre slot 23 (reach animation)
			CopySlotFull(_this, 25, 23);  // Ccarry        <- Ogre slot 23
			CopySlotFull(_this, 26, 23);  // Nend_putdown  <- Ogre slot 23

			// --- Cast Animation Fixes ---
			// The Ogre CBN is missing  cast animation slots. Copied from fight/creation animations as placeholders.
			CopySlotFull(_this, 40, 110);  // Nend_sit    <- Ncreation (slot 110)
			CopySlotFull(_this, 41, 111);  // Nstart_cast <- fight Nstart_cast (slot 111)
			CopySlotFull(_this, 42, 112);  // Ccast       <- fight Ccast (slot 112)
			CopySlotFull(_this, 43, 110);  // Nend_cast   <- Ncreation (slot 110)

			// Creature Building animation (up cast).
			CopySlotFull(_this, 44, 111);  // Nstart_cast <- fight Nstart_cast (slot 111)
			CopySlotFull(_this, 45, 111);  // Nstart_cast <- fight Nstart_cast (slot 111)

			// Cast scatter (food, water etc.).
			CopySlotFull(_this, 46, 110);  // Nstart_scatter <- Ncreation (slot 110)
			CopySlotFull(_this, 47, 111);  // Cscatter    <- fight Nstart_cast (slot 111)
			CopySlotFull(_this, 48, 112);  // Nend_scatter <- fight Ccast (slot 112)

			// Various common animations, especially when impressing. Without this, the Ogre can sometimes get "stuck" idle when just doing random things, especially when impressing.
			CopySlotFull(_this, 61, 52);  // Cfrightened   <- Cscratch
			CopySlotFull(_this, 64, 52);  // Cfeeling_nice <- Cconfused
			CopySlotFull(_this, 65, 52);  // Cimpress      <- Cconfused
			CopySlotFull(_this, 66, 52);  // Cneed_a_poo   <- Cconfused
			CopySlotFull(_this, 67, 52);  // Cfeel_playful <- Ctaunt
			CopySlotFull(_this, 68, 52);  // Cplay_action  <- Ctaunt
			CopySlotFull(_this, 69, 52);  // Clook_at_me   <- Ctaunt
			CopySlotFull(_this, 71, 52);  // Cdrink        <- Ctaunt
			CopySlotFull(_this, 72, 52);  // Cfriendly_wave <- Cembarrassed
			CopySlotFull(_this, 74, 52);  // Cpick_me      <- Cembarrassed

			// --- Slot 86 Stub Fix ---
			// Exists in the Ogre CBN but has only 1 keyframe (66 ticks).
			// This causes an integer divide by zero in sub_87FB30 (animation blender) when the hand tries to give the Ogre an object. Slot 85 is adjacent and has 266 frames.
			CopySlotFull(_this, 86, 85);  // Nfrom_hand  <- Nhold

			// --- Discard Animation Fix ---
			// When the Ogre is handed an object while already holding one. The Ogre's native slot 95 animation gets the object stuck in his hand (release callback never fires).
			// However, BW Ogre does not have this issue, we have actually patched the .cbn to fix it in other Ogres.
			//CopySlotFull(_this, 95, 52);  // Ndiscard <- slot 52 (summon animation, releases object)


			// Take kick animation from fighting.
			CopySlotFull(_this, 117, 129);  // Ckick_low <- Cattack_knee


			// Fix grab thing for BW Ogre, not sure if this is needed?
			CopySlotFull(_this, 225, 112); // Cstep_right  <- Nend_cast 

		}
	}

	return result;
}

// Ogre fix. BW has weird blending for evil Creatures, this forces Ogre to just use the default/neutral spell casts. Otherwise, evil ogres just stands still when casting.
void(__thiscall* LH3DCreature__StateSwitch)(int _this) = (void(__thiscall*)(int))0x004906D0;
void __fastcall LH3DCreature__StateSwitch_Detour(int _this, int edx)
{
	int state = *(int*)(_this + 18836);

	if (state == 2 || state == 3 || state == 4)
	{
		const char* name = (const char*)(_this + 22440);
		if (_strnicmp(name, "bgreek", 6) == 0)
			*(float*)(_this + 160) = 1.0f;
	}

	LH3DCreature__StateSwitch(_this);
}


#pragma endregion // CBNDetours

#pragma region SettingsDetours
// ----- Disable Creature Building -----
int(__thiscall* Creature__SubStatePerformCastPlannedAbode)(DWORD* _this) = (int(__thiscall*)(DWORD*))(0x00513430);
int __fastcall Creature__SubStatePerformCastPlannedAbode_Detour(DWORD* _this)
{
	printf("[Creature::SubStatePerformCastPlannedAbode]\n");
	if (!GetRegInt("CreatureBuilding") && !IsMultiplayerGame())
	{
		printf("[Creature::SubStatePerformCastPlannedAbode] CreatureBuilding Prevented! =%d\n", GetRegInt("CreatureBuilding"));
		return 0;
	}
	return Creature__SubStatePerformCastPlannedAbode(_this);
}

int(__thiscall* Creature__SubStatePerformBuild)(DWORD* _this) = (int(__thiscall*)(DWORD*))(0x0051B950);
int __fastcall Creature__SubStatePerformBuild_Detour(DWORD* _this)
{
	printf("[Creature::SubStatePerformBuild]\n");
	if (!GetRegInt("CreatureBuilding") && !IsMultiplayerGame())
	{
		printf("[Creature::SubStatePerformBuild] CreatureBuilding Prevented! =%d\n", GetRegInt("CreatureBuilding"));
		return 2;
	}
	return Creature__SubStatePerformBuild(_this);
}

int(__thiscall* Creature__SubStatePerformCreatePlannedCivicBuilding)(DWORD* _this) = (int(__thiscall*)(DWORD*))(0x0051BC40);
int __fastcall Creature__SubStatePerformCreatePlannedCivicBuilding_Detour(DWORD* _this)
{
	printf("[Creature::SubStatePerformCreatePlannedCivicBuilding]\n");
	if (!GetRegInt("CreatureBuilding") && !IsMultiplayerGame())
	{
		printf("[Creature::SubStatePerformCreatePlannedCivicBuilding] CreatureBuilding Prevented! =%d\n", GetRegInt("CreatureBuilding"));
		return 1;
	}
	return Creature__SubStatePerformCreatePlannedCivicBuilding(_this);
}

int(__thiscall* Creature__SubStatePerformCreatePlannedAbode)(DWORD* _this) = (int(__thiscall*)(DWORD*))(0x0051BBA0);
int __fastcall Creature__SubStatePerformCreatePlannedAbode_Detour(DWORD* _this)
{
	if (!GetRegInt("CreatureBuilding") && !IsMultiplayerGame())
	{
		printf("[Creature::SubStatePerformCreatePlannedAbode_Detour] CreatureBuilding Prevented! =%d\n", GetRegInt("CreatureBuilding"));
		return 1;
	}
	return Creature__SubStatePerformCreatePlannedAbode(_this);
}

#pragma endregion // SettingsDetours
#pragma region VillagerDetours
// ----- Special Eternal Villager -----
// VILLAGER_INFO mesh values for special villagers (>= 63)
static const std::unordered_map<int, const char*> g_SpecialVillagerMeshes = {
	{ 587, "PIED_PIPER" },
	{ 590, "SHAOLIN" },
	{ 564, "IDOL_BUILDER" },
	{ 562, "HERMIT" },
	{ 563, "HIPPY" },
	{ 535, "PRIEST" },
	{ 536, "PRIESTESS" },
	{ 514, "MARAUDER" },
	{ 527, "FOOTBALLER_1" },
	{ 579, "FOOTBALLER_2" },
	{ 543, "ENGINEER" },
	{ 591, "SHEPHERD" },
	{ 578, "NOMAD" },
	{ 506, "CREATURE_TRAINER" },
	{ 586, "NORSE_SAILOR" },
	{ 505, "BREEDER" },
	{ 498, "HEALER" },
	{ 589, "SCULPTOR" },
	{ 534, "CRUSADER" },
	{ 588, "SAILOR_ACCORDIAN" },
	{  84, "MULE_HEAD" },
	{  83, "EGYPTIAN_BIG_EYES" },
	{ 566, "GIANT" },
};
int(__thiscall* Villager__CheckDeathFromOldAge)(DWORD* _this) = (int(__thiscall*)(DWORD*))(0x0077D350);
int __fastcall Villager__CheckDeathFromOldAge_Detour(DWORD* _this, void* edx)
{
	// If not Multiplayer and user enabled Eternal Special Villagers, make them unable to die from old age.
	DWORD* g_GGame = (DWORD*)(*(DWORD*)0x0D7B614);
	if (!GGame__IsMultiplayerGame(g_GGame))
	{
		if (GetRegInt("EternalSpecialVillagers"))
		{
			int mesh = *(DWORD*)(*(DWORD*)((char*)_this + 40) + 536);
			auto it = g_SpecialVillagerMeshes.find(mesh);
			if (it != g_SpecialVillagerMeshes.end())
			{
				printf("Villager::CheckDeathFromOldAge - skipping special villager: %s (mesh: %d)\n", it->second, mesh);
				return 0;
			}
		}
	}
	return Villager__CheckDeathFromOldAge(_this);
}

// Function to ensure Player has default values for customisation options.
void InitialiseDefaultRegistryValues()
{
	HKEY hKey;
	DWORD* dllBase = (DWORD*)GetModuleHandleA("LHMultiplayerR.DLL");
	char* profilId = (char*)(dllBase + (0x0006B288 / 4));
	char regFolder[300];
	sprintf(regFolder, "Software\\Lionhead Studios Ltd\\Black & White\\LHMultiplayer\\ProfilesCI\\%s", profilId);

	if (RegOpenKeyEx(HKEY_CURRENT_USER, regFolder, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		RegCreateKeyEx(HKEY_CURRENT_USER, regFolder, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	auto SetDefault = [&](const char* name, DWORD defaultValue)
		{
			DWORD value, len = sizeof(DWORD), type = REG_DWORD;
			if (RegQueryValueEx(hKey, name, NULL, &type, (LPBYTE)&value, &len) != ERROR_SUCCESS)
			{
				RegSetValueEx(hKey, name, 0, REG_DWORD, (const BYTE*)&defaultValue, sizeof(DWORD));
				setReg((char*)name, defaultValue);
				printf("[Registry] Set default %s=0x%08X\n", name, defaultValue);
				fflush(stdout);
			}
		};

	SetDefault("PlayerColour", 0x00FF4646);
	SetDefault("PlayerTemple", 0);
	SetDefault("CreatureVariant", 0);
	SetDefault("TykeCreatureVariant", 0);
	SetDefault("UnlimitedDrawDistance", 1);
	SetDefault("EternalSpecialVillagers", 0);
	SetDefault("CreatureBuilding", 1);

	RegCloseKey(hKey);
}

void LoadRegistyValues()
{
	static int g_EternalVillagers = int(getReg((char*)"EternalSpecialVillagers"));
	static int g_DrawDist = int(getReg((char*)"UnlimitedDrawDistance"));
	static int g_CreatureBuilding = int(getReg((char*)"CreatureBuilding"));
	printf("Eternal: %d\n\n\n\n", g_EternalVillagers);
	printf("Eternal Draw: %d\n\n\n\n", g_DrawDist);
	getRegString((char*)"file");
}

void __fastcall GGame__StartGame_Detour(Game* _this, int edx)
{
	//Print stuff.
	InitialiseDefaultRegistryValues();
	g_creatureVariantMap.clear();
	printf("Game Started! %d\n");




	//g_creatureVariantMap.clear();

	//Start Game
	LoadRegistyValues();
	GGame__StartGame(_this);
}

// Swap Creature
void(__thiscall* Creature__ForceActivityAndForceAction)(void* _this, void* plan, int a3, int a4) =
(void(__thiscall*)(void* _this, void* plan, int a3, int a4))(0x004D36F0);

static void __fastcall Creature__ForceActivityAndForceAction_Detour(void* _this, void* /*edx*/, void* plan, int a3, int a4)
{
	void* retAddr = _ReturnAddress();
	if ((DWORD)retAddr >= 0x00486C30 && (DWORD)retAddr <= 0x004873C3)
		return; // skip when called from SwapMinds

	Creature__ForceActivityAndForceAction(_this, plan, a3, a4);
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


#define DETOUR(NAME) if (DetourAttach((PVOID*)(&NAME), (PVOID)NAME ## _Detour)) { printf("Failed to detour " #NAME); }


// Utils
enum Anchor {
	TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT
};

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define streq(STRING1, STRING2) (strcmp(STRING1, STRING2) == 0)


//bool isNumber(const std::string& str);


bool strin(const char* str, ...);


int splitArgs(char* str, char sep, char** dst, int limit);

int getArgIndex(char** argv, int argc, const char* arg);

char* getArgVal(char** argv, int argc, const char* arg);
char* getArgValOrDefault(char** argv, int argc, const char* arg, const char* def);


// Multiplayer
void setBindAddressOverride(const char* ip);
void setHostnameOverride(const char* name);
bool InjectHostOverride();


// Util

#include <filesystem>
#include <fstream>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

struct HandleData {
	DWORD pid;
	HWND window;
	HWND excluding;
};


bool strin(const char* str, ...) {
	va_list args;
	va_start(args, str);
	int count = va_arg(args, int);
	bool res = false;
	for (int i = 0; i < count; ++i) {
		if (streq(str, (char*)va_arg(args, int))) {
			res = true;
			break;
		}
	}
	va_end(args);
	return res;
}


int splitArgs(char* str, char sep, char** dst, int limit) {
	int n = 0, i = 0;
	bool quote = false;
	bool escape = false;
	dst[n++] = str;
	if (n >= limit) return n;
	bool isQuote = false;
	while (str[i] != 0) {
		if (str[i] == '"') {
			str[i] = 0;         //Remove the quotes from the result
			quote = !quote;
			if (quote) {
				dst[n - 1]++;   //Skip the opening quote
				isQuote = true;
			}
		}
		else if (str[i] == sep && !quote) {
			str[i] = 0;
			if (dst[n - 1][0] == 0 && !isQuote) {
				n--;            //Skip empty args, unless they are empty quotes
			}
			dst[n++] = &str[i + 1];
			if (n >= limit) return n;
			isQuote = false;
		}
		i++;
	}
	return n;
}

int getArgIndex(char** argv, int argc, const char* arg) {
	for (int i = 0; i < argc; i++) {
		if (streq(argv[i], arg)) {
			return i;
		}
	}
	return -1;
}


char* getArgVal(char** argv, int argc, const char* arg) {
	return getArgValOrDefault(argv, argc, arg, NULL);
}

char* getArgValOrDefault(char** argv, int argc, const char* arg, const char* def) {
	int index = getArgIndex(argv, argc, arg);
	if (index < 0 || index >= argc - 1) return (char*)def;
	return argv[index + 1];
}



#pragma endregion // VillagerDetours
#pragma region NetworkOverrides
char ipOverride[16] = "0.0.0.0";
char hostnameOverride[64] = "Foobar";

char* h_aliases[] = { 0 };
char* h_addr_list[] = { ipOverride, 0 };

struct hostent hostentOverride;

in_addr lastLANTarget = { 0 };


int(__stdcall* p_bind)(SOCKET s, const struct sockaddr* name, int namelen);
struct hostent* (__stdcall* p_gethostbyname)(const char* name);
int(__stdcall* p_gethostname)(char* name, int namelen);
int(__stdcall* p_connect)(SOCKET s, struct sockaddr* name, int namelen);
int(__stdcall* p_listen)(SOCKET s, int backlog);
SOCKET(__stdcall* p_accept)(SOCKET s, struct sockaddr* addr, int* addrlen);


void setBindAddressOverride(const char* ip) {
	strncpy(ipOverride, ip, sizeof(ipOverride));
}

void setHostnameOverride(const char* hostname) {
	strncpy(hostnameOverride, hostname, sizeof(hostnameOverride));
}

int __stdcall bind_override(SOCKET s, const struct sockaddr* name, int namelen) {
	sockaddr_in* addr = (struct sockaddr_in*)name;
	addr->sin_addr.S_un.S_addr = inet_addr(ipOverride);
	printf("bind address override: %s\n", ipOverride);
	return p_bind(s, (const struct sockaddr*)addr, namelen);
}

struct hostent* __stdcall gethostbyname_override(const char* name) {
	if (strcmp(name, hostnameOverride) == 0) {
		hostentOverride.h_name = hostnameOverride;
		hostentOverride.h_aliases = h_aliases;
		hostentOverride.h_addrtype = AF_INET;
		hostentOverride.h_length = 16;
		hostentOverride.h_addr_list = h_addr_list;
		printf("gethostbyname(%s) override: %s %s\n", name, hostentOverride.h_name, hostentOverride.h_addr_list[0]);
		return &hostentOverride;
	}
	else {
		struct hostent* he = p_gethostbyname(name);
		printf("gethostbyname(%s): %s %s\n", name, he->h_name, he->h_addr_list[0]);
		return he;
	}
}

int __stdcall gethostname_override(char* name, int namelen) {
	strncpy(name, hostnameOverride, namelen);
	printf("gethostname override: %s\n", hostnameOverride);
	return 0;
}

int __stdcall connect_override(SOCKET s, struct sockaddr_in* name, int namelen) {
	if (name->sin_addr.S_un.S_un_b.s_b1 != 192 && lastLANTarget.S_un.S_addr != 0) {
		char buf1[16];
		char buf2[16];
		strncpy(buf1, inet_ntoa(name->sin_addr), 16);
		strncpy(buf2, inet_ntoa(lastLANTarget), 16);
		printf("Overriding target address %s with %s...\n", buf1, buf2);
		name->sin_addr = lastLANTarget;
	}
	if (name->sin_addr.S_un.S_un_b.s_b1 == 192) {
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_addr.S_un.S_addr = inet_addr(ipOverride);
		sa.sin_port = 0;
		memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
		p_bind(s, (sockaddr*)&sa, sizeof(sa));
		lastLANTarget = name->sin_addr;
		printf("Connecting to %s:%d using %s...\n", inet_ntoa(name->sin_addr), htons(name->sin_port), ipOverride);
	}
	else {
		printf("Connecting to %s:%d using default interface...\n", inet_ntoa(name->sin_addr), htons(name->sin_port));
	}
	return p_connect(s, (sockaddr*)name, namelen);
}

int __stdcall listen_override(SOCKET s, int backlog) {
	struct sockaddr_in name;
	int namelen = 16;
	getsockname(s, (sockaddr*)&name, &namelen);
	printf("Listening on %s:%d\n", inet_ntoa(name.sin_addr), htons(name.sin_port));
	return p_listen(s, backlog);
}

SOCKET __stdcall accept_override(SOCKET s, struct sockaddr_in* addr, int* addrlen) {
	struct sockaddr_in name;
	int namelen = 16;
	getsockname(s, (sockaddr*)&name, &namelen);
	printf("Accepting connection from %s on port %d\n", inet_ntoa(addr->sin_addr), htons(name.sin_port));
	return p_accept(s, (sockaddr*)addr, addrlen);
}

bool InjectHostOverride() {
	HMODULE hmod = GetModuleHandle("ws2_32.dll");
	if (hmod == NULL) {
		printf("Failed to get module 'ws2_32.dll'\n");
		return false;
	}
	*(FARPROC*)&p_bind = GetProcAddress(hmod, "bind");
	if (p_bind == NULL) {
		printf("Failed to get address of 'bind'\n");
		return false;
	}
	*(FARPROC*)&p_gethostname = GetProcAddress(hmod, "gethostname");
	if (p_gethostname == NULL) {
		printf("Failed to get address of 'gethostname'\n");
		return false;
	}
	*(FARPROC*)&p_gethostbyname = GetProcAddress(hmod, "gethostbyname");
	if (p_gethostbyname == NULL) {
		printf("Failed to get address of 'gethostbyname'\n");
		return false;
	}
	*(FARPROC*)&p_connect = GetProcAddress(hmod, "connect");
	if (p_connect == NULL) {
		printf("Failed to get address of 'connect'\n");
		return false;
	}
	*(FARPROC*)&p_listen = GetProcAddress(hmod, "listen");
	if (p_listen == NULL) {
		printf("Failed to get address of 'listen'\n");
		return false;
	}
	*(FARPROC*)&p_accept = GetProcAddress(hmod, "accept");
	if (p_accept == NULL) {
		printf("Failed to get address of 'accept'\n");
		return false;
	}
	//
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (DetourAttach((PVOID*)(&p_bind), (PVOID)bind_override)) {
		printf("Failed to detour 'bind'\n");
	}
	if (DetourAttach((PVOID*)(&p_gethostname), (PVOID)gethostname_override)) {
		printf("Failed to detour 'gethostname'\n");
	}
	if (DetourAttach((PVOID*)(&p_gethostbyname), (PVOID)gethostbyname_override)) {
		printf("Failed to detour 'gethostbyname'\n");
	}
	if (DetourAttach((PVOID*)(&p_connect), (PVOID)connect_override)) {
		printf("Failed to detour 'connect'\n");
	}
	if (DetourAttach((PVOID*)(&p_listen), (PVOID)listen_override)) {
		printf("Failed to detour 'listen'\n");
	}
	if (DetourAttach((PVOID*)(&p_accept), (PVOID)accept_override)) {
		printf("Failed to detour 'accept'\n");
	}
	if (DetourTransactionCommit() != NO_ERROR) {
		printf("InjectHostOverride failed.\n");
		return false;
	}
	printf("InjectHostOverride success.\n");
	return true;
}


#pragma endregion // NetworkOverrides
#pragma endregion // Detours


void PrintCallStack(DWORD retAddr)
{
	printf("[BWFix]   caller[0]=0x%08X\n", retAddr);
	DWORD* ebp = (DWORD*)_AddressOfReturnAddress();
	for (int i = 1; i < 10; i++)
	{
		__try
		{
			ebp = (DWORD*)*ebp;
			printf("[BWFix]   caller[%d]=0x%08X\n", i, *(ebp + 1));
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { break; }
	}
}

// This usually fails when game crashes on fail/save. Detouring this to not crash will at least allow the user to not randomly crash.
// Additionally, occasionally "ProcessDeadList" can very occasionally fail on this due to I think corrupt vtable on the object. This is non fatal though but detour should prevent a non-needed crash.
char* (__cdecl* FindCompleteObject)(void** a1) = (char* (__cdecl*)(void**))(0x007DEA2B);
char* __cdecl FindCompleteObject_Detour(void** a1)
{
	__try
	{
		return FindCompleteObject(a1);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("[BWFix] FindCompleteObject: suppressed crash a1=0x%08X\n", (DWORD)a1);
		return nullptr;
	}
}
char* (__cdecl* RTDynamicCast)(void** a1, int a2, struct TypeDescriptor* a3, struct TypeDescriptor* a4, int a5) = (char* (__cdecl*)(void**, int, struct TypeDescriptor*, struct TypeDescriptor*, int))(0x007DE8C3);
char* __cdecl RTDynamicCast_Detour(void** a1, int a2, struct TypeDescriptor* a3, struct TypeDescriptor* a4, int a5)
{
	__try
	{
		return RTDynamicCast(a1, a2, a3, a4, a5);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("[BWFix] RTDynamicCast: suppressed crash a1=0x%08X *a1=0x%08X\n", (DWORD)a1, a1 ? *(DWORD*)a1 : 0);
		return nullptr;
	}
}

BOOL(__thiscall* MapCoords_InBounds)(unsigned __int16* _this) = (BOOL(__thiscall*)(unsigned __int16*))(0x0061C8F0);
BOOL __fastcall MapCoords_InBounds_Detour(unsigned __int16* _this, int edx)
{
	if ((DWORD)_ReturnAddress() == 0x0064D654)
	{
		printf("Rock delete thingy\n");
		return true;
	}

	BOOL result = MapCoords_InBounds(_this);
	return result;
}

void* (__thiscall* Object_EndPhysics)(void* _this, int a2, char a3) = (void* (__thiscall*)(void*, int, char))(0x0064D5D0);
void* __fastcall Object_EndPhysics_Detour(void* _this, int edx, int a2, char a3)
{
	printf("[Object::EndPhysics] Object::EndPhysics this=0x%08X a2=%d a3=%d\n", (DWORD)_this, a2, a3);
	return Object_EndPhysics(_this, a2, a3);
}

#pragma region DetourRegistration
void ShaneDetours()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());


	// In progress
	//DETOUR(MapCoords_InBounds);
	//DETOUR(Morphable__GetAnim);

	// Dangling pointer error fixing.
	DETOUR(BuildingSite_Save);
	//DETOUR(SpecialVillager_Create);
	DETOUR(FindCompleteObject);
	DETOUR(RTDynamicCast);

	// Functioning Detours
	DETOUR(GSetup__LoadMapFeatures);
	DETOUR(GGame__ClearMap);
	DETOUR(GGame__StartGame);
	DETOUR(GGame__Loop);
	DETOUR(BWCheckFeatureIsEnabled);
	DETOUR(GGame_ProcessKey);
	DETOUR(Process3DEngine);
	DETOUR(MobileWallHug__MoveToCircleHugLinearSquareSweep);
	DETOUR(GScript__IsScriptRunning); // Fix Creature Curse.
	DETOUR(GetRemapedPlayer); // For colour loading on first map load.
	DETOUR(GScript__CreateYoungCreatureWithKnowledge);

	// ----- Creature Stuff ----- //
	DETOUR(Creature__ForceActivityAndForceAction);
	DETOUR(Creature__Initialise);
	DETOUR(Creature__Load);
	//DETOUR(Creature__Load_OS);
	DETOUR(Creature__LoadFullyQualified);
	DETOUR(Creature__SwapMinds);
	DETOUR(CreatureMental__LoadMind);
	DETOUR(CreatureMental__SaveMind);
	DETOUR(GBalance__LoadVariables);
	DETOUR(GScript__SetObjectTattoo);
	DETOUR(LH3DCreature__LoadBinary);
	DETOUR(LH3DCreature__StateSwitch);
	DETOUR(LHOSFile__Read);

	// ----- Settings ----- //
	DETOUR(Creature__SubStatePerformBuild);
	DETOUR(Creature__SubStatePerformCreatePlannedCivicBuilding);
	DETOUR(Creature__SubStatePerformCreatePlannedAbode);
	DETOUR(Creature__SubStatePerformCastPlannedAbode);
	DETOUR(Villager__CheckDeathFromOldAge); // Eternal Special Villagers

	// ----- Multiplayer Related Detours ----- //
	DETOUR(MPFEMessageObject__ProcessIncomingMessage);
	DETOUR(MPFEMessageObject__Send);
	DETOUR(MPFEData__GetLocalPlayer);

	if (DetourTransactionCommit() == NO_ERROR)
		printf("Detours are good.\n");


	// Multiplayer Stuff (only for local MP testing, commented out by default).
	/*
	char* cmd = GetCommandLineA();
	char* argv[32];
	int argc = splitArgs(cmd, ' ', argv, 32);
	char* ip = getArgVal(argv, argc, "/ip");
	if (ip != NULL) setBindAddressOverride(ip);
	char* hostname = getArgVal(argv, argc, "/hostname");
	if (hostname != NULL) setHostnameOverride(hostname);
	if (ip != NULL || hostname != NULL) InjectHostOverride();
	*/
}

static void inline Fail(const char* a) { MessageBoxA(0, a, "Error", MB_TASKMODAL); ExitProcess(1); }
void FreeDetours()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourTransactionCommit();
}

void init_mods()
{
	char filepath[MAX_PATH];
	GetModuleFileName(0, filepath, MAX_PATH);

	std::string file_name = std::filesystem::path(filepath).filename().u8string();

	// Only run for "BWUltimate.exe".
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

#pragma endregion // DetourRegistration