#include "ddraw.h"
#include "mods.h"

ddrawDll ddraw;

static void LoadOriginalDll() {
	char path[MAX_PATH];
	CopyMemory(path + GetSystemDirectoryA(path, MAX_PATH - 10), "\\ddraw.dll", 11); // path to original dll

	ddraw.dll = LoadLibraryA(path);
	if (ddraw.dll) {
		ddraw.AcquireDDThreadLock = GetProcAddress(ddraw.dll, "AcquireDDThreadLock");
		ddraw.CheckFullscreen = GetProcAddress(ddraw.dll, "CheckFullscreen");
		ddraw.CompleteCreateSysmemSurface = GetProcAddress(ddraw.dll, "CompleteCreateSysmemSurface");
		ddraw.D3DParseUnknownCommand = GetProcAddress(ddraw.dll, "D3DParseUnknownCommand");
		ddraw.DDGetAttachedSurfaceLcl = GetProcAddress(ddraw.dll, "DDGetAttachedSurfaceLcl");
		ddraw.DDInternalLock = GetProcAddress(ddraw.dll, "DDInternalLock");
		ddraw.DDInternalUnlock = GetProcAddress(ddraw.dll, "DDInternalUnlock");
		ddraw.DSoundHelp = GetProcAddress(ddraw.dll, "DSoundHelp");
		ddraw.DirectDrawCreateClipper = GetProcAddress(ddraw.dll, "DirectDrawCreateClipper");
		ddraw.DirectDrawCreate = GetProcAddress(ddraw.dll, "DirectDrawCreate");
		ddraw.DirectDrawCreateEx = GetProcAddress(ddraw.dll, "DirectDrawCreateEx");
		ddraw.DirectDrawEnumerateA = GetProcAddress(ddraw.dll, "DirectDrawEnumerateA");
		ddraw.DirectDrawEnumerateExA = GetProcAddress(ddraw.dll, "DirectDrawEnumerateExA");
		ddraw.DirectDrawEnumerateExW = GetProcAddress(ddraw.dll, "DirectDrawEnumerateExW");
		ddraw.DirectDrawEnumerateW = GetProcAddress(ddraw.dll, "DirectDrawEnumerateW");
		ddraw.GetDDSurfaceLocal = GetProcAddress(ddraw.dll, "GetDDSurfaceLocal");
		ddraw.GetOLEThunkData = GetProcAddress(ddraw.dll, "GetOLEThunkData");
		ddraw.GetSurfaceFromDC = GetProcAddress(ddraw.dll, "GetSurfaceFromDC");
		ddraw.RegisterSpecialCase = GetProcAddress(ddraw.dll, "RegisterSpecialCase");
		ddraw.ReleaseDDThreadLock = GetProcAddress(ddraw.dll, "ReleaseDDThreadLock");
	}
}

static void FreeOriginalDll()
{
	if (ddraw.dll)
		FreeLibrary(ddraw.dll);
}

bool _stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpreserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		LoadOriginalDll();

		//DisableThreadLibraryCalls(hModule);

		init_mods();
		break;
	case DLL_PROCESS_DETACH:
		//deinit_mods();
		FreeOriginalDll();
		break;
	}

	return true;
}