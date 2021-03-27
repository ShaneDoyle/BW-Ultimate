#pragma once

#include <assert.h>
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>

struct ddrawDll {
	HMODULE dll;
	FARPROC DirectDrawEnumerateExA;
	FARPROC DirectDrawCreate;
	FARPROC DirectDrawCreateEx;
	FARPROC AcquireDDThreadLock;
	FARPROC ReleaseDDThreadLock;
	FARPROC CheckFullscreen;
	FARPROC CompleteCreateSysmemSurface;
	FARPROC D3DParseUnknownCommand;
	FARPROC DDGetAttachedSurfaceLcl;
	FARPROC DDInternalLock;
	FARPROC DDInternalUnlock;
	FARPROC DSoundHelp;
	FARPROC DirectDrawCreateClipper;
	FARPROC DirectDrawEnumerateA;
	FARPROC DirectDrawEnumerateExW;
	FARPROC DirectDrawEnumerateW;
	//FARPROC DllCanUnloadNow;
	//FARPROC DllGetClassObject;
	FARPROC GetDDSurfaceLocal;
	FARPROC GetOLEThunkData;
	FARPROC GetSurfaceFromDC;
	FARPROC RegisterSpecialCase;
};