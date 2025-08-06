#include "SEQPlayer.h"
#include <cstdarg>
#include <iostream>

namespace Alundra {
	namespace SEQPlayer {

		const uint16_t PITCH_BEND_MIDDLE = 8192;

		void resetAudioData();
		void FUN_8009675c();
		void FUN_8009399c();
		uint32_t SetupAudioSEQ();
		int ReadNextDelay();
		void updateBGM_and_SEs();
		void process();
		void nextInstr();
		void FUN_800902d8();
		void ExecCC(uint8_t seqDByte);
		void ExecNRPN_06(uint8_t data1);
		uint32_t ExecCC_7_10(short vab, short channel, uint32_t data1, uint16_t orientation);
		void ExecNRPN_98(uint8_t data1);
		void ExecNRPN_99(uint8_t data1);
		uint32_t SPU_PlayNote(uint16_t vab, int16_t channel);

		fluid_synth_t* _synth;

		uint8_t* SEQ = nullptr;

		AudioData audioData;
		AudioData* AudioDataPtr = &audioData;

		// ########################################
		//        Some needed psx variables
		// ########################################
		int UpdateAudioLock = 0;
		// Parallel audio systems?
		//short AudioDataCount = 4;
		// Stops the execution of updateBGM_and_SEs
		int AudioDataActiveMask = 1;
		// Amount of Sequences to be executed
		//short SEQ_Count_AudioData = 1;

		uint8_t MaxSPUVoices = 0x18; // 24 for SPU
		uint16_t* DAT_8012b436[0x1a]; // some struct with oxa1 size
		uint16_t DAT_801e71d2 = 0;

		uint8_t VoiceStates; //TODO define array
		uint16_t DAT_8012b42c; // TODO define array
		uint16_t DAT_8012b428; // TODO define array
		uint16_t DAT_8012b43e; // TODO define array
		uint16_t DAT_8012b43a; // TODO define array
		uint16_t DAT_8012b434; // TODO define array
		uint16_t DAT_8012b43c; // TODO define array
		uint16_t DAT_8012b294; // TODO define array
		uint8_t DAT_8012b410; // TODO define array
		uint8_t DAT_801e71d8; // TODO define array (something with VAB?)
		uint16_t DAT_8012b438; // TODO define array
		uint16_t DAT_8012b430; // TODO define array

		uint16_t DAT_801f94f8 = 0;
		uint16_t DAT_801f9500 = 0;
		uint16_t DAT_8012e260 = 0;
		uint16_t DAT_8012e268 = 0;
		uint16_t DAT_801e71ce = 0;
		uint8_t DAT_801e71bf = 0;
		uint32_t DAT_801d93e0 = 0;
		uint8_t DAT_801e71c4 = 0;
		uint8_t DAT_800cb600 = 0;
		uint16_t DAT_801d7f38 = 0;

		uint32_t DAT_801d8748 = 0;
		uint8_t DAT_801533b8 = 0;
		uint32_t DAT_801d8758 = 0;
		uint8_t DAT_80153438 = 0;
		uint8_t DAT_80153480 = 0;
		int8_t DAT_801e71b9 = 0;
		int8_t DAT_801e71be = 0;
		uint32_t VAM_Timer = 0;
		uint32_t VoiceActivityMask[16];
		int16_t CurrentVolume = 0;
		uint8_t DAT_801efc00 = 0;
		uint16_t DAT_8012b444 = 0;
		uint16_t DAT_8012b450 = 0;
		uint16_t DAT_8012b290 = 0;
		uint16_t DAT_8012b29a = 0;
		uint16_t DAT_8012b298 = 0;
		uint16_t DAT_8012b292 = 0;
		uint16_t DAT_8012b296 = 0;
		uint16_t DAT_8012e278 = 0;
		uint16_t DAT_8012e270 = 0;
		uint16_t DAT_801d7f18 = 0;

		uint32_t MainAudioTempo = 0x3c;

		// ########################################
		//               Functions
		// ########################################
		void printDebug(const char* format, ...) {
#if _DEBUG
			va_list args;
			va_start(args, format);
			vprintf(format, args);
			va_end(args);
#endif
		}

		void Init(fluid_synth_t* synth) {
			_synth = synth;
		}

		void SetSEQ(uint8_t* aSeq, int aLoops, int aResetLoop) {
			SEQ = aSeq;

			AudioDataPtr->PlayState = 1;
			AudioDataPtr->LoopCount = aLoops;
			AudioDataPtr->ResetLoop = aResetLoop;

			resetAudioData();
			SetupAudioSEQ();
		}

		void Play() {
			AudioDataPtr->PlayState = 1;
		}

		void Pause() {
			AudioDataPtr->PlayState = 2;
		}

		void Stop() {
			AudioDataPtr->PlayState = 4;
		}

		void Exec() {
			updateBGM_and_SEs();
		}

		// ########################################
		//              PSX Functions
		// ########################################

		void resetAudioData() {
			union AudioTmp {
				struct {
					uint8_t f1;
					uint8_t f2;
					uint16_t f3;
				};
				uint32_t data;
			};

			AudioTmp tmpData;
			uint32_t data2;
			uint32_t data3;
			uint32_t data4;
			AudioData* audioData;

			audioData = AudioDataPtr;

			tmpData.f1 = audioData->PlayState;
			tmpData.f2 = audioData->field_x91;
			tmpData.f3 = audioData->field_x92;
			tmpData.data &= ~1;
			audioData->PlayState = tmpData.f1;
			audioData->field_x91 = tmpData.f2;
			audioData->field_x92 = tmpData.f3;

			tmpData.f1 = audioData->PlayState;
			tmpData.f2 = audioData->field_x91;
			tmpData.f3 = audioData->field_x92;
			tmpData.data &= ~2;
			audioData->PlayState = tmpData.f1;
			audioData->field_x91 = tmpData.f2;
			audioData->field_x92 = tmpData.f3;

			tmpData.f1 = audioData->PlayState;
			tmpData.f2 = audioData->field_x91;
			tmpData.f3 = audioData->field_x92;
			tmpData.data &= ~8;
			audioData->PlayState = tmpData.f1;
			audioData->field_x91 = tmpData.f2;
			audioData->field_x92 = tmpData.f3;

			tmpData.f1 = audioData->PlayState;
			tmpData.f2 = audioData->field_x91;
			tmpData.f3 = audioData->field_x92;
			tmpData.data |= 4;
			audioData->PlayState = tmpData.f1;
			audioData->field_x91 = tmpData.f2;
			audioData->field_x92 = tmpData.f3;

			FUN_8009675c();

			audioData->field_x2b = 0;
			audioData->Playtime = 0;
			audioData->field_x27 = 0;
			audioData->field_x13 = 0;
			audioData->field_x14 = 0;
			audioData->field_x29 = 0;
			audioData->field_x16 = 0;
			audioData->field_x2a = 0;
			audioData->CurrentChannel = 0;
			audioData->TimesPlayed = 0;
			audioData->field_x27 = 0;
			audioData->Loops = 0;
			audioData->field_x10 = 0;
			audioData->MessageType = 0;
			audioData->Delay = audioData->field_x7c;
			audioData->field_x8c = audioData->field_x84;
			audioData->CurrentTempo = audioData->field_x72;
			audioData->SeqPosition = audioData->SeqStartPos;
			audioData->SeqLoopPos = audioData->SeqStartPos;
			for (int i = 0; i < 16; i++) {
				audioData->Channel[i] = i;
				audioData->Orientation[i] = 0x40;
				audioData->Volume[i] = 0x7f;
			}
			audioData->field_x78 = 0x7f;
			audioData->field_x7a = 0x7f;
		}

		void FUN_8009675c() {
			uint32_t uVar1;
			int8_t i;

			i = 0;
			if (MaxSPUVoices != 0) {
				uVar1 = 0;
				do {
					if ((&DAT_8012b436)[uVar1 * 0x1a] == 0) {
						DAT_801e71d2 = (uint16_t)i;
						FUN_8009399c();
					}
					i = i + 1;
					uVar1 = (uint32_t)i;
				} while (i < MaxSPUVoices);
			}
		}

		void FUN_8009399c() {
			uint32_t uVar1;
			uint16_t uVar2;
			uint16_t uVar3;

			uVar1 = (uint32_t)DAT_801e71d2;
			if (uVar1 < 0x10) {
				uVar3 = (uint16_t)(1 << (uVar1 & 0x1f));
				uVar2 = 0;
			} else {
				uVar3 = 0;
				uVar2 = (uint16_t)(1 << (uVar1 - 0x10 & 0x1f));
			}
			(&VoiceStates)[uVar1 * 0x34] = 0; // Maay throw error
			(&DAT_8012b42c)[uVar1 * 0x1a] = 0;
			(&DAT_8012b428)[uVar1 * 0x1a] = 0;
			DAT_801f94f8 = uVar3 | DAT_801f94f8;
			DAT_801f9500 = uVar2 | DAT_801f9500;
			DAT_8012e260 = DAT_8012e260 & ~DAT_801f94f8;
			DAT_8012e268 = DAT_8012e268 & ~DAT_801f9500;
		}

		uint32_t SetupAudioSEQ() {
			uint8_t cVar1;
			uint8_t bVar2;
			uint8_t bVar3;
			uint32_t uVar4;
			uint32_t uVar5;
			uint32_t uVar6;
			uint16_t uVar7;
			uint8_t* pbVar10;
			uint32_t iVar11;
			AudioData* ad;
			uint32_t uVar12;
			uint8_t* rawSEQ = SEQ;

			iVar11 = 0;
			ad = AudioDataPtr;
			ad->VAB = 0;//vab?;
			ad->Tempo = 0;
			ad->field_x13 = 0;
			ad->field_x14 = 0;
			ad->field_x29 = 0;
			ad->field_x16 = 0;
			ad->field_x2a = 0;
			ad->CurrentChannel = 0;
			ad->field_x7c = 0;
			ad->Playtime = 0;
			ad->field_x84 = 0;
			ad->field_x72 = 0;
			ad->TimesPlayed = 0;
			ad->field_x2b = 0;
			ad->Delay = 0;
			ad->field_x27 = 0;
			ad->Loops = 0;
			ad->field_x10 = 0;
			ad->MessageType = 0;
			ad->field_xa8 = 0x7F;
			do {
				ad->Channel[iVar11] = (uint8_t)iVar11;
				ad->Orientation[iVar11] = 0x40;
				ad->Volume[iVar11] = 0x7f;
				iVar11 = iVar11 + 1;
			} while (iVar11 < 0x10);
			ad->PreDelay = 1;
			ad->SeqPosition = rawSEQ;
			if ((*rawSEQ == 'S') || (*rawSEQ == 'p')) {
				ad->SeqPosition = (uint8_t*)(rawSEQ + 8);
				if (rawSEQ[7] == 1) {
					ad->SeqPosition = (uint8_t*)(rawSEQ + 9);
					cVar1 = rawSEQ[8];
					ad->SeqPosition = (uint8_t*)(rawSEQ + 10);
					pbVar10 = (uint8_t*)ad->SeqPosition;
					ad->Tempo = ((uint16_t)cVar1 << 8) | ((uint8_t)rawSEQ[9]);
					ad->SeqPosition = pbVar10 + 1;
					bVar2 = *pbVar10;
					ad->SeqPosition = pbVar10 + 2;
					bVar3 = pbVar10[1];
					ad->SeqPosition = pbVar10 + 3;
					uVar5 = (uint32_t)bVar2 << 0x10 | (uint32_t)bVar3 << 8 | (uint32_t)pbVar10[2];
					if (uVar5 == 0) {
						throw std::exception("0x1c00");
						exit(0x1c00);
					}
					ad->field_x84 = uVar5;
					if (uVar5 >> 1 < 60000000U % uVar5) {
						ad->field_x84 = 60000000 / uVar5 + 1;
					} else {
						ad->field_x84 = 60000000 / uVar5;
					}
					ad->field_x8c = ad->field_x84;
					ad->SeqPosition = ad->SeqPosition + 2;
					uVar6 = ReadNextDelay();
					iVar11 = MainAudioTempo;
					uVar12 = (int)(short)ad->Tempo * ad->field_x84;
					ad->field_x7c = uVar6;
					ad->Delay = uVar6;
					ad->SeqLoopPos = ad->SeqPosition;
					uVar5 = iVar11 * 0x3c;
					ad->SeqStartPos = ad->SeqPosition;
					if (uVar12 * 10 < uVar5) {
						if (uVar12 == 0) {
							throw std::exception("0x1c00");
							exit(0x1c00);
						}
						uVar7 = (uint16_t)((uint32_t)(iVar11 * 600) / uVar12);
						ad->PreDelay = uVar7;
						ad->CurrentTempo = uVar7;
					} else {
						if (uVar5 == 0) {
							throw std::exception("0x1c00");
							exit(0x1c00);
						}
						if (uVar5 == 0) {
							throw std::exception("0x1c00");
							exit(0x1c00);
						}
						ad->PreDelay = 0xffff;
						uVar7 = (uint16_t)(((int)(short)ad->Tempo * ad->field_x84 * 10) / uVar5);
						ad->CurrentTempo = uVar7;
						if ((uint32_t)(iVar11 * 0x1e) < ((int)(short)ad->Tempo * ad->field_x84 * 10) % uVar5)
							ad->CurrentTempo = uVar7 + 1;
					}
					uVar4 = 0;
					ad->field_x72 = ad->CurrentTempo;
				} else {
					printDebug("This is not SEQ Data.\n");
					uVar4 = 0xffffffff;
				}
			} else {
				printDebug("This is an old SEQ Data Format.\n");
				uVar4 = 0;
			}
			return uVar4;
		}

		int ReadNextDelay() {
			int32_t delay;
			uint32_t iInstruction;
			AudioData* audioData;
			uint8_t instruction;

			audioData = AudioDataPtr;
			instruction = *audioData->SeqPosition;
			audioData->SeqPosition++;
			iInstruction = (uint32_t)instruction;
			if (iInstruction == 0) {
				delay = 0;
			} else {
				delay = iInstruction << 2;
				if ((instruction & 0x80) != 0) {
					/* Multibyte delay */
					iInstruction = iInstruction & 0x7f;
					do {
						instruction = *audioData->SeqPosition;
						audioData->SeqPosition++;
						iInstruction = iInstruction * 0x80 + (instruction & 0x7f);
					} while ((instruction & 0x80) != 0);
					delay = iInstruction * 4;
				}
				delay = (delay + iInstruction) * 2;
				audioData->Playtime = delay + audioData->Playtime;
			}
			return delay;
		}

		void updateBGM_and_SEs() {
			if (UpdateAudioLock == 1)
				return;

			UpdateAudioLock = 1;
			//updateSPU();

			AudioData* ad = AudioDataPtr;
			uint32_t playState = ad->PlayState;
			if ((playState & 1) != 0) {
				// SEQ is running
				process();
				if ((playState & 0x10) != 0) {
					//FUN_80090e78(audioIndex, seqIndex);
				}
				if ((playState & 0x20) != 0) {
					//FUN_80091138(audioIndex, seqIndex);
				}
				if ((playState & 0x40) != 0) {
					//FUN_80091d14(audioIndex, seqIndex);
				}
				if ((playState & 0x80) != 0) {
					//FUN_80091d14(audioIndex, seqIndex);
				}
			}
			if ((playState & 2) != 0) {
				/* SEQ Pause (not reset) */
				//FUN_800913c4(audioIndex, seqIndex);
			}
			if ((playState & 8) != 0) {
				//FUN_8009148c(audioIndex, seqIndex);
			}
			if ((playState & 4) != 0) {
				/* SEQ Reset */
				resetAudioData();
				ad->PlayState = 0;
			}
			UpdateAudioLock = 0;
		}

		void process() {
			uint32_t reducedDelay;
			uint32_t delay;
			AudioData* audioData;
			uint16_t preDelay;
			uint16_t tempo;

			audioData = AudioDataPtr;
			tempo = audioData->CurrentTempo;
			delay = audioData->Delay;
			reducedDelay = delay - (int32_t)(int16_t)tempo;
			if ((int32_t)reducedDelay < 1) {
				if ((int32_t)(int16_t)tempo < (int32_t)delay) {
					return;
				}
				do {
					do {
						nextInstr();
					} while (audioData->Delay == 0);
					delay = delay + audioData->Delay;
					reducedDelay = delay - (int)(short)audioData->CurrentTempo;
				} while ((int)delay < (int)(short)audioData->CurrentTempo);
			} else {
				preDelay = audioData->PreDelay;
				if (0 < (short)preDelay) {
					audioData->PreDelay = preDelay - 1;
					return;
				}
				if (preDelay != 0) {
					audioData->Delay = reducedDelay;
					return;
				}
				audioData->PreDelay = tempo;
				reducedDelay = audioData->Delay - 1;
			}
			audioData->Delay = reducedDelay;
			return;
		}

		void nextInstr() {

			enum MessageType {
				NOTE_OFF = 0x80,
				NOTE_ON = 0x90,
				POLYPHONIC_AFTERTOUCH = 0xA0,
				CONTROL_CHANGE = 0xB0,
				PROGRAM_CHANGE = 0xC0,
				CHANNEL_AFTERTOUCH = 0xD0,
				PITCH_BEND_CHANGE = 0xE0,
				SYSTEM_MESSAGE = 0xF0
			};

			AudioData* ad = AudioDataPtr;
			if ((ad->PlayState & 1) != 1)
				return;

			uint8_t instr = *ad->SeqPosition;
			ad->SeqPosition++;

			uint8_t channel = instr & 0x0f;
			uint8_t messageType;

			uint8_t data1;
			uint8_t data2;

			if ((instr & 0x80) != 0) {
				// Status Byte
				ad->CurrentChannel = channel;
				messageType = instr & 0xf0;
			ExecInstr:
				switch (messageType) {
				case NOTE_OFF:
				case NOTE_ON:
				case POLYPHONIC_AFTERTOUCH:
				case PITCH_BEND_CHANGE:
					data1 = *ad->SeqPosition & 0x7f;
					ad->SeqPosition++;
					data2 = *ad->SeqPosition & 0x7f;
					ad->SeqPosition++;
					switch (messageType) {
					case NOTE_OFF:
						fluid_synth_noteoff(_synth, channel, data1);
						ad->Delay = ReadNextDelay();
						printDebug("Note Off: Ch=%d | note=%d\n", channel, data1);
						break;
					case NOTE_ON:
						// Added reset Pitch Bend on new Note to prevent wrong pitch on new notes like in tracks Pub or Nightmare
						fluid_synth_pitch_bend(_synth, channel, PITCH_BEND_MIDDLE);
						ad->MessageType = NOTE_ON;
						fluid_synth_noteon(_synth, channel, data1, data2);
						ad->Delay = ReadNextDelay();
						printDebug("Note On: Ch=%d | note=%d | vel=%d%s\n", channel, data1, data2, data2 == 0 ? " (Off)" : "");
						break;
					case POLYPHONIC_AFTERTOUCH:
						fluid_synth_key_pressure(_synth, channel, data1, data2);
						ad->Delay = ReadNextDelay();
						printDebug("POLYPHONIC_AFTERTOUCH: Ch=%d | note=%d | val = %d\n", channel, data1, data2);
						break;
					case PITCH_BEND_CHANGE:
						ad->MessageType = PITCH_BEND_CHANGE;
						// Added pitch bend calculations to sound like the PSU
						uint16_t pitchBendValue = ((uint16_t)data1) + (data2 << 7);
						//int16_t pitchBendDiff = PITCH_BEND_MIDDLE - pitchBendValue;
						//int maxPitch = 0;
						//for (int i = 0; i < currentVAB->ProgramAttrTable[channel].tones; i++)
						//    if (currentVAB->ToneAttrTable[channel][0].pbmax > maxPitch)
						//        maxPitch = currentVAB->ToneAttrTable[channel][0].pbmax;
						//pitchBendDiff = (pitchBendDiff / 127.0 * maxPitch);
						//pitchBendValue = PITCH_BEND_MIDDLE + pitchBendDiff;

						fluid_synth_pitch_bend(_synth, channel, pitchBendValue);
						ad->Delay = ReadNextDelay();
						printDebug("PITCH_BEND_CHANGE: Ch=%d | val = %d\n", channel, pitchBendValue);
						break;
					}
					break;
				case PROGRAM_CHANGE:
				case CHANNEL_AFTERTOUCH:
					data1 = *ad->SeqPosition & 0x7f;
					ad->SeqPosition++;
					if (messageType == PROGRAM_CHANGE) {
						ad->MessageType = PROGRAM_CHANGE;
						// if channel 9 (Drumkit) use preset 0!
						fluid_synth_program_change(_synth, channel, channel == 9 ? 0 : data1);
						ad->Delay = ReadNextDelay();
						if (channel == 9)
							printDebug("Program change: Ch=%d | program = 0 (Drumkit)\n", channel, data1);
						else
							printDebug("Program change: Ch=%d | program = %d\n", channel, data1);
					} else {
						fluid_synth_channel_pressure(_synth, channel, data1);
						printDebug("CHANNEL_AFTERTOUCH: Ch=%d | val = %d\n", channel, data1);
					}
					break;
				case CONTROL_CHANGE:
					ad->MessageType = CONTROL_CHANGE;
					data1 = *ad->SeqPosition;
					ad->SeqPosition++;
					ExecCC(data1);
					break;
				case SYSTEM_MESSAGE:
					if (instr == 0xff) {
						// Meta-Event
						ad->MessageType = 0xff;
					SystemMessageMeta:
						data1 = *ad->SeqPosition;
						ad->SeqPosition++;
						if (data1 == 0x2f) {
							uint32_t uVar2 = ++ad->TimesPlayed;
							if (ad->LoopCount == 0) {
								ad->Playtime = 0;
								ad->field_x27 = 0;
								ad->Delay = 0;
								ad->SeqPosition = ad->SeqStartPos;
							} else if ((int16_t)uVar2 < ad->LoopCount) {
								ad->Playtime = 0;
								ad->field_x27 = 0;
								ad->Delay = 0;
								ad->SeqPosition = ad->SeqStartPos;
								ad->SeqLoopPos = ad->SeqStartPos;
							} else {
								ad->PlayState &= 0xfffffff4;
								ad->PlayState |= 0x204;
								ad->field_x2b = 0;
								ad->SeqLoopPos = ad->SeqStartPos;
								if ((int8_t)ad->ResetLoop != -1) {
									ad->field_x2b = 0;
									FUN_800902d8();
									FUN_8009675c();
								}
								FUN_8009675c();
								ad->Delay = (int32_t)(int16_t)ad->CurrentTempo;
							}
						} else if (data1 == 0x51) {
							// Tempo
							int32_t tempoMult = MainAudioTempo;
							uint8_t v1 = *ad->SeqPosition;
							ad->SeqPosition++;
							uint8_t v2 = *ad->SeqPosition;
							ad->SeqPosition++;
							uint8_t v3 = *ad->SeqPosition;
							ad->SeqPosition++;
							uint32_t rTempo = (uint32_t)v1 << 16 | (uint32_t)v2 << 8 | (uint32_t)v3;
							if (rTempo == 0) {
								throw std::exception("0x1c00");
								exit(0x1c00);
							}
							uint32_t bmp = 60000000 / rTempo;
							uint32_t scaledTempo = (int32_t)(int16_t)ad->Tempo * bmp;
							rTempo = MainAudioTempo * 0x3c;
							ad->field_x8c = bmp;
							if (scaledTempo * 10 < rTempo) {
								if (scaledTempo == 0) {
									throw std::exception("0x1c00");
									exit(0x1c00);
								}
								uint16_t nextTempo = (uint16_t)((uint32_t)(tempoMult * 600) / scaledTempo);
								ad->PreDelay = nextTempo;
								ad->CurrentTempo = nextTempo;
							} else {
								if (rTempo == 0) {
									throw std::exception("0x1c00");
									exit(0x1c00);
								}
								ad->PreDelay = 0xffff;
								uint16_t nextTempo = (uint16_t)(((int32_t)(int16_t)ad->Tempo * ad->field_x8c * 10) / rTempo);
								ad->CurrentTempo = nextTempo;
								if ((uint32_t)(tempoMult * 0x1e) < ((int32_t)(int16_t)ad->Tempo * ad->field_x8c * 10) % rTempo) {
									ad->CurrentTempo = nextTempo + 1;
								}
							}
							ad->Delay = ReadNextDelay();
							printDebug("Tempo change: new tempo=%d\n", ad->CurrentTempo);
						} else {
							printDebug("Unimplemented Meta-Event: %d\n", data1);
							throw std::exception(); //exit(-1996);
						}
					} else {
						printDebug("Unknown SYSTEM MESSAGE instruction: %d\n", instr);
						throw std::exception(); //exit(-1996);
					}
					break;
				default:
					printDebug("Unknown message type: %d\n", messageType);
					throw std::exception(); //exit(-1996);
					break;
				}
			} else {
				// Continues instructions (message saved previous)
				messageType = ad->MessageType;
				channel = ad->CurrentChannel;
				ad->SeqPosition--;
				if (messageType == 0xff)
					goto SystemMessageMeta;
				else
					goto ExecInstr;
			}
		}

		void FUN_800902d8() {
			AudioData* ad = AudioDataPtr;

			ad->LoopCount = 1;
			ad->TimesPlayed = 0;
			ad->PlayState &= 0xfffffCF1;
			ad->PlayState |= 0x1;
			ad->field_x2b = 0;
			ad->SeqPosition = ad->SeqStartPos;
		}

		void ExecCC(uint8_t seqDByte) {
			uint32_t delay;
			AudioData* ad = AudioDataPtr;
			uint8_t channel = ad->CurrentChannel;
			uint8_t seqData = *ad->SeqPosition++;

			if (0x79 < seqDByte) {
				ad->Delay = ReadNextDelay();
				return;
			}
			switch (seqDByte) {
			case 0:
				ad->VAB = (uint16_t)seqData;
				ad->Delay = ReadNextDelay();
				break;
			case 6:
				ExecNRPN_06(seqData);
				printDebug("ExecCC seqDByte %d not implemented!\n", seqDByte);
				break;
			case 7:
				//ExecCC_7_10((int)(short)ad->VAB, ad->Channel[channel], (int)(short)ad->Volume[channel], seqData);

				ad->Volume[ad->Channel[channel]] = seqData;
				fluid_synth_cc(_synth, channel, seqDByte, seqData);
				ad->Delay = ReadNextDelay();
				printDebug("Control change: Channel Volume: Ch=%d | vol = %d\n", channel, seqData);
				break;
			case 10:
				//ExecCC_7_10((int)(short)ad->VAB, ad->Channel[channel], (int)(short)ad->Volume[channel], seqData);

				ad->Orientation[channel] = seqData;
				fluid_synth_cc(_synth, channel, seqDByte, seqData);
				ad->Delay = ReadNextDelay();
				printDebug("Control change: Channel Pan: Ch=%d | pan = %d\n", channel, seqData);
				break;
			case 11:
				//FUN_800967f4((int)(short)audioData->VAB ? , (&audioData->Channel1)[channel], seqData);
				//SPU_BGM3 ? ((int)seqIndex << 8 | (int)(short)audioIndex, (int)(short)audioData->VAB ? ,
				//    (&audioData->Channel1)[channel], (int)(short)(&audioData->Volume1)[channel],
				//    (&audioData->Orientation1)[channel]);
				//delay = ReadNextDelay((int)(short)audioIndex, (int)seqIndex);
				//audioData->Delay = delay;
				printDebug("ExecCC seqDByte %d not implemented!\n", seqDByte);
				break;
			case 64:
				//if (seqData < 0x40) {
				//    FUN_800929ac();
				//}
				//else {
				//    FUN_800929bc();
				//}
				fluid_synth_cc(_synth, channel, seqDByte, seqData);
				ad->Delay = ReadNextDelay();
				printDebug("Control change: Sustain: Ch=%d | val = %d\n", channel, seqData);
				printDebug("ExecCC seqDByte %d not completely implemented!\n", seqDByte);
				ad->Delay = ReadNextDelay();
				break;
			case 65:
				//FUN_8008ef74((int)(short)audioIndex, (int)seqIndex, seqData);
				printDebug("ExecCC seqDByte %d not implemented!\n", seqDByte);
				break;
			case 91:
				//FUN_80092f90(seqData, seqData);
				printDebug("ExecCC seqDByte %d not implemented!\n", seqDByte);
				ad->Delay = ReadNextDelay();
				break;
			case 98:
				ExecNRPN_98(seqData);
				break;
			case 99:
				ExecNRPN_99(seqData);
				break;
			case 100:
				//FUN_8008f3f0((int)(short)audioIndex, (int)seqIndex, seqData);
				fluid_synth_cc(_synth, channel, seqDByte, seqData);
				ad->Delay = ReadNextDelay();
				printDebug("Control change: RPN LSB : Ch=%d | val = %d\n", channel, seqData);
				printDebug("ExecCC seqDByte %d not completely implemented!\n", seqDByte);
				break;
			case 0x65:
				//FUN_8008f464((int)(short)audioIndex, (int)seqIndex, seqData);
				printDebug("ExecCC seqDByte %d not implemented!\n", seqDByte);
				break;
			case 0x79:
				//FUN_8008f0bc((int)(short)audioIndex, (int)seqIndex);
				printDebug("ExecCC seqDByte %d not implemented!\n", seqDByte);
				break;
			default:
				ad->Delay = ReadNextDelay();
				printDebug("ExecCC default executed! %d, %d\n", seqDByte, seqData);
				break;
			}
		}

		void ExecNRPN_06(uint8_t data1) {
			uint8_t uVar2;
			AudioData* audioData;
			int iVar3;
			int8_t local_b0[16];
			uint32_t local_a0;
			uint32_t local_9c;
			uint32_t local_98;
			uint32_t local_94;
			uint32_t local_90;
			uint32_t local_8c;
			uint32_t local_88;
			uint32_t local_84;
			uint32_t local_80;
			uint32_t local_7c;
			uint32_t local_78;
			uint32_t local_74;
			uint16_t local_70;
			int8_t channel;

			audioData = AudioDataPtr;
			channel = audioData->CurrentChannel;
			//FUN_80092a10((int)(short)audioData->VAB, audioData->Channel[channel], local_b0);
			if ((audioData->field_x27 == 1) && (audioData->field_x10 == 0)) {
				audioData->Loops = data1;
				audioData->field_x10 = 1;
				audioData->Delay = ReadNextDelay();
			} else if (audioData->field_x29 == 2) {
				if (((audioData->field_x13 == 0) && ((char)audioData->field_x14 == 0))
					&& (iVar3 = 0, local_b0[0] != 0)) {
					do {
						//SPU_Sound((int)(short)audioData->VAB, audioData->Channel[channel], iVar3, &local_a0);
						local_94 = ((local_94 & 0xffff0000) | (data1 << 8) | data1) & 0xffff7f7f;
						//FUN_8009308c((int)(short)audioData->VAB, audioData->Channel[channel], iVar3, &local_a0);
						iVar3 = iVar3 + 1;
					} while (iVar3 < (int)(uint32_t)local_b0[0]);
				}
				uVar2 = audioData->field_x13;
				if ((uVar2 == 1) && ((char)audioData->field_x14 == 0)) {
					iVar3 = 0;
					if (local_b0[0] != 0) {
						do {
							//SPU_Sound((int)(short)audioData->VAB, audioData->Channel[channel], iVar3, &local_a0);
							//FUN_8009308c((int)(short)audioData->VAB, audioData->Channel[channel], iVar3, &local_a0);
							iVar3 = iVar3 + 1;
						} while (iVar3 < (int)(uint32_t)local_b0[0]);
					}
					uVar2 = audioData->field_x13;
				}
				if (((uVar2 == 2) && ((char)audioData->field_x14 == 0)) &&
					(iVar3 = 0, local_b0[0] != 0)) {
					do {
						//SPU_Sound((int)(short)audioData->VAB, audioData->Channel[channel], iVar3, &local_a0);
						//FUN_8009308c((int)(short)audioData->VAB, audioData->Channel[channel], iVar3, &local_a0);
						iVar3 = iVar3 + 1;
					} while (iVar3 < (int)(uint32_t)local_b0[0]);
				}
				audioData->Delay = ReadNextDelay();
				audioData->field_x29 = 0;
			} else if (audioData->field_x2a == 2) {
				if (audioData->field_x16 == 16) {
					iVar3 = 0;
					if (local_b0[0] != 0) {
						do {
							//FUN_8008fa5c((int)(short)audioData->VAB, audioData->Channel[channel], iVar3, local_a0, local_9c, local_98,
							//    local_94, local_90, local_8c, local_88, local_84, local_80, local_7c, local_78,
							//    local_74, local_70, *(uint8_t*)((int)&audioData->field_x14 + 1)
							//    , data1);
							iVar3 = iVar3 + 1;
						} while (iVar3 < (int)(uint32_t)local_b0[0]);
					}
				} else {
					//FUN_8008fa5c((int)(short)audioData->VAB, audioData->Channel[channel],
					//    audioData->field_x16, local_a0, local_9c, local_98, local_94, local_90,
					//    local_8c, local_88, local_84, local_80, local_7c, local_78, local_74, local_70,
					//    *(uint8_t*)((int)&audioData->field_x14 + 1), data1);
				}
				audioData->Delay = ReadNextDelay();
				audioData->field_x2a = 0;
			} else {
				audioData->Delay = ReadNextDelay();
			}
		}

		uint32_t ExecCC_7_10(short vab, short channel, uint32_t data1, uint16_t orientation) {
			uint32_t uVar1;
			AudioData* ad;
			uint32_t uVar3;
			int32_t iVar4;
			int8_t bVar5;
			int32_t _channel;
			int32_t iVar6;
			uint32_t uVar7;

			ad = AudioDataPtr;
			_channel = (int)channel;
			SPU_PlayNote((int)vab, _channel);
			iVar6 = 0;
			DAT_801e71ce = 0;
			bVar5 = 0;
			if (MaxSPUVoices != 0) {
				uVar1 = 0;
				do {
					if ((((short)(&DAT_8012b43a)[uVar1 * 0x1a] == _channel)) &&
						((&DAT_8012b43e)[uVar1 * 0x1a] == vab)) {
						if (((int)(short)ad->Volume[ad->CurrentChannel] != (data1 & 0xffff)) &&
							((int)(short)ad->Volume[ad->CurrentChannel] == 0)) {
							ad->Volume[ad->CurrentChannel] = 1;
						}
						iVar4 = (_channel * 0x10 + (int)(short)(&DAT_8012b43c)[(uint32_t)bVar5 * 0x1a]) * 0x20 +
							DAT_801d93e0;
						uVar7 = (((int)(((int)((int)(short)(&DAT_8012b430)[(uint32_t)bVar5 * 0x1a] * (data1 & 0xffff)) /
								 0x7f) * (uint32_t) * (int8_t*)(DAT_801d8758 + 0x18) * 0x3fff) / 0x3f01) *
								 (uint32_t) * (int8_t*)(_channel * 0x10 + DAT_801d8748 + 1) * (uint32_t) * (int8_t*)(iVar4 + 2)) /
							0x3f01;
						uVar1 = (uVar7 * ad->field_x74) / 0x7f;
						uVar7 = (uVar7 * ad->field_x76) / 0x7f;
						uVar3 = (uint32_t) * (int8_t*)(iVar4 + 3);
						if (uVar3 < 0x40) {
							uVar7 = (uVar7 * uVar3) / 0x3f;
						} else {
							uVar1 = (uVar1 * (0x7f - uVar3)) / 0x3f;
						}
						uVar3 = (uint32_t) * (int8_t*)((short)(&DAT_8012b438)[(uint32_t)bVar5 * 0x1a] * 0x10 + DAT_801d8748 + 4
													   );
						if (uVar3 < 0x40) {
							uVar7 = (uVar7 * uVar3) / 0x3f;
						} else {
							uVar1 = (uVar1 * (0x7f - uVar3)) / 0x3f;
						}
						uVar3 = orientation & 0xff;
						if (uVar3 < 0x40) {
							uVar7 = (uVar7 * uVar3) / 0x3f;
						} else {
							uVar1 = (uVar1 * (0x7f - uVar3)) / 0x3f;
						}
						uVar3 = uVar1 * uVar1;
						if (DAT_801d7f18 == 1) {
							if (uVar1 < uVar7) {
								uVar1 = uVar7;
							}
							uVar3 = uVar1 * uVar1;
							uVar7 = uVar1;
						}
						uVar1 = (uint32_t)bVar5;
						(&DAT_8012b290)[uVar1 * 8] = (short)(uVar3 / 0x3fff);
						(&DAT_8012b292)[uVar1 * 8] = (short)((uVar7 * uVar7) / 0x3fff);
						iVar6 = iVar6 + 1;
						(&DAT_8012b410)[uVar1] = (&DAT_8012b410)[uVar1] | 3;
					}
					bVar5 = bVar5 + 1;
					uVar1 = (uint32_t)bVar5;
				} while (bVar5 < MaxSPUVoices);
			}
			return iVar6;
		}

		void ExecNRPN_98(uint8_t data1) {
			char cVar1;
			uint8_t uVar2;
			AudioData* ad;

			ad = AudioDataPtr;
			if ((ad->field_x27 == 1) && (ad->field_x10 == 0)) {
				ad->Loops = data1;
				ad->field_x10 = 1;
			} else {
				uVar2 = ad->field_x16;
				if ((uVar2 == 30) || (uVar2 == 20)) goto LAB_8008f230;
				cVar1 = ad->field_x2a;
				*(uint8_t*)((int)&ad->field_x14 + 1) = data1;
				ad->field_x2a = cVar1 + 1;
			}
			uVar2 = ad->field_x16;
		LAB_8008f230:
			if (uVar2 == 40) {
				// TODO in live data always 0? And don't know what function it points to.
				//if (*(code**)(&UNK_801f3158 + seqIndex * 4 + (short)audioIndex * 0x40) != (code*)0) {
				//    (**(code**)(&UNK_801f3158 + seqIndex * 4 + (short)audioIndex * 0x40))
				//        ((int)(short)audioIndex, (int)seqIndex, data1);
				//}
				printDebug("ExecNRPN_D2 Unimplemented synamic function call!\n");
			}
			ad->Delay = ReadNextDelay();
			printDebug("ExecNRPN_D2!\n");
		}

		void ExecNRPN_99(uint8_t data1) {
			uint32_t delay;
			AudioData* ad = AudioDataPtr;

			if (data1 == 20) {
				ad->field_x16 = 20;
				ad->field_x27 = 1;
				ad->Delay = ReadNextDelay();
				ad->SeqLoopPos = ad->SeqPosition;
				printDebug("ExecNRPN_D1 loop start!\n");
			} else if (data1 == 30) {
				ad->field_x16 = 30;
				if (ad->Loops == 0) {
					ad->field_x10 = 0;
					ad->Delay = ReadNextDelay();
				} else if (ad->Loops < 0x7f) {
					ad->Loops = ad->Loops + 0xff;
					ad->Delay = ReadNextDelay();
					if (ad->Loops == 0) {
						ad->field_x10 = 0;
					} else {
						ad->SeqPosition = ad->SeqLoopPos;
					}
				} else {
					ReadNextDelay();
					ad->Delay = 0;
					ad->SeqPosition = ad->SeqLoopPos;
				}
				printDebug("ExecNRPN_D1 loop end!\n");
			} else {
				ad->field_x16 = data1;
				ad->field_x2a = ad->field_x2a + 1;
				ad->Delay = ReadNextDelay();
				printDebug("ExecNRPN_D1 else!\n");
			}
		}

		uint32_t SPU_PlayNote(uint16_t vab, int16_t channel) {
			int iVar1;

			if (vab < 0x10) {
				if ((&DAT_801e71d8)[(int16_t)vab] != 1) {
					return 0xffffffff;
				}
				iVar1 = (int16_t)vab * 4;
				if ((int32_t)channel < (int32_t)DAT_801d7f38) {
					DAT_801d8748 = *(int32_t*)(&DAT_801533b8 + iVar1);
					DAT_801d8758 = *(uint32_t*)(&DAT_80153438 + iVar1);
					DAT_801d93e0 = *(uint32_t*)(&DAT_80153480 + iVar1);
					DAT_801e71b9 = (int8_t)vab;
					DAT_801e71be = (int8_t)channel;
					DAT_801e71bf = *(uint8_t*)(channel * 0x10 + *(int32_t*)(&DAT_801533b8 + iVar1) + 8);
					return 0;
				}
			}
			return 0xffffffff;
		}

	}
}