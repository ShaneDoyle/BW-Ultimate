#pragma once

#include <Windows.h>
#include "BW_MP_types.h"

/*
	this file contains pointers to all functions
*/

struct LH3DColor
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
};

struct LHRegion
{
	int x;
	int y;
	int w;
	int h;
};

struct PathCreator
{
	char* DevPath;
	char* ProfilesPath;
	char* SavedGamesPath;
	char* PicturesPath;
	char* CurrentPath;
	char* VersionPathMaybe;
	char GameRootPath[256];
	char* Path2;
	char ProfileNameKey[256];
	char* ProfileNameKeyPtr;
};

// simple thing to compile for either bw1 or creature isle
#define GAME_BLACKANDWHITE
// #define GAME_CREATUREISLE

#ifdef GAME_BLACKANDWHITE

bool (*PathCreator_CreateDir)(LPCSTR lpPathName) = (bool(*)(LPCSTR))(0x0078EBF0);

void (*BW_Report3D)(char *, ...) = (void(*)(char *, ...))(0x0081BBD0);
void (__cdecl* BW_LH3DRender_SetRenderState)(DWORD, DWORD) = (void(__cdecl*)(DWORD state, DWORD value))(0x00412940);
void (__cdecl* BW_PlayPreIntroVideo)() = (void(__cdecl*)())(0x006426F0);
void (__cdecl* BW_LH3DIsland_Draw)() = (void(__cdecl*)())(0x007FF610);

void(__thiscall* BW_ProcessGraphicsEngine)(void*, int a2, int a3) = (void(__thiscall*)(void*, int a2, int a3))(0x0054D850);
void(__thiscall* BW_GGame_Draw)(void*) = (void(__thiscall*)(void*))(0x005533B0);
void(__thiscall* BW_GGame_Process3dEngine)(void*) = (void(__thiscall*)(void*))(0x0054DA80);


// Skirmish Detour
int(__thiscall* menu_playerColor)() = (int(__thiscall*)())(0x0057B520);
int(__thiscall* GPlayer__ProcessPlayers)(int) = (int(__thiscall*)(int ebp))(0x0065FCD0);
bool(__thiscall* menu_setSkirmish)(DWORD*) = (bool(__thiscall*)(DWORD*))(0x00643D70);
int(__cdecl* GameStats__PlayerLostTheGame)(DWORD* a1, int a2) = (int(__cdecl*)(DWORD*, int))(0x0057F4E0);
int(__thiscall* GGame__GetNumberOfPlayersThatHaveLost)() = (int(__thiscall*)())(0x005696E0);
int(__thiscall* GPlayer__Process)(DWORD* a1) = (int(__thiscall*)(DWORD*))(0x0065F7D0);
int(__thiscall* GameBlock__Clean)() = (int(__thiscall*)())(0x005F9DE0);

//Draw distance.
const CHAR* (__cdecl* sub_8A8880)(DWORD* a1) = (const CHAR * (__cdecl*)(DWORD*))(0x008A8880);
int (__fastcall* GPlayer__GetCreature)(int a1) = (int(__fastcall*)(int))(0x000E5430);


//On creature delete.
int(__fastcall* sub_4887D0)() = (int(__fastcall*)())(0x004887D0);
//int(__thiscall* LH3DCreature__StateSet)(DWORD* result, int a2) = (int(__thiscall*)(DWORD* result, int a2))(0x00214CE0);

//Testing...
int(__cdecl* sub_5547E0)(DWORD* a1) = (int(__cdecl*)(DWORD*))(0x005547E0);

void(*BW_DrawMouseCross)() = (void(*)())(0x0054D6F0);

void(__cdecl* BW_SetupThing_DrawTextWrap)(int, int, int, int, int, bool, const wchar_t *, int, LH3DColor*, int, int) = (void(__cdecl*)(int, int, int, int, int, bool, const wchar_t *, int, LH3DColor*, int, int))(0x00411750);

float(__thiscall* BW_GatheringText__DrawText)(void*, const wchar_t*, float, float, float, float, float, float, float, float, float, LH3DColor*, int, int, int) =
(float(__thiscall*)(void*, const wchar_t*, float, float, float, float, float, float, float, float, float, LH3DColor*, int, int, int))(0x008315B0);

void(__cdecl* BW_SetupThing_DrawBg)(int, int, int, int, LH3DColor, int, int) = (void(__cdecl*)(int, int, int, int, LH3DColor, int, int))(0x00413960);
void(__cdecl* BW_BackgroundBox_Draw)(LHRegion*, LH3DColor*, int) = (void(__cdecl*)(LHRegion*, LH3DColor*, int))(0x0081E590);

inline short LHScreen_Width() { return *(short*)0xE85058; }
inline short LHScreen_Height() { return *(short*)0xE8505A; }

void(__thiscall* BW_PathCreator)(void*) = (void(__thiscall*)(void*))(0x0078E8C0);
void(__thiscall* BW_PathCreator_GetProfilePath)(void*, char* a2) = (void(__thiscall*)(void*, char* a2))(0x0078EA00);

void(__cdecl* BW_DrawLoading)(float, float) = (void(__cdecl*)(float, float))(0x005F3DC0);

void(*BW_LH3DRender_StartFrame)()  = (void(*)())(0x0082F0E0);
void(*BW_LH3DRender_FinishFrame)() = (void(*)())(0x0082F460);

void(__thiscall* Game3DObject_AddForDrawing)(void*) = (void(__thiscall*)(void*))(0x0063B5D0);
void(__thiscall* LH3DMeshedObject_AddDrawing)(void*) = (void(__thiscall*)(void*))(0x00815A70);

void(__thiscall* LoaderAnon_ReadVariableFile)(void*, char* a2) = (void(__thiscall*)(void*, char* a2))(0x005F32F0);
void(__thiscall* LoaderAnon_StoreFieldNamesOrInstances)(void*, char* a2) = (void(__thiscall*)(void*, char* a2))(0x005F3680);
void(__thiscall* LoaderAnon_StoreInstancesOrFieldNames)(void*, char* a2) = (void(__thiscall*)(void*, char* a2))(0x005F39B0);
char*(__thiscall* LoaderAnon_AddField)(void*, char* a2, int, char, int16_t, int) = (char*(__thiscall*)(void*, char* a2, int, char, int16_t, int))(0x005F2C10);

void(__thiscall* LH3DWater_DrawSea)(void*) = (void(__thiscall*)(void*))(0x00879930);

// MULTIPLAYER METHODS:

// .text:00885740 ; char __cdecl peerStartListingGames(PEER *a1, void *a2, int a3)
// peerStartListingGames(g_gamespyPeer, BWGameSpy::ListingGamesCallback, 0);
void(__cdecl* peerStartListingGames)(void* peer, void* listCallback, int a3) = (void(__cdecl*)(void*, void*, int))(0x00885740);

// .text:0043F440; const wchar_t* __cdecl BWGameSpy::ListingGamesCallback(int peer, const wchar_t* a2, int a3, GServerImplementation* a4, int a5, int a6)
// .text:0043F440 BWGameSpy::ListingGamesCallback((void*, PEERBool, char const*, GServerImplementation*, PEERBool, int, int, void*)) proc near
// char __cdecl BWGameSpy::ListingGamesCallback(int peer, const wchar_t *a2, int a3, GServerImplementation *a4, int a5, int a6)
void(__cdecl* BWGameSpy_ListingGamesCallback)(void* peer, void* a2, int a3, void* a4, int a5, int a6) = (void(__cdecl*)(void*, void*, int, void*, int, int))(0x0043F440);

// _DWORD *__cdecl piNewListingGamesOperation(int a1, int a2, int a3)
void*(__cdecl* piNewListingGamesOperation)(int a1, int a2, int a3) = (void*(__cdecl*)(int, int, int))(0x0088A560);

// int __cdecl piListingGamesEnterChannelCallback(int a1, int a2, char a3, _DWORD *a4)
int(__cdecl* piListingGamesEnterChannelCallback)(void* chat, int a2, char* a3, int *a4) = (int(__cdecl*)(void*, int, char*, int*))(0x0088A610);

// char __cdecl piListingGamesChannelMessage(int a1, int a2, char *a3, char *a4, int a5, int i)
void*(__cdecl* piListingGamesChannelMessage)(int a1, int a2, char* a3, char* a4, int a5, int i) = (void*(__cdecl*)(int, int, char*, char*, int, int))(0x0088B120);

// .text:0088AE50 _piCEngineStartListingGames
// _DWORD *__cdecl piCEngineStartListingGames(_DWORD *a1)
int(__cdecl* _piCEngineStartListingGames)(void* a1) = (int(__cdecl*)(void*))(0x0088AE50);

void(__cdecl* chatEnterChannel)(void* chat, const char* channel, const char* password, void* callbacks, void* enterChannelCallback, void* param, int blocking) = (void(__cdecl*)(void*, const char*, const char*, void*, void*, void*, int))(0x00887240);

// .text:00896A30 _ServerListThink

int(__cdecl* ServerListThink)(int* a1) = (int(__cdecl*)(int*))(0x00896A30);

//.text:008969B0; int __cdecl ServerListAddServer(int serverlist, int inet, __int16 port, int a4)
void(__cdecl* ServerListAddServer)(void* serverlist, int ip, uint16_t port, int a4) = (void(__cdecl*)(void*, int, uint16_t, int))(0x008969B0);

// .text:008854E0 ; void __cdecl peerCreateStagingRoom(void *PEER, char *name, int maxPlayers, char *password, void *callback, void *param, bool blocking)
void(__cdecl* peerCreateStagingRoom)(void* PEER, char *name, int maxPlayers, char *password, void *callback, void *param, bool blocking) = (void(__cdecl*)(void*, char*, int, char*, void*, void*, bool))(0x008854E0);

// .text:00889F00 ; int __cdecl piNewCreateStagingRoomOperation(int, char *, int, int, int)
int(__cdecl* piNewCreateStagingRoomOperation)(int, char*, int, int, int*) = (int(__cdecl*)(int, char*, int, int, int*))(0x00889F00);

// .text:00889FF0 ; int __cdecl piCreateStagingRoomEnterChannelCallback(int chat, int success, char *channel, int a4)
int(__cdecl* piCreateStagingRoomEnterChannelCallback)(int chat, int success, char* channel, int a4) = (int(__cdecl*)(int chat, int success, char* channel, int a4))(0x00889FF0);

// .text:0043F360 ; SetupButton_t *__cdecl BWGameSpy::JoinCallback(PEER *peer, int a2, int a3, int a4)
void*(__cdecl* BWGameSpy_JoinCallback)(void* peer, int a2, int a3, int a4) = (void*(__cdecl*)(void*, int, int, int))(0x0043F360);

// .text:0043EF30 BWGameSpy::RoomJoined((RoomType)) proc near
void(__cdecl* BWGameSpy_RoomJoined)(int roomtype) = (void(__cdecl*)(int))(0x0043EF30);

// .text:00887880 ; int __cdecl chatEnumUsers()
void(__cdecl* chatEnumUsers)(void* chat, char* channel, void* callback, void* param, int id) = (void(__cdecl*)(void*, char*, void*, void*, int))(0x00887880);

// .text:00892650 ciInChannel 
bool(__cdecl* ciInChannel)(void* chat, char* channel) = (bool(__cdecl*)(void*, char*))(0x00892650);

// .text:0088DB50 ; DWORD __cdecl send_heartbeat(qr_t *qrec, int statechanged)
int(__cdecl* send_heartbeat)(void* qrec, int statechanged) = (int(__cdecl*)(void*, int))(0x0088DB50);





//////////////////////////////////////////////

//Ultimate Functions
int(__cdecl* TownCentre_DrawPSys)(DWORD*) = (int(__cdecl*)(DWORD*))(0x006B0AE0);

int(__thiscall* GetPlayerColour)(void*) = (int(__thiscall*)(void*))(0x006632D0);

int(__cdecl* GetRemapedPlayer)(int) = (int(__cdecl*)(int))(0x00663260);

double(__thiscall* Town__GetBaseInfluence)(int _this) = (double(__thiscall*)(int _this))(0x0075C660);

void*(__cdecl* PlayerSymbolSprite__Create)(uint8_t player_id) = (void * (__cdecl*)(uint8_t))(0x006B22D0);

int(__cdecl* ReloadSymbols)() = (int(__cdecl*)())(0x005F4300);

void(__thiscall* GGame__ClearMap)(void*) = (void(__thiscall*)(void*))(0x005670A0);

struct Game {};
Game* GetGame() { return (Game*)(0x00D7B614); }

void(__thiscall* GGame__StartGame)(Game*) = (void(__thiscall*)(Game*))(0x00560A10);

DWORD* (__thiscall* GGame__Loop)(char*) = (DWORD*(__thiscall*)(char*))(0x005617D0);

bool(__thiscall* GGame__IsMultiplayerGame)(DWORD* _this) = (bool(__thiscall*)(DWORD* _this))(0x005674C0);


void(__thiscall* DrawAll__TownCentre)() = (void(__thiscall*)())(0x00761230);

int(__cdecl* GSetup__LoadMapFeatures)(const char* a1) = (int(__cdecl*)(const char* a1))(0x007336C0);

//MP Stuff
int(__thiscall* TeamControls__InitControls)(DWORD*, int, int, int, int, int) = (int(__thiscall*)(DWORD*, int, int, int, int, int))(0x006338D0);
//


Game* (__thiscall* GScript__Reset)(DWORD*, int) = (Game* (__thiscall*)(DWORD*, int))(0x00700220);

char (__thiscall* BWCheckFeatureIsEnabled)(char*) = (char (__thiscall*)(char*))(0x0053A3B0);
int (__thiscall* stop_draw_sprite_to_screen)() = (int (__thiscall*)())(0x00657DC0);
bool (__thiscall* GSetup__LoadMapScript)() = (bool (__thiscall*)())(0x00715080);

signed int(__thiscall* GGame__Init)(void*) = (signed int(__thiscall*)(void*))(0x00563AA0);


int(__cdecl* _SaveAllMap)(char* lpFilename) = (int(__cdecl*)(char* lpFilename))(0x00733940);
int(__cdecl* _SaveLandOnDisk)(char* lpFilename) = (int(__cdecl*)(char* lpFilename))(0x0081E560);


void(__thiscall* PauseGame)(int num) = (void(__thiscall*)(int num))(0x0055F7E0); //sub_55F7E0



void(__thiscall* GGame_ProcessKey)(unsigned __int8*, int, int) = (void(__thiscall*)(unsigned __int8*, int, int))(0x00654F80); //sub_55F7E0


//Tribe stuff
int (__cdecl* GTribeInfo__GetTribeFromText)(char* a1) = (int (__cdecl*)(char* a1))(0x00769260);

// Abode stuff
DWORD* (__cdecl* Abode__Create)(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) =
(DWORD*(__cdecl*)(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10))(0x00402F10);

int* (__cdecl* GAbodeInfo__GetInfoFromText)(char* a1) = (int* (__cdecl*)(char* a1))(0x00405A10);

// Villager stuff
int (__cdecl* Villagers__Create)(int a1, int a2, int a3, int a4) = (int (__cdecl*)(int a1, int a2, int a3, int a4))(0x0076C480);

unsigned int(__cdecl* GVillagerInfo__GetInfoFromText)(char* a1) = (unsigned int(__cdecl*)(char* a1))(0x0076E240);

int (__cdecl* GSetup__FeatureMapCommandProcess)(int a1, LPCSTR lpMultiByteStr) = (int (__cdecl*)(int a1, LPCSTR lpMultiByteStr))(0x00730700);
int (__cdecl* GSetup__MapCommandProcess)(int a1, char* a2) = (int(__cdecl*)(int a1, char* a2))(0x00730210);


 


signed int(__thiscall* EveCreatureFunction)(char* _this, const char* a2) = (signed int(__thiscall*)(char* _this, const char* a2))(0x00631250);

int(__thiscall* Creature__isFemale)(byte *_this) = (int(__thiscall*)(byte *_this))(0x0048A260);

double(__thiscall* CreatureMaximumPickUpWeight)(void* _this) = (double(__thiscall*)(void* _this))(0x004F4550);


// SaveMind helpers.
int(__thiscall* GPlayer__GetLeaderInterfaceStatus)(DWORD* a1) = (int(__thiscall*)(DWORD* a1))(0x006604C0);
int(__thiscall* GInterfaceStatus__GetInterface)(DWORD* a1) = (int(__thiscall*)(DWORD* a1))(0x005F1D40);


DWORD* (__thiscall* Creature__Creature)(DWORD* _this, int a2, int a3, int a4, int a5) = (DWORD* (__thiscall*)(DWORD* _this, int a2, int a3, int a4, int a5))(0x0047DE00);

signed int (__thiscall* Creature__Load_OS)(unsigned __int8* _this, int a2) = (signed int (__thiscall*)(unsigned __int8* _this, int a2))(0x004F6AD0);

int(__thiscall* Creature__ResolveLoad)(int _this) = (int(__thiscall*)(int _this))(0x004F78A0);

//int(__thiscall* GScript__CreateYoungCreatureWithKnowledge)() = (int(__thiscall*)())(0x007098A0);

void(__thiscall * GPacket__ProcessPacket)(int _this, double st7_0, int a2)= (void(__thiscall*)(int, double, int))(0x006523D0);



int(__thiscall* CreatureSubActionAgenda__Load)(DWORD* _this, int a2, int a3) = (int(__thiscall*)(DWORD * _this, int a2, int a3))(0x004F6760);


//TODO: Give this a name (hooking female Creature code into this).
int(__thiscall* sub_4FAA10)(int _this, LPVOID lpBuffer) = (int(__thiscall*)(int _this, LPVOID lpBuffer))(0x004FAA10);
int(__thiscall* Creature__GetCreature3D)(DWORD* _this) = (int(__thiscall*)(DWORD* _this))(0x00482360);


int(__cdecl* Creature__CreateFemale)(int a1, int a2, int a3, int a4) = (signed int(__cdecl*)(int a1, int a2, int a3, int a4))(0x0047ED60);
int(__thiscall* HelpSystem__RunMessage)(DWORD* _this, unsigned int a2, unsigned int a3, int a4) = (int(__thiscall*)(DWORD * _this, unsigned int a2, unsigned int a3, int a4))(0x005DE2A0);



DWORD* (__thiscall* female_setter_maybe)(DWORD* _this, BYTE* a2) = (DWORD * (__thiscall*)(DWORD * _this, BYTE* a2))(0x0072D710);

int(__cdecl* Creature__Create)(int* a1, int a2, int a3, int a4) = (int(__cdecl*)(int* a1, int a2, int a3, int a4))(0x0047EDA0);



signed int(__thiscall* LH3DCreature__LoadBase)(char* a1, const char* a2) = (signed int(__thiscall*)(char* a1, const char* a2))(0x004FC000);

signed int(__thiscall* Morphable__LoadMesh)(DWORD* _this, const char* a2, signed int a3) = (signed int(__thiscall*)(DWORD * _this, const char* a2, signed int a3))(0x00631630);

signed int(__thiscall* Morphable__LoadBase)(char* a1, const char* a2) = (signed int(__thiscall*)(char* a1, const char* a2))(0x00631250); 


int(__thiscall* LH3D__LoadBinary)(char* _this, const char* a2, int a3) = (int(__thiscall*)(char* _this, const char* a2, int a3))(0x004FC7E0);

int(__thiscall* GBalance__LoadVariables)(void* _this, int a2) = (int(__thiscall*)(void* _this, int a2))(0x0042C930);


int(__thiscall* LH3DCreature__Init)(DWORD* _this, int a2, int a3) = (int(__thiscall*)(DWORD * _this, int a2, int a3))(0x0048B010);


int(__thiscall* LH3DCreature__LH3DCreature)(int _this, int a2, int a3, int a4) = (int(__thiscall*)(int _this, int a2, int a3, int a4))(0x0048A810);





int(__thiscall* LH3DCreature__LH3DCreature_2)(DWORD* _this, int a2, int a3, int a4) = (int(__thiscall*)(DWORD * _this, int a2, int a3, int a4))(0x0048AB30);

int(__thiscall* Creature__Initialise)(DWORD* _this, DWORD* a2, int a3, int a4, int a5) = (int(__thiscall*)(DWORD * _this, DWORD * a2, int a3, int a4, int a5))(0x0047E610);

void(__thiscall* LH3DCreature__SetSize)(int _this, float size) =
(void(__thiscall*)(int, float))0x0048B9B0;

void (__thiscall* Creature__SetTyke)(DWORD* _this, DWORD* a2) = (void(__thiscall*)(DWORD*, DWORD*))(0x0047EF30);




signed int(__cdecl* SetupThing__adjusty)(int a1) = (signed int(__cdecl*)(int))(0x0411D80);




int (__thiscall* Process3DEngine)(BYTE*) = (int(__thiscall*)(BYTE*))(0x00562310);


int(__thiscall* ControlMap__ProcessActionsPerformed)(DWORD*) = (int(__thiscall*)(DWORD*))(0x0047A6B0);


void (__thiscall* MobileWallHug__SetupMobileMoveToPos)(DWORD* a1, DWORD* a2, int a3) = (void(__thiscall*)(DWORD* a1, DWORD* a2, int a3))(0x00623860);

signed int(__thiscall* MobileWallHug__MoveToCircleHugLinearSquareSweep)(DWORD* a1, int a2) = (signed int(__thiscall*)(DWORD * a1, int a2))(0x00625740);



void(__thiscall* GGame__GetPlayerCreature)(DWORD* GGame, int* Number, int unk) = (void(__thiscall*)(DWORD* GGame, int* Number, int unk))(0x005FF140);


int(__thiscall* GGame__GetPlayerColour)(DWORD* Player) = (int(__thiscall*)(DWORD* Player))(0x006632D0);


// Custom Creature
void(__thiscall* loadgame20MainFunction1)() = (void(__thiscall*)())(0x00578BA0);
void(__thiscall* loadMapFunction)() = (void(__thiscall*)())(0x0075B3D0);
void(__thiscall* start_system)() = (void(__thiscall*)())(0x00659380);

// Multiplayer Lobby
char(__cdecl* GatheringBox__SendPacketToPlayers)(char a1, unsigned int a2, int a3, char a4) = (char(__cdecl*)(char a1, unsigned int a2, int a3, char a4))(0x0058A060);


char (__cdecl* MPFEChangedTeamRequestMessage__ProcessIncoming)(wchar_t* a1, int a2) = (char(__cdecl*)(wchar_t* a1, int a2))(0x00648D80);
int(__thiscall* sub_6385B0_getmessagestring)(DWORD* _this, wchar_t* a2, int a3, char a4, char a5, char a6) =
(int(__thiscall*)(DWORD * _this, wchar_t* a2, int a3, char a4, char a5, char a6))(0x006385B0);
char(__cdecl* MPFEAskForTeamDetailsMessage__ProcessIncoming)(int a1, int a2) = (char(__cdecl*)(int a1, int a2))(0x00649300);
void(__thiscall* MPFEPlayerDetails__AssignToTeam)(int _this, int a2) = (void(__thiscall*)(int _this, int a2))(0x0648210);
wchar_t* (__cdecl* MPFEData__D2A_IncomingChatString)(int a1, wchar_t* a2) = (wchar_t*(__cdecl*)(int a1, wchar_t* a2))(0x0063DE10);
void* (__thiscall* SetupList__InsertString)(DWORD* _this, int a2, wchar_t* a3) = (void* (__thiscall*)(DWORD* _this, int a2, wchar_t* a3))(0x0040AE00);
int* (__thiscall* MPFEData__GetPlayerDetailsFromName)(DWORD* _this, wchar_t* a2) = (int* (__thiscall*)(DWORD* _this, wchar_t* a2))(0x0063A160);

DWORD* (__thiscall* MPFEData__D2A_RefreshTeams)(void** _this) = (DWORD * (__thiscall*)(void** _this))(0x00638DC0);
int (__thiscall* MPFEData__GetLowestNumberedPlayerInTeam)(DWORD* _this, int a2) = (int (__thiscall*)(DWORD* _this, int a2))(0x0063C4F0);

DWORD* (__thiscall* ChannelBox__UpdatePlayerInfo)(DWORD* _this) = (DWORD * (__thiscall*)(DWORD * _this))(0x00636480);
void (__thiscall* ChannelBox__UpdateClans)(DWORD* _this) = (void (__thiscall*)(DWORD * _this))(0x00635B00);
int(__thiscall* ChannelBox__UpdateTeams)(int _this) = (int(__thiscall*)(int _this))(0x00636730);


// Multiplayer InGame Stuff
int(__thiscall* SCROLLCLICKED)(DWORD* _this, int a2) = (int(__thiscall*)(DWORD * _this, int a2))(0x00725030);

double(__thiscall* FloatRand)(DWORD* _this, float a2, const char* a3, int a4) = (double(__thiscall*)(DWORD * _this, float a2, const char* a3, int a4))(0x005235D0);
int(__thiscall* NormalRand)(DWORD* _this, int a2, const char* a3, int a4) = (int(__thiscall*)(DWORD * _this, int a2, const char* a3, int a4))(0x00523570);

int (__thiscall* OOSDebug__GetOverallChecksum)() = (int (__thiscall*)())(0x006522C0);
int(__stdcall* GNetwork__PrepareCheckSum)(DWORD* a1) = (int(__stdcall*)(DWORD * a1))(0x0064A4C0);

int(__cdecl* SendNetworkChecksum)(char a1) = (int(__cdecl*)(char a1))(0x0064B280);

int(__thiscall* ScriptDLL__RestoreState)(int _this, int a2) = (int(__thiscall*)(int _this, int a2))(0x0070D970);

int(__thiscall* GGame__Load)(unsigned __int8* _this, int a2) = (int(__thiscall*)(unsigned __int8* _this, int a2))(0x00568DC0);
int(__thiscall* GGame__Load_Shane)(unsigned char* _this, int a2) = (int(__thiscall*)(unsigned char* _this, int a2))(0x00568DC0);

// ***** GScript ***** /
void(__thiscall* GScript__Create)() = (void(__thiscall*)())(0x00700120);
int(__thiscall* GScript__GetComputerPlayerPos)() = (int(__thiscall*)())(0x00704630);
int(__thiscall* CHL__Command)(int _this, int a2, int a3, int a4, int a5, int a6, int a7) = (int(__thiscall*)(int _this, int a2, int a3, int a4, int a5, int a6, int a7))(0x0070D7E0);


// ***** LHConnection ***** /
// 
//int(__thiscall *LHConnection__RawPeek)(DWORD a1, DWORD a2, DWORD a3) = (int(__thiscall*)(DWORD a1, DWORD a2, DWORD a3))(0x008D4604);

// ***** GComputerPlayer *****/
DWORD* (__thiscall* GComputerPlayer__GetHandPos)(DWORD* _this, DWORD* a1) = (DWORD * (__thiscall*)(DWORD* _this, DWORD * a1))(0x0066D2B0);

// Gnetwork::Sendpacket
int(__thiscall* sub_64B4B0)(DWORD* _this, int a2, int a3) = (int(__thiscall*)(DWORD * _this, int a2, int a3))(0x0064B4B0);

int(__fastcall* GNetwork__ProcessOnePacket)(GNetwork* _this) = (int(__fastcall*)(GNetwork * _this))(0x0064AA60);

char(__thiscall* GNetwork__SendPacket)(BYTE* _this) = (char(__thiscall*)(BYTE * _this))(0x00566B40);


//char __thiscall GNetwork::SendPacket(BYTE* _this)


HMODULE mpDll = GetModuleHandleA("LHMultiplayerR.DLL");
int(__thiscall* LHConnection__RawPeek)(DWORD a1, DWORD a2, DWORD a3) =
(int(__thiscall*)(DWORD a1, DWORD a2, DWORD a3)) GetProcAddress(mpDll, "?RawPeek@LHConnection@@QAEPAVLHNetEvent@@KW4LH_NETEVENT_TYPE@@@Z");

int(__cdecl* LHNetEvent__VDecode)(DWORD a1, DWORD a2, DWORD a3, DWORD a4) =
(int(__cdecl*)(DWORD a1, DWORD a2, DWORD a3, DWORD a4)) GetProcAddress(mpDll, "?VDecode@LHNetEvent@@QAA?AW4LH_RETURN@@W4LH_NETEVENT_TYPE@@ZZ");

int(__thiscall* LHTransport__GetFirstEventOfType)(DWORD* _this, int a2) =
(int(__thiscall*)(DWORD * _this, int a2)) GetProcAddress(mpDll, "?GetFirstEventOfType@LHTransport@@QAEPAVLHNetEvent@@W4LH_NETEVENT_TYPE@@@Z");


LHNetEvent*(__thiscall* LHTransport__Peek)(LHTransport* _this, unsigned int a2) =
(LHNetEvent*(__thiscall*)(LHTransport * _this, unsigned int a2)) GetProcAddress(mpDll, "?Peek@LHTransport@@UAEPAVLHNetEvent@@K@Z");

void (__thiscall* LHSession__SendDataPacketToAllGamePlayers)(LHSession* _this, unsigned int a2, void* a3, int a4) =
(void(__thiscall*)(LHSession * _this, unsigned int a2, void* a3, int a4)) GetProcAddress(mpDll, "?SendDataPacketToAllGamePlayers@LHSession@@QAEXKPAXH@Z");


int(__thiscall* LHConnection__Write)(LHNetEvent* _this, int a2) =
(int(__thiscall*)(LHNetEvent * _this, int a2)) GetProcAddress(mpDll, "?Write@LHConnection@@QAE?AW4LH_RETURN@@PAVLHNetEvent@@@Z");



// 2005 - Speed/Slow Event.
int(__thiscall* sub_10006670)(DWORD* _this, int a2, int a3) = (int(__thiscall*)(DWORD * _this, int a2, int a3))(mpDll + 0x0005670);


int(__cdecl* LHSPrintfW__LHSPrintfW)(DWORD _this, DWORD a2, DWORD a3) = (int(__cdecl*)(DWORD _this, DWORD a2, DWORD a3))(0x008D43B0);



typedef struct TownCentre
{
	char padding[4];
	int32_t colour;
} TownCentre;

typedef struct PlayerSymbolSprite
{
	char padding[8];
	DWORD colour;
} PlayerSymbolSprite;


typedef struct MapCoords
{
	int x;
	int z;
	float altitude;
} MapCoords;

typedef struct LHPoint {
	float x;
	float y;
	float z;
};

float(__fastcall* LH3DIsland__GetAltitude)(MapCoords* cords) = (float(__fastcall*)(MapCoords* cords))(0x0081FD80);


typedef struct Living
{
	char padding[0x2C];
	MapCoords MapCord;
} Living;

int(__cdecl* PlayerSymbolSprite__setPlayerID)(PlayerSymbolSprite* VillageCentre, uint8_t PlayerNum) = (int(__cdecl*)(PlayerSymbolSprite*, uint8_t))(0x06B23C0);


////////////////////////////////////////////////////////

typedef struct ciServerMessage
{
	char* message;
	char* server;
	char* nick; // s
	char* user; // null
	char* host;
	char* command;
	char* middle;
	char* param;
	char** params;
	int numParams;
} ciServerMessage;

// .text:00886DE0 ; signed int __cdecl ciProcessServerMessage(void *chat, ciServerMessage *message)
int(__cdecl* ciProcessServerMessage)(void* chat, ciServerMessage* message) = (int(__cdecl*)(void*, ciServerMessage*))(0x00886DE0);

// Extra Creature CBN expansion stuff.
/*
int(__thiscall* LoadCreatureAnims)(char* _this, int ctrspec, int ctrspec28, int a4) = (int(__thiscall*)(char*, int, int, int))(0x006309D0);

int(__thiscall* ProcessAnimSlots)(void** _this, int a2, int a3, int a4, int a5) =
(int(__thiscall*)(void**, int, int, int, int))0x00630DD0;

int(__thiscall* GetAnim)(char* _this, int slot, int a3) =
(int(__thiscall*)(char*, int, int))0x00630DA0;*/


#elif GAME_CREATUREISLE

// define creature isle function pointers here

#endif