struct LHLobby;
struct LHSyncTransport;
struct LHTransport;

struct ListEntry
{
  ListEntry *next;
  void *pValue;
};

struct List
{
  ListEntry *head;
  DWORD size;
};

struct QueueEntry
{
  void *pValue;
  QueueEntry *prev;
  QueueEntry *next;
};

struct Queue
{
  QueueEntry *head;
  QueueEntry *tail;
};

struct LHNetPacketHeader
{
  WORD unk0;
  WORD eventType;
  DWORD userId;
};

union LHNetPacketData
{
  LHNetPacketHeader header;
  BYTE data[];
};

struct LHConnectionVftable
{
  DWORD *ProcessEvent;
  DWORD *__dt__;
  DWORD *Close;
  LHNetPacketData temp; // *(__thiscall* Read)(LHSession*, int, DWORD);
};

struct LHConnection
{
  LHConnectionVftable *vftable;
  DWORD isConnected;
  int (__cdecl *customEventProcessor)(LHNetPacketData *, LHConnection *, DWORD);
  DWORD unk3;
  DWORD unk4;
  DWORD unk5;
  DWORD unk6;
  DWORD unk7;
  DWORD unk8;
  DWORD unk9;
  DWORD unk10;
  DWORD unk11;
  DWORD unk12;
  DWORD unk13;
  DWORD unk14;
  DWORD unk15;
  DWORD unk16;
  DWORD unk17;
  DWORD unk18;
  DWORD unk19;
  DWORD unk20;
  DWORD unk21;
  DWORD unk22;
  DWORD unk23;
  DWORD unk24;
  DWORD unk25;
  DWORD unk26;
  DWORD unk27;
  DWORD unk28;
  DWORD unk29;
  DWORD unk30;
  DWORD unk31;
  DWORD unk32;
  LHTransport* lhTransport;
  DWORD unk34;
  DWORD unk35;
};

struct LHChannel
{
  DWORD vftable;
  DWORD unk1;
  DWORD unk2;
  DWORD unk3;
  DWORD unk4;
  DWORD unk5;
  DWORD unk6;
  DWORD unk7;
  DWORD unk8;
  DWORD unk9;
  DWORD unk10;
  DWORD unk11;
  DWORD unk12;
  DWORD unk13;
  DWORD unk14;
  DWORD unk15;
  DWORD unk16;
  DWORD unk17;
  DWORD unk18;
  DWORD unk19;
  DWORD unk20;
  DWORD unk21;
  DWORD unk22;
  DWORD unk23;
  DWORD unk24;
  DWORD unk25;
  DWORD unk26;
  DWORD unk27;
  List playersList;
};

struct LHMPPacketSave
{
  DWORD unk0;
  DWORD unk1;
  DWORD unk2;
  DWORD unk3;
  DWORD unk4;
  DWORD unk5;
  DWORD count;
  DWORD unk7;
  DWORD unk8;
};

struct LHLobbyChannel
{
  LHChannel super;	//Need a union to handle vftable override
  DWORD unk30;
  DWORD unk31;
  DWORD unk32;
  DWORD unk33;
  DWORD unk34;
  DWORD packetSource;
  LHMPPacketSave lhmpPacketSave;
};

struct LHTransportInfo
{
  void *vftable;
  DWORD f1;
  DWORD f2;
  WORD port;
  BYTE ip[4];
  BYTE data[94];
  DWORD f4;
};

struct LHNetEvent
{
  LHNetPacketData *pData;
  DWORD f1;
  LHTransportInfo transportInfo;
};

struct LHPlayer
{
  DWORD unk0;
  wchar_t userFileName[130];
  DWORD userData;
  DWORD userDataLen;
  wchar_t name[30];
  DWORD unk83;
  DWORD unk84;
  DWORD unk85;
  DWORD unk86;
  DWORD unk87;
  DWORD unk88;
  DWORD unk89;
  DWORD unk90;
  DWORD unk91;
  DWORD unk92;
  DWORD playerId;
  DWORD userId;
  DWORD systemData;
  LHTransportInfo lhTransportInfo;
  DWORD unk125;
  DWORD unk126;
  DWORD unk127;
};

struct LHSession
{
  LHConnection conn;	//need a union to handle vftable override
  List playersList;
  DWORD f38;
  DWORD f39;
  LHLobbyChannel *lhLobbyChannel;
  Queue eventQueue;
  DWORD superPacketGameTurn;
  DWORD f44;
  DWORD f45;
  DWORD f46;
  DWORD f47;
  LHNetEvent *lastGameEventRead;
  DWORD f49;
  DWORD gameTickInterval;
  DWORD mgjInProgress;
  DWORD checksumErrorData;
  DWORD checksumErrorLength;
  LHPlayer *checksumErrorPlayer;
  DWORD checksumFromFileFlag;
  DWORD f56;
  DWORD f57;
  DWORD f58;
  DWORD f59;
  DWORD f60;
  DWORD f61;
  DWORD f62;
  DWORD f63;
  DWORD f64;
  DWORD f65;
  DWORD f66;
  DWORD f67;
  DWORD f68;
  DWORD f69;
  DWORD f70;
  DWORD f71;
  DWORD f72;
  DWORD f73;
  DWORD f74;
  DWORD f75;
  DWORD f76;
  DWORD f77;
  DWORD f78;
  DWORD f79;
  DWORD f80;
  DWORD f81;
  DWORD f82;
  DWORD f83;
  DWORD f84;
  DWORD f85;
  DWORD f86;
  DWORD f87;
  DWORD f88;
  DWORD f89;
  DWORD f90;
  DWORD f91;
  DWORD f92;
  DWORD f93;
  DWORD f94;
  DWORD f95;
  DWORD f96;
  DWORD f97;
  DWORD f98;
  DWORD f99;
  DWORD f100;
  DWORD f101;
  DWORD f102;
  DWORD f103;
  DWORD f104;
  DWORD f105;
  DWORD f106;
  DWORD f107;
  DWORD f108;
  DWORD f109;
  DWORD f110;
  DWORD f111;
  DWORD f112;
  DWORD f113;
  DWORD f114;
  DWORD f115;
  DWORD f116;
  DWORD f117;
  DWORD f118;
  DWORD f119;
  DWORD f120;
  DWORD f121;
  DWORD f122;
  DWORD f123;
  DWORD f124;
  DWORD f125;
  DWORD f126;
  DWORD f127;
  DWORD f128;
  DWORD f129;
  DWORD f130;
  DWORD f131;
  DWORD f132;
  DWORD f133;
  DWORD f134;
  DWORD f135;
  DWORD f136;
  DWORD f137;
  DWORD f138;
  DWORD f139;
  DWORD f140;
  DWORD f141;
  DWORD f142;
  DWORD f143;
  DWORD f144;
  DWORD f145;
  DWORD f146;
  DWORD f147;
  DWORD f148;
  DWORD f149;
  DWORD f150;
  DWORD f151;
  DWORD f152;
  DWORD f153;
  DWORD f154;
  DWORD f155;
  DWORD f156;
  DWORD f157;
  DWORD f158;
  DWORD f159;
  DWORD f160;
  DWORD f161;
  DWORD f162;
  DWORD f163;
  DWORD f164;
  DWORD f165;
  DWORD f166;
  DWORD f167;
  DWORD f168;
  DWORD f169;
  DWORD f170;
  DWORD f171;
  DWORD f172;
  DWORD f173;
  DWORD f174;
  DWORD f175;
  DWORD f176;
  DWORD f177;
  DWORD f178;
  DWORD f179;
  DWORD f180;
  DWORD f181;
  DWORD f182;
  DWORD f183;
  DWORD f184;
  DWORD f185;
  DWORD f186;
  DWORD f187;
  DWORD f188;
  DWORD f189;
  DWORD f190;
  DWORD f191;
  DWORD f192;
  DWORD f193;
  DWORD f194;
  DWORD f195;
  DWORD f196;
  DWORD f197;
  DWORD f198;
  DWORD f199;
  DWORD f200;
  DWORD f201;
  DWORD f202;
  DWORD f203;
  DWORD f204;
  DWORD f205;
  DWORD f206;
  DWORD f207;
  DWORD f208;
  DWORD f209;
  DWORD f210;
  DWORD f211;
  DWORD f212;
  DWORD f213;
  DWORD f214;
};

struct LHNetUser
{
  DWORD id;
  DWORD unk1;
  DWORD unk2;
  wchar_t name[48];
  wchar_t unk18;
  wchar_t password[24];
  wchar_t unk20;
  DWORD unk21;
  DWORD unk22;
  DWORD unk23;
  DWORD unk24;
  DWORD unk25;
};

struct LHNetBase
{
  LHLobby *lhLobby;
  DWORD unk1;
  DWORD unk2;
  DWORD unk3;
  DWORD unk4;
  DWORD unk5;
  DWORD unk6;
  DWORD unk7;
  DWORD unk8;
  LHSession *lhSession;
  DWORD unk10;
  LHNetUser *lhNetUser;
  DWORD unk12;
  DWORD unk13;
  DWORD unk14;
  DWORD unk15;
};

struct GNetwork
{
	DWORD unk0;
	DWORD unk1;
	DWORD unk2;
	DWORD unk3;
	LHSession* lhSession;
	DWORD unk5[56];
};


