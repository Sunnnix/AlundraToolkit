#include "AudioEngine.h"
#include <thread>
#include <chrono>
#include "SoundFont.h"
#include "SEQPlayer.h"

namespace Alundra {

	namespace AudioEngine {

		void audioThreadFunc();

		bool initialized = false;
		bool running = false;
		bool skip = false; // say the audioLoop it should wait

		fluid_settings_t* settings = nullptr;
		fluid_synth_t* synth = nullptr;
		fluid_audio_driver_t* adriver = nullptr;

		fluid_player_t* player = nullptr;

		std::thread* audioLoopThread = nullptr;

		uint8_t* SEQ = nullptr;
		int loops;
		int resetLoops;

		bool Init() {
			if (initialized)
				return true;

			settings = new_fluid_settings();
			if (settings == NULL) {
				fprintf(stderr, "Failed to create the settings\n");
				Close();
				return false;
			}

			synth = new_fluid_synth(settings);
			if (synth == NULL) {
				fprintf(stderr, "Failed to create the synthesizer\n");
				Close();
				return false;
			}

			adriver = new_fluid_audio_driver(settings, synth);
			if (adriver == NULL) {
				fprintf(stderr, "Failed to create the audio driver\n");
				Close();
				return false;
			}

			SEQPlayer::Init(synth);

			audioLoopThread = new std::thread(audioThreadFunc);

			initialized = true;
			return true;
		}

		bool SetSoundFont(VAB::VAB* vab) {
			skip = true;
			SF2::SoundFont* sf2 = SF2::SoundFont::FromVAB(vab);
			sf2->saveSoundfont(".sf2");
			delete sf2;

			int sfid = fluid_synth_sfload(synth, ".sf2", 1);
			if (sfid == FLUID_FAILED) {
				fprintf(stderr, "Error loading SoundFont!\n");
				skip = false;
				return false;
			}
			fluid_synth_set_reverb_on(synth, true);

			skip = false;
			return true;
		}

		void SetSEQ(uint8_t* aSeq, int aLoops, int aResetLoop) {
			Stop();
			skip = true;
			SEQPlayer::SetSEQ(aSeq, aLoops, aResetLoop);
			skip = false;
		}

		void Play() {
			SEQPlayer::Play();
		}

		void Pause() {
			SEQPlayer::Pause();
		}

		void Stop() {
			SEQPlayer::Stop();
			fluid_synth_all_notes_off(synth, -1);
		}

		void Close() {
			if (audioLoopThread) {
				running = false;
				audioLoopThread->join();
				delete audioLoopThread;
			}
			if (adriver)
				delete_fluid_audio_driver(adriver);
			if (synth)
				delete_fluid_synth(synth);
			if (settings)
				delete_fluid_settings(settings);
			initialized = false;
		}

		void audioThreadFunc() {
			running = true;

			using namespace std::chrono;
			using clock = steady_clock;

			auto tickDuration = duration_cast<clock::duration>(duration<double>(1.0 / 60.0));
			auto nextTick = clock::now();

			SEQPlayer::Exec();

			double targetFPS = 60.0;
			double targetFrameTime = 1.0 / targetFPS;
			auto lastTime = clock::now();

			do {
				if (skip)
					continue;

				auto currentTime = clock::now();
				duration<double> elapsed = currentTime - lastTime;

				if (elapsed.count() >= targetFrameTime) {
					lastTime = currentTime;

					try {
						SEQPlayer::Exec();
					} catch (std::exception) {
						SEQPlayer::Stop();
						return;
					}
				}
			} while (running);
		}
	}

}