#include "setupAndLoop.h"
#include "PSX.h"

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
int ARAN_XA_XA_FILE;

int* DATAS_BIN_FILE_START;
int* SOUND_BIN_FILE_START;

long EVENT_0;
long EVENT_1;
long EVENT_2;
long EVENT_3;
long EVENT_4;
long EVENT_5;
long EVENT_6;
long EVENT_7;

char s_DATA_DATAS_BIN_800266a8[] = "\\DATA\\DATAS.BIN;1";
char s_DATA_SOUND_BIN_800266bc[] = "\\DATA\\SOUND.BIN;1";

void setupDataAndLoop() {
	PSX::VSync(0);
	PSX::ResetCallback();
	PSX::ResetGraph(3);
	// Original CdInit() without params (maybe because it should ever be 0?, that's how it is described in the documentation)
	// According to the documentation it has no return value, but a return value has been described and one is returned in the original.
	PSX::CdInit(0);
	SetupAudio();
	SetupPad();
	LinkResources();
	SetupCardAndEvents();
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
	//undefined2* puVar1;
	//undefined2* puVar2;
	int iVar3;

	//iVar3 = 0;
	//puVar2 = &DAT_801dda48;
	//puVar1 = &DAT_801dda60;
	DAT_801dda58 = 0;
	DAT_801dda40 = 0;
	DAT_801dda5c = 0;
	DAT_801dda44 = 0;
	//do {
	//	*puVar1 = 0;
	//	*puVar2 = 0;
	//	puVar2 = puVar2 + 1;
	//	iVar3 = iVar3 + 1;
	//	puVar1 = puVar1 + 1;
	//} while (iVar3 < 4);
}

void LinkResources() {
	int cdc_success;
	PSX::CdlFILE* pCVar1;

	PSX::CdSetDebug(0);
	CD_MODE_PARAM = 0x80; // global var
	DAT_80121b59 = 0;
	do {
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

	char s_ARAN_XA_XA[] = "\\ARAN_XA.XA;1"; // original a global variable

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