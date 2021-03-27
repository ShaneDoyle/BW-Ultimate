#pragma once

#include <Windows.h>

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
void(__cdecl* TownCentre_DrawPSys)(void*) = (void(__cdecl*)(void*))(0x006B0AE0);

int(__thiscall* GetPlayerColour)(void*) = (int(__thiscall*)(void*))(0x006632D0);

int(__cdecl* GetRemapedPlayer)(int) = (int(__cdecl*)(int))(0x00663260);

void*(__cdecl* PlayerSymbolSprite__Create)(uint8_t player_id) = (void * (__cdecl*)(uint8_t))(0x006B22D0);

int(__cdecl* ReloadSymbols)() = (int(__cdecl*)())(0x005F4300);

void(__thiscall* GGame__ClearMap)(void*) = (void(__thiscall*)(void*))(0x005670A0);

struct Game {};
Game* GetGame() { return (Game*)(0x00D7B614); }

void(__thiscall* GGame__StartGame)(Game*) = (void(__thiscall*)(Game*))(0x00560A10);

void(__thiscall* GGame__Loop)(void*) = (void(__thiscall*)(void*))(0x005617D0);

bool(__thiscall* IsMultiplayerGame)(Game*) = (bool(__thiscall*)(Game*))(0x005674C0);

void(__thiscall* DrawAll__TownCentre)() = (void(__thiscall*)())(0x00761230);

Game* (__thiscall* GScript__Reset)(DWORD*, int) = (Game* (__thiscall*)(DWORD*, int))(0x00700220);

char (__thiscall* BWCheckFeatureIsEnabled)(char*) = (char (__thiscall*)(char*))(0x0053A3B0);
int (__thiscall* stop_draw_sprite_to_screen)() = (int (__thiscall*)())(0x00657DC0);
bool (__thiscall* GSetup__LoadMapScript)() = (bool (__thiscall*)())(0x00715080);

signed int(__thiscall* GGame__Init)(void*) = (signed int(__thiscall*)(void*))(0x00563AA0);


int(__cdecl* _SaveAllMap)(char* lpFilename) = (int(__cdecl*)(char* lpFilename))(0x00733940);
int(__cdecl* _SaveLandOnDisk)(char* lpFilename) = (int(__cdecl*)(char* lpFilename))(0x0081E560);


void(__thiscall* PauseGame)(int num) = (void(__thiscall*)(int num))(0x0055F7E0); //sub_55F7E0



void(__thiscall* GGame_ProcessKey)(unsigned __int8*, int, int) = (void(__thiscall*)(unsigned __int8*, int, int))(0x00654F80); //sub_55F7E0

int(__thiscall* Creature__Create)(int* a1, int a2, int a3, int a4) = (int(__thiscall*)(int* a1, int a2, int a3, int a4))(0x0047EDA0); //sub_55F7E0


 


signed int(__thiscall* EveCreatureFunction)(char* _this, const char* a2) = (signed int(__thiscall*)(char* _this, const char* a2))(0x00631250);





int (__thiscall* Process3DEngine)(BYTE*) = (int(__thiscall*)(BYTE*))(0x00562310);

//void(__thiscall* Process3DEngine)(void*, int, int) = (void(__thiscall*)(void*, int, int))(0x0054D850);

int(__thiscall* ControlMap__ProcessActionsPerformed)(DWORD*) = (int(__thiscall*)(DWORD*))(0x0047A6B0);

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

#elif GAME_CREATUREISLE

// define creature isle function pointers here

#endif