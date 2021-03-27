///* Functions used to interact with the .chl + reg values *///

/*
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
	PlayerColors[0] = P1;
	PlayerColors[1] = P2;
	PlayerColors[2] = P3;
	PlayerColors[3] = P4;
	PlayerColors[4] = P5;
	PlayerColors[5] = P6;
	PlayerColors[6] = P7;
}

//Set influence colours for all players.
void SetPlayerInfluenceColours(DWORD P1, DWORD P2, DWORD P3, DWORD P4, DWORD P5, DWORD P6, DWORD P7)
{
	DWORD* PlayerInfluenceColours = GetPlayerColours();
	PlayerInfluenceColours[0] = P1;
	PlayerInfluenceColours[1] = P2;
	PlayerInfluenceColours[2] = P3;
	PlayerInfluenceColours[3] = P4;
	PlayerInfluenceColours[4] = P5;
	PlayerInfluenceColours[5] = P6;
	PlayerInfluenceColours[6] = P7;
}
*/