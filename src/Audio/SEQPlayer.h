#pragma once
#include <cstdint>
#include <fluidsynth.h>

namespace Alundra {
	namespace SEQPlayer {

#pragma pack(push, 2)
		struct AudioData {
			uint32_t	field_x00;
			uint8_t* SeqPosition;
			uint8_t* SeqStartPos;
			uint8_t* SeqLoopPos;		// For BGM with intro music
			//uint32_t	SeqPosition;
			//uint32_t	SeqStartPos;
			//uint32_t	SeqLoopPos;		// For BGM with intro music
			uint8_t		field_x10;
			uint8_t		MessageType;	// Seq instruction & 0xf0
			uint8_t		CurrentChannel;
			uint8_t		field_x13;
			uint16_t	field_x14;
			uint8_t		field_x16;
			uint8_t		Orientation[16];
			uint8_t		field_x27;
			uint8_t		Loops;
			uint8_t		field_x29;
			uint8_t		field_x2a;
			uint8_t		field_x2b;
			uint8_t		Channel[16];
			uint32_t	ResetLoop; // Should reset loop on SYSTEM_MESSAGE 0x2f?
			uint32_t	field_x40;
			uint16_t	field_x44;
			uint16_t	LoopCount;
			uint16_t	TimesPlayed;
			uint16_t	Tempo;
			uint16_t	VAB;			// ?? Check
			uint16_t	Volume[16];
			int16_t	PreDelay;
			uint16_t	CurrentTempo;
			uint16_t	field_x72;
			uint16_t	field_x74;
			uint16_t	field_x76;
			uint16_t	field_x78;
			uint16_t	field_x7a;
			uint32_t	field_x7c;
			uint32_t	Playtime;
			uint32_t	field_x84;
			uint32_t	Delay;
			uint32_t	field_x8c;
			uint8_t		PlayState;
			uint8_t		field_x91;
			uint16_t	field_x92;
			uint32_t	field_x94;
			uint32_t	field_x98;
			uint32_t	field_x9c;
			uint32_t	field_xa0;
			uint32_t	field_xa4;
			uint32_t	field_xa8;
		};

		void Init(fluid_synth_t* synth);

		void SetSEQ(uint8_t* aSeq, int aLoops, int aResetLoop);
		void Play();
		void Pause();
		void Stop();
		void Exec();

	}
}