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

int cdMode;
int tmpData; // tmpData may be to small

LONG PRINT_STREAM_ID;
unsigned short TexturePageIDs;
unsigned short DAT_8012e2a0;

int DATAS_BIN_HEADER[494]; // 0x7B8 / 4

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
char s_DATA_DATAS_BIN_800266a8[] = "\\DATA\\DATAS.BIN;1";
char s_DATA_SOUND_BIN_800266bc[] = "\\DATA\\SOUND.BIN;1";
char s_ARAN_XA_XA[] = "\\ARAN_XA.XA;1";
char s_DATA_DATAS_BIN_800c7dd4[] = "\\DATA\\DATAS.BIN";
char s_800266d8[] = "\\%s.;1";
char s_800266d0[] = "\\%s;1";

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

void LoadLanguageTexts() {

}

void drawingStuff(/*undefined4*/ void* param_1, /*undefined4*/ void* param_2) {

}

void SetupGlobalROTs() {

}

void SetupSpriteEnvironment(/*undefined4*/ int param_1) {

}

void LoadSpriteTable(char* filename, int start_data1, int end_data1, int start_data2, int end_data2) {

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