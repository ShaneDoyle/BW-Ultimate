#pragma once

#include <Windows.h>

// Typed function pointers for main create functions
typedef HRESULT(__stdcall* DirectDrawCreateProc)(GUID* lpGUID, void** lplpDD, IUnknown* pUnkOuter);
typedef HRESULT(__stdcall* DirectDrawCreateExProc)(GUID* lpGUID, void** lplpDD, REFIID riid, IUnknown* pUnkOuter);

// Extern declarations for the real functions
extern DirectDrawCreateProc realDirectDrawCreate;
extern DirectDrawCreateExProc realDirectDrawCreateEx;

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
