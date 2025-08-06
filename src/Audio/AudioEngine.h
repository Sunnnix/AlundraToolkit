#pragma once
#include <fluidsynth.h>
#include "VAB.h"

namespace Alundra {

	namespace AudioEngine {

		bool Init();

		bool SetSoundFont(VAB::VAB* vab);
		/// <param name="loops">times to loop the seq until stop</param>
		/// <param name="resetLoop">if set to -1 it will always reset the loop count</param>
		void SetSEQ(uint8_t* seq, int loops = 0, int resetLoop = 0);

		void Play();
		void Pause();
		void Stop();

		void Close();

	}

}