#pragma once
#include "PSX.h"

void setupDataAndLoop();
void SetupAudio();
void SetupAudio2();
void SetupPad();
void FUN_8002e670();
void LinkResources();
int LoadXASoundsAndSetupAudio();
void SetupCardAndEvents();
void SetupEvents();

void ReadDataFromCD(char* filename, ULONG* readData, unsigned int offset, int bytesToRead);
void SetCD_DOUBLE_SPEED_MODE();
void StrToUpper(char* text);
PSX::CdlFILE* getResourceFile(PSX::CdlFILE* resource_file, char* resourceName);

void LoadLanguageTexts();
void drawingStuff(void* param_1, void* param_2);
void SetupGlobalROTs();
void SetupSpriteEnvironment(int param_1);
void LoadSpriteTable(char* filename, int start_data1, int end_data1, int start_data2, int end_data2);
void LoadBalanceFile();
void FUN_8002aaf8();
void FUN_80042f24(char* param_1, undefined4 param_2);
void FUN_80043584();
void FUN_80044f50();
void FUN_80045b58();
void FUN_80049464();
void FUN_8005f78c(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4,
	undefined2 param_5, undefined2 param_6, undefined2 param_7, undefined2 param_8,
	int* param_9);