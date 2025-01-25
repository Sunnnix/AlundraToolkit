#define _CRT_SECURE_NO_WARNINGS
#include "setupAndLoop.h"
#include <cstdio>
#include <string.h>

// Globals
unsigned char CD_MODE_PARAM;
int FLAG_RESOURCES_LOADED;
int PAD_BUFFER_0;
int PAD_BUFFER_1;
int DAT_801dda54;
int DAT_801dda3c;
int DAT_801dda58;
int DAT_801dda40;
int DAT_801dda5c;
int DAT_801dda44;
int DAT_80121b59;
int DAT_80121b60;
int DAT_80121b68;
int DAT_80121b64;
int DAT_8009c6c0;
int DAT_800c667c;
int DAT_801dda48;
int DAT_801dda60;
int CD_STATUS_DOUBLE_SPEED;
int DAT_801ef420; // maybe no int? something to do with DATAS.BIN string
int DATAS_BIN_FILE_SIZE;
int DAT_801ef42c;
int DAT_801ef428;
int DAT_801ef424;
int DAT_801f00a0;
int SOUND_BIN_FILE_SIZE;
int DAT_801f00ac;
int DAT_801f00a8;
int DAT_801f00a4;
int SemiTransparanceRate;
int DAT_801ac6e8;
int DAT_801f2e36;
int DAT_801f2e62;
int DAT_801f2e8e;
int DAT_801f2eba;
int DAT_8012e3c0;
int DAT_801f5358;
int DAT_801f11fc;
undefined* ETC_USA_DATA_0;
undefined* ETC_USA_DATA_1;
undefined* ETC_USA_DATA_2;
undefined1 DAT_800295b2;
int DAT_800295b0;
int DAT_801d7cb0;
int DAT_801d7cb4;
int drawEnv_clip;
int DAT_801d7cd4;
int DAT_801d7cd8;
int DAT_801d7cdc;
int drawEnv_color;
int DAT_801d7cc8;
int DAT_801d7ccc;
void* DAT_801d7ce0; // function pointer?
void* DAT_801d7ce4; // function pointer?
int DAT_801d7ba0;
int DAT_801d7c24;
int* DAT_801d7ca8;
int* DAT_801d7cac;
ULONG RenderOrderTables;
ULONG RenderOrderTables2;
ULONG* RenderOrderTablesPtr;
ULONG* RenderOrderTablesPtr2;
int DAT_801e70b0;
int Texpages;
int* TexPagesPtr;
int SpritePrimitives; // maybe array of 4
int texturePage; // DR_TPAGE?
undefined1 TexTableSpritePos;
int DAT_801c67f8;
ULONG DATAS_BIN_DATA_SET_0; // Huge data set, probably large array of some object (TODO try to figure out the size of this data set)
int DATAS_BIN_DATA_SET_0_SIZE;
int* EntitySpriteTable;
ULONG* DAT_801f395c; // image content?
ULONG DAT_801536c0;
ULONG SOURCE_ADDRESS;
ULONG* DAT_801248f8;
char* DAT_801248fc;
int	DAT_80124900;
int DAT_80133990;
int DAT_801f4054;
undefined DAT_801c47dc;
int DAT_801c67dc;
int DAT_801f2e3c;
short DAT_80137990; // begin of short array?
int DAT_801d93ec;
int DAT_801ac6f8;
int DAT_801ddac0; // some type of flag

int cdMode;
int tmpData; // tmpData may be to small

LONG PRINT_STREAM_ID;
unsigned short TexturePageIDs;
unsigned short DAT_8012e2a0;
unsigned short* DAT_801e6370;

int DATAS_BIN_HEADER[494]; // 0x7B8 / 4

ULONG ETC_USA_LANG_DATA;
ULONG* ETC_USA_LANG_DATA_PTR;

// undefined functions?
void* LAB_8002bb74;
void* LAB_8002bafc;

// File Pointers
PSX::CdlLOC DATAS_BIN_FILE_START;
PSX::CdlLOC SOUND_BIN_FILE_START;
int ARAN_XA_XA_FILE;

// Events
LONG EVENT_0;
LONG EVENT_1;
LONG EVENT_2;
LONG EVENT_3;
LONG EVENT_4;
LONG EVENT_5;
LONG EVENT_6;
LONG EVENT_7;

// Strings
char s_DATA_DATAS_BIN_800266a8[] = "\\DATA\\DATAS.BIN;1"; // \\DATA\\DATAS.BIN;1
char s_DATA_SOUND_BIN_800266bc[] = "\\DATA\\SOUND.BIN;1"; // \\DATA\\SOUND.BIN;1
char s_ARAN_XA_XA[] = "\\ARAN_XA.XA;1"; // \\ARAN_XA.XA;1
char s_DATA_DATAS_BIN_800c7dd4[] = "\\DATA\\DATAS.BIN"; // \\DATA\\DATAS.BIN
char s_800266d8[] = "\\%s.;1"; // \\%s.;1
char s_800266d0[] = "\\%s;1"; // \\%s;1
char s_Etc_USA_R[]  = "DATA\\Etc_USA.R"; // DATA\\Etc_USA.R
char s_BIZEN_80026b04[] = "BIZEN"; // BIZEN
undefined* PTR_s_BIZEN_800c6274 = (undefined*) s_BIZEN_80026b04; // maybe a pointer to the string pointer of BIZEN to loop through all string names, as they are aligned behind each other

void setupDataAndLoop() {
	unsigned short texturePageID;
	//undefined4 uVar1;
	int* dbh_10;
	int y;
	int* dbh_11;
	unsigned short* puVar2;
	int x;
	int iVar3;
	int semiTranspRate_CLUTX;
	unsigned short* puVar4;
	int iVar5;
	int iVar6;
	int local_30;

	PSX::VSync(0);
	iVar6 = 0;
	PSX::ResetCallback();
	PSX::ResetGraph(3);
	iVar5 = 0;
	// Original CdInit() without params (maybe because it should ever be 0?, that's how it is described in the documentation)
	// According to the documentation it has no return value, but a return value has been described and one is returned in the original.
	PSX::CdInit(0);
	semiTranspRate_CLUTX = 0;
	SetupAudio();
	SetupPad();
	LinkResources();
	SetupCardAndEvents();
	PSX::FntLoad(960, 256);
	PRINT_STREAM_ID = PSX::FntOpen(8, 32, 304, 192, 0, 1024);
	PSX::SetDumpFnt(PRINT_STREAM_ID);
	puVar4 = &TexturePageIDs;
	local_30 = 0;
	
	// Setup TPages?
	do {
		iVar3 = 0;
		y = 0;
		do {
			x = 320;
			puVar2 = puVar4 + iVar3;
			do {
				texturePageID = PSX::GetTPage(0, semiTranspRate_CLUTX, x, y);
				*puVar2 = texturePageID;
				x = x + 64;
				puVar2 = puVar2 + 1;
				iVar3 = iVar3 + 1;
			} while (x < 1024);
			y = y + 256;
		} while (y < 512);
		puVar4 = puVar4 + 22;
		semiTranspRate_CLUTX = semiTranspRate_CLUTX + 1;
		y = 0;
	} while (semiTranspRate_CLUTX < 4);
	semiTranspRate_CLUTX = 0;

	// Setup Plaetts?
	do {
		iVar3 = 0x1e0;
		puVar2 = &DAT_8012e2a0 + y;
		do {
			texturePageID = PSX::GetClut(semiTranspRate_CLUTX, iVar3);
			*puVar2 = texturePageID;
			iVar3 = iVar3 + 1;
			puVar2 = puVar2 + 1;
			y = y + 1;
		} while (iVar3 < 512);
		semiTranspRate_CLUTX = semiTranspRate_CLUTX + 64;
	} while (semiTranspRate_CLUTX < 320);
	
	ReadDataFromCD(s_DATA_DATAS_BIN_800c7dd4, (ULONG*) DATAS_BIN_HEADER, 0, 0x7b8);
	semiTranspRate_CLUTX = 0;
	dbh_10 = DATAS_BIN_HEADER + 10;
	dbh_11 = DATAS_BIN_HEADER + 11;
	SemiTransparanceRate = 0;
	DAT_801ac6e8 = 0;
	// Todo what's happening here
	do {
		if (DAT_801ac6e8 < *dbh_11 - *dbh_10) {
			SemiTransparanceRate = semiTranspRate_CLUTX;
			DAT_801ac6e8 = *dbh_11 - *dbh_10;
		}
		dbh_10 = dbh_10 + 1;
		semiTranspRate_CLUTX = semiTranspRate_CLUTX + 1;
		dbh_11 = dbh_11 + 1;
	} while (semiTranspRate_CLUTX < 0x1e3);

	LoadLanguageTexts();
	drawingStuff(&LAB_8002bb74, &LAB_8002bafc);
	SetupGlobalROTs();
	SetupSpriteEnvironment(DATAS_BIN_HEADER[5]);
	LoadSpriteTable(s_DATA_DATAS_BIN_800c7dd4, DATAS_BIN_HEADER[0], DATAS_BIN_HEADER[1], DATAS_BIN_HEADER[2],
		DATAS_BIN_HEADER[3]);
	LoadBalanceFile(); // original LoadBalanceFile(0, 0); but the function has no params
	FUN_8002aaf8();
	FUN_80042f24(s_DATA_DATAS_BIN_800c7dd4, DATAS_BIN_HEADER[4]);
	FUN_80043584();
	FUN_80044f50();
	FUN_80045b58();
	FUN_80049464();
	FUN_8005f78c(832, 256, 256, 496, DAT_801f2e36, DAT_801f2e62, DAT_801f2e8e, DAT_801f2eba, &DAT_8012e3c0);
	DAT_801f5358 = ~DAT_801f11fc;
}

void SetupAudio() {
	PSX::ResetCallback();
	PSX::SsUtReverbOff();
	SetupAudio2();
}

void SetupAudio2() {
	// TODO analyse code
}

void SetupPad() {
	PSX::PadInit(0);
	// TODO analyse variables
	PAD_BUFFER_1 = 0x14;
	PAD_BUFFER_0 = 0x14;
	DAT_801dda54 = 0;
	DAT_801dda3c = 0;
	FUN_8002e670();
}

void FUN_8002e670() {
	// TODO analyse variables
	int* puVar1;
	int* puVar2;
	int iVar3;

	iVar3 = 0;
	puVar2 = &DAT_801dda48;
	puVar1 = &DAT_801dda60;
	DAT_801dda58 = 0;
	DAT_801dda40 = 0;
	DAT_801dda5c = 0;
	DAT_801dda44 = 0;
	do {
		*puVar1 = 0;
		*puVar2 = 0;
		puVar2 = puVar2 + 1;
		iVar3 = iVar3 + 1;
		puVar1 = puVar1 + 1;
	} while (iVar3 < 4);
}

void LinkResources() {
	int cdc_success;
	PSX::CdlFILE* pCVar1;

	PSX::CdSetDebug(0);
	CD_MODE_PARAM = 0x80;
	DAT_80121b59 = 0;
	do {
		// Set basic mode with 0x80
		cdc_success = PSX::CdControlB('\x0e', &CD_MODE_PARAM, (unsigned char*)0x0);
	} while (cdc_success == 0);
	DAT_80121b60 = 0;
	DAT_80121b68 = 0;
	DAT_80121b64 = 0;
	do {
		pCVar1 = PSX::CdSearchFile((PSX::CdlFILE*)&DATAS_BIN_FILE_START, s_DATA_DATAS_BIN_800266a8);
	} while (pCVar1 == (PSX::CdlFILE*)0x0);
	do {
		pCVar1 = PSX::CdSearchFile((PSX::CdlFILE*)&SOUND_BIN_FILE_START, s_DATA_SOUND_BIN_800266bc);
	} while (pCVar1 == (PSX::CdlFILE*)0x0);
	FLAG_RESOURCES_LOADED = 1;
	LoadXASoundsAndSetupAudio();
}

int LoadXASoundsAndSetupAudio() {
	PSX::CdlFILE* pCVar1;
	PSX::CdlFILE loc_ARAN_XA_XA_FILE_START;
	PSX::SpuCommonAttr soundAttr;

	if ((DAT_8009c6c0 != 0) || ((FLAG_RESOURCES_LOADED != 0 && (DAT_800c667c == 0)))) {
		do {
			pCVar1 = CdSearchFile(&loc_ARAN_XA_XA_FILE_START, s_ARAN_XA_XA);
		} while (pCVar1 == (PSX::CdlFILE*)0x0);
		ARAN_XA_XA_FILE = PSX::CdPosToInt(&loc_ARAN_XA_XA_FILE_START.pos);
		soundAttr.mask = 0x2ec0;
		soundAttr.cd.volume.left = 0x7fff;
		soundAttr.cd.volume.right = 0x7fff;
		soundAttr.cd.mix = 1;
		soundAttr.ext.volume.left = 0x7fff;
		soundAttr.ext.volume.right = 0x7fff;
		soundAttr.ext.mix = 1;
		PSX::SpuSetCommonAttr(&soundAttr);
		PSX::CdControlB(0xb, (unsigned char*)0x0, (unsigned char*)0x0);
	}
	return 1;
}

void SetupCardAndEvents() {
	PSX::InitCARD(1);
	PSX::StartCARD();
	PSX::_bu_init();
	SetupEvents();
	PSX::_card_auto(0);
}

void SetupEvents() {
	PSX::EnterCriticalSection();
	EVENT_0 = PSX::OpenEvent(0xf4000001, 4, 0x2000, 0);
	EVENT_1 = PSX::OpenEvent(0xf4000001, 0x8000, 0x2000, 0);
	EVENT_2 = PSX::OpenEvent(0xf4000001, 0x100, 0x2000, 0);
	EVENT_3 = PSX::OpenEvent(0xf4000001, 0x2000, 0x2000, 0);
	EVENT_4 = PSX::OpenEvent(0xf0000011, 4, 0x2000, 0);
	EVENT_5 = PSX::OpenEvent(0xf0000011, 0x8000, 0x2000, 0);
	EVENT_6 = PSX::OpenEvent(0xf0000011, 0x100, 0x2000, 0);
	EVENT_7 = PSX::OpenEvent(0xf0000011, 0x2000, 0x2000, 0);
	PSX::EnableEvent(EVENT_0);
	PSX::EnableEvent(EVENT_1);
	PSX::EnableEvent(EVENT_2);
	PSX::EnableEvent(EVENT_3);
	PSX::EnableEvent(EVENT_4);
	PSX::EnableEvent(EVENT_5);
	PSX::EnableEvent(EVENT_6);
	PSX::EnableEvent(EVENT_7);
	PSX::ExitCriticalSection();
}

void ReadDataFromCD(char* filename, ULONG* readData, unsigned int offset, int bytesToRead) {
	char* fileExt;
	int resFilePtr;
	unsigned int uVar1;
	int tempPos;
	PSX::CdlLOC resFileCDLoc;
	int fileSizeInSectors = 0; // don't know what this variable is used for, it is never written
	char ffilename[264];
	int* tmpPtr;

	SetCD_DOUBLE_SPEED_MODE();
	StrToUpper(filename);
	fileExt = strrchr(filename, '.');
	if (fileExt == (char*)0x0)
		fileExt = s_800266d8; //"\\%s.;1";
	else
		fileExt = s_800266d0; //"\\%s;1";
	sprintf(ffilename, fileExt, filename);
	if ((offset & 0x7ff) != 0) {
		do {
			/* WARNING: Do nothing block with infinite loop */
		} while (true);
	}
	do {
		resFilePtr = (int) getResourceFile((PSX::CdlFILE*)&resFileCDLoc, ffilename); // casting CdlLOC to CdlFILE seems wrong
		if (resFilePtr != 0) {
			if (bytesToRead == -1)
				bytesToRead = fileSizeInSectors;
			uVar1 = offset;
			if ((int)offset < 0) {
				uVar1 = offset + 2047;
			}
			resFilePtr = bytesToRead;
			if (bytesToRead < 0) {
				resFilePtr = bytesToRead + 2047;
			}
			resFilePtr = resFilePtr >> 11;
			tempPos = CdPosToInt(&resFileCDLoc);
			PSX::CdIntToPos(tempPos + ((int)uVar1 >> 11), &resFileCDLoc);
			if (bytesToRead < 2048) {
			LAB_80050374:
				if (bytesToRead == 0)
					return;
				resFilePtr = PSX::CdControl(2, &resFileCDLoc.minute, (unsigned char*)0x0);
				if ((resFilePtr != 0) && (resFilePtr = PSX::CdRead(1, (ULONG*)&tmpData, cdMode), resFilePtr != 0) // tmpData type may be to small, because it should be a buffer
					) {
					do {
						resFilePtr = PSX::CdReadSync(1, (unsigned char*)0x0);
						if (resFilePtr < 1) {
							resFilePtr = 0;
							if (bytesToRead < 1) {
								return;
							}
							do {
								tmpPtr = &tmpData + resFilePtr;
								resFilePtr = resFilePtr + 1;
								*(int*)readData = *tmpPtr;
								readData = (ULONG*)((int)readData + 1);
							} while (resFilePtr < bytesToRead);
							return;
						}
					} while (resFilePtr != -1);
				}
			}
			else {
				tempPos = PSX::CdControl('\x02', &resFileCDLoc.minute, (unsigned char*)0x0);
				if ((tempPos != 0) && (tempPos = PSX::CdRead(resFilePtr, readData, cdMode), tempPos != 0)) {
					do {
						tempPos = PSX::CdReadSync(1, (unsigned char*)0x0);
						if (tempPos < 1) {
							bytesToRead = bytesToRead + resFilePtr * -2048;
							readData = readData + resFilePtr * 512;
							tempPos = CdPosToInt(&resFileCDLoc);
							CdIntToPos(tempPos + resFilePtr, &resFileCDLoc);
							if (2047 < bytesToRead) {
								return;
							}
							goto LAB_80050374;
						}
					} while (tempPos != -1);
				}
			}
		}
		PSX::VSync(60);
	} while (true);
}

void SetCD_DOUBLE_SPEED_MODE() {
	int cdc_success;
	unsigned char local_10[8];

	if (((DAT_8009c6c0 != 0) || ((FLAG_RESOURCES_LOADED != 0 && (DAT_800c667c == 0)))) &&
		(CD_STATUS_DOUBLE_SPEED != 0)) {
		do {
			cdc_success = PSX::CdControlB(0xb, (unsigned char*)0x0, (unsigned char*)0x0);
		} while (cdc_success == 0);
		do {
			cdc_success = PSX::CdControlB(0x9, (unsigned char*)0x0, (unsigned char*)0x0);
		} while (cdc_success == 0);
		local_10[0] = 0x80; // double speed argument
		do {
			cdc_success = PSX::CdControlB(0xe, local_10, (unsigned char*)0x0);
		} while (cdc_success == 0);
		CD_STATUS_DOUBLE_SPEED = 0;
	}
	return;
}

void StrToUpper(char* text) {
	char tmp;

	tmp = *text;
	while (tmp != 0) {
		if (tmp - 0x61 < 0x1a)
			*text = tmp - 0x20;
		text = text + 1;
		tmp = *text;
	}
	return;
}

PSX::CdlFILE* getResourceFile(PSX::CdlFILE* resource_file, char* resourceName) {
	int uVar1;
	int uVar2;
	int compareResult;
	ULONG fileSize;

	SetCD_DOUBLE_SPEED_MODE();
	compareResult = strcmp(s_DATA_DATAS_BIN_800266a8, resourceName);
	uVar1 = DAT_801ef420;
	fileSize = DATAS_BIN_FILE_SIZE;
	if (compareResult == 0) {
		resource_file->pos = DATAS_BIN_FILE_START;
		resource_file->size = fileSize;
		*(char*)resource_file->name = uVar1;
		uVar2 = DAT_801ef42c;
		uVar1 = DAT_801ef428;
		*(char*)(resource_file->name + 4) = DAT_801ef424;
		*(char*)(resource_file->name + 8) = uVar1;
		*(char*)(resource_file->name + 12) = uVar2;
	} else {
		compareResult = strcmp(s_DATA_SOUND_BIN_800266bc, resourceName);
		uVar1 = DAT_801f00a0;
		fileSize = SOUND_BIN_FILE_SIZE;
		if (compareResult == 0) {
			resource_file->pos = SOUND_BIN_FILE_START;
			resource_file->size = fileSize;
			*(char*)resource_file->name = uVar1;
			uVar2 = DAT_801f00ac;
			uVar1 = DAT_801f00a8;
			*(char*)(resource_file->name + 4) = DAT_801f00a4;
			*(char*)(resource_file->name + 8) = uVar1;
			*(char*)(resource_file->name + 0xc) = uVar2;
		} else
			resource_file = CdSearchFile(resource_file, resourceName);
	}
	return resource_file;
}

ULONG* getGlobal_ETC_USA_DATA_VAR() { return &ETC_USA_LANG_DATA; };

int getOffsetPosOf_ETC_USA_LANG_DATA(int offset) {
	ETC_USA_LANG_DATA_PTR = &ETC_USA_LANG_DATA;
	return (int)&ETC_USA_LANG_DATA + (unsigned int) * (unsigned short*)((int)&ETC_USA_LANG_DATA + offset * 2);
}

void LoadLanguageTexts() {
	short* psVar1;
	ULONG* fileDataPtr;
	undefined** ptrLoc2;
	undefined4 uVar2;
	undefined* tmpPtrLoc;
	undefined1* puVar3;
	int currentSubPos;
	int currentMainPos;
	undefined1* puVar4;
	undefined** ptrLoc3;

	fileDataPtr = getGlobal_ETC_USA_DATA_VAR();
	ReadDataFromCD(s_Etc_USA_R, fileDataPtr, 0, 0x2800); // 0x2800 basically the whole file
	currentSubPos = 0;
	ptrLoc2 = &ETC_USA_DATA_1;
	ptrLoc3 = &ETC_USA_DATA_2;
	currentMainPos = 0;
	// Get texts headers from ETC File ?
	do {
		uVar2 = getOffsetPosOf_ETC_USA_LANG_DATA(currentSubPos + 512);
		*(undefined4*)((int)&ETC_USA_DATA_0 + currentMainPos) = uVar2;
		tmpPtrLoc = (undefined*)getOffsetPosOf_ETC_USA_LANG_DATA(currentSubPos + 640);
		*ptrLoc2 = tmpPtrLoc;
		tmpPtrLoc = (undefined*)getOffsetPosOf_ETC_USA_LANG_DATA(currentSubPos + 768);
		*ptrLoc3 = tmpPtrLoc;
		ptrLoc3 = ptrLoc3 + 2;
		ptrLoc2 = ptrLoc2 + 2;
		currentSubPos = currentSubPos + 1;
		currentMainPos = currentMainPos + 8;
	} while (currentSubPos < 98);
	currentSubPos = 0;
	currentMainPos = 0;
	puVar4 = &DAT_800295b2;
	// 
	while (true) {
		tmpPtrLoc = (undefined*)getOffsetPosOf_ETC_USA_LANG_DATA(currentSubPos);
		puVar3 = puVar4;
		do {
			*puVar3 = *tmpPtrLoc;
			puVar3 = puVar3 + 1;
			tmpPtrLoc = tmpPtrLoc + 1;
		} while ((int)puVar3 < (int)(puVar4 + 0x20));
		psVar1 = (short*)((int)&DAT_800295b0 + currentMainPos);
		currentMainPos = currentMainPos + 0x22;
		if (*psVar1 == -1) break;
		puVar4 = puVar4 + 0x22;
		currentSubPos = currentSubPos + 1;
	}
	currentSubPos = 0;
	ptrLoc2 = &PTR_s_BIZEN_800c6274;
	// Get names from ETC File ? (256 bytes â name)
	do {
		tmpPtrLoc = (undefined*)getOffsetPosOf_ETC_USA_LANG_DATA(currentSubPos + 0x100);
		*ptrLoc2 = tmpPtrLoc;
		currentSubPos = currentSubPos + 1;
		ptrLoc2 = ptrLoc2 + 1;
	} while (currentSubPos < 0x100);
}

void InitDrawAndDispEnv(undefined2* param_1, int param_2, int param_3, int param_4, int param_5) {
	*param_1 = (short)param_2;
	param_1[1] = (short)param_3;
	param_1[2] = 0x140;
	param_1[3] = 0xf0;
	param_1[4] = (short)param_4;
	param_1[6] = 0x140;
	param_1[7] = 0xf0;
	param_1[5] = (short)param_5;
	PSX::SetDefDrawEnv((PSX::DRAWENV*)(param_1 + 8), param_2, param_3, 0x140, 0xf0);
	PSX::SetDefDispEnv((PSX::DISPENV*)(param_1 + 0x36), param_4, param_5, 0x140, 0xf0);
	*(undefined*)(param_1 + 0x14) = 1;
}

void FUN_800430c8() {
	short* psVar1;

	// TODO

	//psVar1 = DAT_801d7cac;
	//PSX::PutDispEnv((DISPENV*)(DAT_801d7cac + 0x36));
	//*(char*)((int)psVar1 + 0x29) = (char)drawEnv_color;
	//*(char*)(psVar1 + 0x15) = (char)DAT_801d7cc8;
	//*(char*)((int)psVar1 + 0x2b) = (char)DAT_801d7ccc;
	//psVar1[8] = *psVar1 + (short)drawEnv_clip;
	//psVar1[9] = psVar1[1] + (short)DAT_801d7cd4;
	//psVar1[10] = (short)DAT_801d7cd8;
	//psVar1[0xb] = (short)DAT_801d7cdc;
	//PSX::PutDrawEnv((DRAWENV*)(psVar1 + 8));
	//DAT_801d7cac = DAT_801d7ca8;
	//DAT_801d7ca8 = psVar1;
	//DAT_801d7cb0 = DAT_801d7cb0 + 1;
}

void OnDrawCompleteCallback() {
	int rootCounter;

	rootCounter = PSX::GetRCnt(0xf2000001);
	if (*(int*)(DAT_801d7cac + 0x80) < rootCounter) {
		*(int*)(DAT_801d7cac + 0x80) = rootCounter;
	}
}

void drawingStuff(/*undefined4*/ void* param_1, /*undefined4*/ void* param_2) {
	DAT_801d7cb0 = 0;
	DAT_801d7cb4 = 0;
	drawEnv_clip = 0;
	DAT_801d7cd4 = 0;
	DAT_801d7cd8 = 0x140;
	DAT_801d7cdc = 0xf0;
	drawEnv_color = 0;
	DAT_801d7cc8 = 0;
	DAT_801d7ccc = 0;
	DAT_801d7ce0 = param_1;
	DAT_801d7ce4 = param_2;
	//InitDrawAndDispEnv(&DAT_801d7ba0, 0, 0, 0, 0xf0, 0x140, 0xf0); // (too many arguments) why Ghidra... why? TODO
	//InitDrawAndDispEnv(&DAT_801d7c24, 0, 0xf0, 0, 0, 0x140, 0xf0);
	DAT_801d7ca8 = &DAT_801d7ba0;
	DAT_801d7cac = &DAT_801d7c24;
	PSX::VSync(0);
	FUN_800430c8();
	PSX::SetRCnt(0xf2000001, 0xffff, 0x2010);
	PSX::DrawSyncCallback(OnDrawCompleteCallback);
}

void SetupRenderOrderTables(ULONG *param_1) {
	PSX::ClearOTag(param_1 + 4, 0x3c0);
	PSX::ClearOTag(param_1 + 2, 1);
	PSX::ClearOTag(param_1 + 3, 1);
	PSX::ClearOTag(param_1, 1);
	PSX::ClearOTag(param_1 + 1, 1);
}

void SetupGlobalROTs() {
	RenderOrderTablesPtr = &RenderOrderTables;
	RenderOrderTablesPtr2 = &RenderOrderTables2;
	//SetupRenderOrderTables(); // this function is called twice, first without params?
	SetupRenderOrderTables(RenderOrderTablesPtr2);
}

void SetupSpriteEnvironment(/*undefined4*/ int param_1) {
	undefined1* puVar1;
	undefined1* TextTableSpritePosPtr;
	PSX::SPRT* spritePrimitive;
	PSX::DR_TPAGE* texturePagePtr;
	int j;
	int locOffset;
	int groupOffset;
	int i;

	i = 0;
	groupOffset = 0;
	DAT_801e6370 = &DAT_8012e2a0;
	TexPagesPtr = &Texpages;
	DAT_801e70b0 = param_1;
	do {
		j = 0;
		locOffset = 0;
		do {
			spritePrimitive = (PSX::SPRT*)(&SpritePrimitives + groupOffset + locOffset);
			PSX::SetSprt(spritePrimitive);
			PSX::SetShadeTex(spritePrimitive, 1);
			PSX::SetSemiTrans(spritePrimitive, 0);
			locOffset = locOffset + 20;
			j = j + 1;
			spritePrimitive->w = 24;
			spritePrimitive->h = 16;
			spritePrimitive->r0 = 128;
			spritePrimitive->g0 = 128;
			spritePrimitive->b0 = 128;
		} while (j < 600);
		i = i + 1;
		groupOffset = groupOffset + 13600;
	} while (i < 2);
	texturePagePtr = (PSX::DR_TPAGE*)&texturePage;
	groupOffset = 0;
	do {
		i = groupOffset + 1;
		PSX::SetDrawTPage(texturePagePtr, 0, 0, (unsigned int)(unsigned short)TexPagesPtr[groupOffset]);
		texturePagePtr = texturePagePtr + 1;
		groupOffset = i;
	} while (i < 6);
	TextTableSpritePosPtr = &TexTableSpritePos;
	i = 0;
	groupOffset = 0;
	do {
		do {
			puVar1 = TextTableSpritePosPtr + 2;
			j = 0;
			do {
				*TextTableSpritePosPtr = (char)i;
				puVar1[-1] = (char)j;
				*puVar1 = (char)groupOffset;
				puVar1 = puVar1 + 3;
				locOffset = j + 48;
				TextTableSpritePosPtr = TextTableSpritePosPtr + 3;
				j = j + 24;
			} while (locOffset < 257);
			groupOffset = groupOffset + 16;
		} while (groupOffset < 256);
		i = i + 1;
		groupOffset = 0;
	} while (i < 6);
}

void ConvertDataForVRAM(int** outArray, int* rawData) {
	// TODO
}

void ValidateClut(unsigned int param_1, int param_2) {
	if (((319 < (int)param_1) || ((param_1 & 63) != 0)) || (31 < param_2 - 480U)) {
		//ShowError("Clut Trans error!\r\n");
		//exit();
	}
}

void CheckAndRepositionCoords(int* posX, int* posY, int height) {
	int tmp;

	tmp = *posY;
	*posY = tmp + height;
	if (511 < tmp + height) {
		*posY = 480;
		tmp = *posX;
		*posX = tmp + 64;
		ValidateClut(tmp + 64, *posY);
	}
}

void LoadImageToVRAM(ULONG* image_content, undefined4 posX, int posY, int minHeight) {
	int height;
	undefined4 x;
	int y[2];
	PSX::RECT rect;

	x = posX;
	y[0] = posY;
	// ValidateClut(); no params passed?
	if (0 < minHeight) {
		do {
			height = 512 - y[0];
			if (minHeight < 512 - y[0]) {
				height = minHeight;
			}
			minHeight = minHeight - height;
			rect.x = (short)x;
			rect.w = 16;
			rect.h = (short)height;
			rect.y = (short)y[0];
			PSX::LoadImage(&rect, image_content);
			PSX::DrawSync(0);
			image_content = image_content + height * 8;
			CheckAndRepositionCoords(&x, y, height);
		} while (0 < minHeight);
	}
}

void FUN_80083680(ULONG* param_1, char* param_2, int param_3) {
	DAT_801248f8 = param_1;
	DAT_801248fc = param_2;
	DAT_80124900 = param_3;
	return;
}

void FUN_800836a0(ULONG* param_1) {
	DAT_801248f8 = param_1;
	return;
}

int FUN_800836b0(){
	char cVar1;
	int iVar2;
	unsigned int uVar3;
	char* pcVar4;
	int iVar5;

	iVar5 = 0;
	if (DAT_80124900 < 1) {
		return 0;
	}
	do {
		cVar1 = *DAT_801248fc;
		pcVar4 = DAT_801248fc + 1;
		iVar2 = DAT_80124900 + -1;
		if (cVar1 == -0x53) {
			pcVar4 = DAT_801248fc + 2;
			iVar2 = DAT_80124900 + -2;
			if ((char)DAT_801248fc[1] == 0) goto LAB_80083734;
			uVar3 = (unsigned int)(char)DAT_801248fc[2];
			DAT_80124900 = DAT_80124900 + -3;
			if (uVar3 == 0) {
				DAT_801248fc = DAT_801248fc + 3;
				DAT_80124900 = 0;
				return iVar5;
			}
			//pcVar4 = (char*)(DAT_801248f8 + -(unsigned int)(char)DAT_801248fc[1]); // TODO check negating unsigned value
			DAT_801248fc = DAT_801248fc + 3;
			while (uVar3 = uVar3 - 1, uVar3 != 0xffffffff) {
				cVar1 = *pcVar4;
				pcVar4 = pcVar4 + 1;
				*DAT_801248f8 = cVar1;
				DAT_801248f8 = DAT_801248f8 + 1;
				iVar5 = iVar5 + 1;
			}
		}
		else {
		LAB_80083734:
			DAT_80124900 = iVar2;
			DAT_801248fc = pcVar4;
			iVar5 = iVar5 + 1;
			pcVar4 = (char*)DAT_801248f8 + 1;
			*DAT_801248f8 = cVar1;
			DAT_801248f8 = (ULONG*)pcVar4;
		}
		if (DAT_80124900 < 1) {
			return iVar5;
		}
		if (0x7fff < iVar5) {
			return iVar5;
		}
	} while (true);
}

void FUN_800837f8(void) {
	PSX::RECT local_10;

	local_10.w = 0x140;
	local_10.x = 0;
	local_10.y = 0;
	local_10.h = 0x1e0;
	PSX::ClearImage(&local_10, 0xfa, '2', '2');
	PSX::DrawSync(0);
	PSX::VSync(0);
	return;
}

int FUN_8008384c(short param_1, short param_2, ULONG* param_3, int param_4, ULONG* param_5) {
	int iVar1;
	int iVar2;
	int iVar3;
	PSX::RECT local_30;

	*(undefined*)(param_5 + 0x2000) = 0xfe;
	iVar2 = 0;
	if ((*(char*)param_3 == 'E') && (*(char*)((int)param_3 + 1) == 'Z')) {
		iVar3 = param_4 * 0x8000 + -6;
		FUN_80083680(param_5, (char*)((int)param_3 + 6), iVar3); // probably PSX::StSetMask
		do {
			FUN_800836a0(param_5); // Possible PSX::GsSetNearClip, PSX::GsSetFarClip, PSX::GsSetWorkBase
			PSX::DrawSync(0);
			iVar1 = FUN_800836b0();
			if (iVar1 == 0) break;
			iVar2 = iVar2 + iVar1;
			local_30.w = 0x40;
			local_30.h = 0x100;
			local_30.x = param_1;
			local_30.y = param_2;
			LoadImage(&local_30, param_5);
			param_1 = param_1 + 0x40;
		} while (iVar2 < iVar3);
	}
	else {
		do {
			local_30.w = 0x40;
			local_30.h = 0x100;
			local_30.x = param_1;
			local_30.y = param_2;
			LoadImage(&local_30, param_3);
			iVar2 = iVar2 + 0x8000;
			param_3 = param_3 + 0x2000;
			param_1 = param_1 + 0x40;
		} while (iVar2 < param_4 * 0x8000);
	}
	PSX::DrawSync(0);
	if (*(char*)(param_5 + 0x2000) != -2) {
		printf("Memory Burst !!\n");
		FUN_800837f8();
	}
	return iVar2;
}

void FUN_800839d8(ULONG* param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, ULONG* param_5) {
	FUN_8008384c(param_2, param_3, param_1, param_4, param_5);
}

void FUN_8003cb60() {
	int iVar1;
	int* piVar2;

	iVar1 = 0;
	piVar2 = &DAT_80133990;
	do {
		*piVar2 = iVar1;
		iVar1 = iVar1 + 1;
		piVar2 = piVar2 + 0x20;
	} while (iVar1 < 0x80);
}

void FUN_8003ba28() {
	int iVar1;
	int* piVar2;
	PSX::DR_MODE* p;
	int iVar3;
	int iVar4;
	undefined* puVar5;
	int iVar6;
	int iVar7;

	iVar6 = 0;
	iVar7 = 0;
	DAT_801f4054 = 0;
	do {
		iVar3 = 0;
		puVar5 = &DAT_801c47dc;
		do {
			PSX::SetTile((PSX::TILE*)(puVar5 + iVar6 * 0x1000));
			PSX::SetSemiTrans((PSX::TILE*)(puVar5 + iVar6 * 0x1000), 1);
			iVar3 = iVar3 + 1;
			puVar5 = puVar5 + 0x10;
		} while (iVar3 < 0x100);
		p = (PSX::DR_MODE*)(&DAT_801c67dc + iVar7);
		iVar7 = iVar7 + 0xc;
		iVar6 = iVar6 + 1;
		PSX::SetDrawMode(p, 0, 0, (uint)DAT_801f2e3c, (PSX::RECT*)0x0);
	} while (iVar6 < 2);
	iVar7 = 0;
	iVar6 = 0;
	do {
		iVar4 = 0;
		iVar3 = iVar6;
		do {
			(&DAT_80137990)[iVar3] = (short)iVar7;
			iVar1 = DAT_801d93ec;
			iVar4 = iVar4 + 1;
			iVar3 = iVar6 + iVar4;
		} while (iVar4 < 0x18);
		iVar7 = iVar7 + 1;
		iVar6 = iVar6 + 0x18;
	} while (iVar7 < 0x34);
	piVar2 = &DAT_801ac6f8;
	iVar6 = 0;
	if (-1 < DAT_801d93ec) {
		do {
			*piVar2 = iVar6;
			iVar6 = iVar6 + 1;
			piVar2 = piVar2 + 0xa5;
		} while (iVar6 <= iVar1);
	}
	//FUN_80031cbc(); // TODO implement function
	DAT_801ddac0 = 0xffffffff;
}

void LoadSpriteTable(char* filename, int start_data1, int end_data1, int start_data2, int end_data2) {
	int j;
	PSX::POLY_FT4* p;
	PSX::POLY_FT4* pPVar1;
	int i;

	i = 0;
	pPVar1 = (PSX::POLY_FT4*)&DAT_801c67f8;
	do {
		j = 0;
		p = pPVar1;
		do {
			PSX::SetPolyFT4(p);
			PSX::SetShadeTex(p, 1);
			p->r0 = 128;
			p->g0 = 128;
			p->b0 = 128;
			j = j + 1;
			p = p + 512;
		} while (j < 2);
		i = i + 1;
		pPVar1 = pPVar1 + 1;
	} while (i < 512);
	ReadDataFromCD(filename, &DATAS_BIN_DATA_SET_0, start_data1, end_data1 - start_data1);
	DATAS_BIN_DATA_SET_0_SIZE = end_data1 - start_data1;
	ConvertDataForVRAM(&EntitySpriteTable, (int*)&DATAS_BIN_DATA_SET_0);
	LoadImageToVRAM(DAT_801f395c, 192, 480, 40);
	ReadDataFromCD(filename, &DAT_801536c0, start_data2, end_data2 - start_data2);
	FUN_800839d8(&DAT_801536c0, 320, 256, 8, &SOURCE_ADDRESS);
	//FUN_8003ceec(); // empty function, maybe optimized debug function
	FUN_8003cb60();
	FUN_8003ba28();
}

void LoadBalanceFile() {

}

void FUN_8002aaf8() {

}

void FUN_80042f24(char* param_1, undefined4 param_2) {

}

void FUN_80043584() {

}

void FUN_80044f50() {

}

void FUN_80045b58() {

}

void FUN_80049464() {

}

void FUN_8005f78c(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4,
	undefined2 param_5, undefined2 param_6, undefined2 param_7, undefined2 param_8,
	/*undefined4*/ int* param_9) {

}