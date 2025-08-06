#include "SoundFont.h"
#include <algorithm>
#include <format>

size_t DecodeADPCM(const uint8_t* data, size_t size, int16_t** outSamples, int* loopStart, int* loopEnd, bool* looping);

void DecodeADSR(uint16_t* adsr, double* attack_time, double* decay_time, double* sustain_time, double* release_time, double* sustain_level);

namespace Alundra {
	namespace SF2 {
		
		//##################################
		//           classes
		//##################################

		size_t Chunk::getSize() {
			return naming.length() + sizeof(uint32_t) + getBodySize();
		}

		void Chunk::save(std::ofstream& oFile) {
			uint32_t size = getBodySize();
			oFile.write(naming.data(), naming.length());
			oFile.write((const char*)&size, sizeof(uint32_t));
		}

		size_t TopLevelChunk::getBodySize() {
			return format.length();
		}

		void TopLevelChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write(format.data(), format.length());
		}

		size_t ifilChunk::getBodySize() {
			return sizeof(wMajor) + sizeof(wMinor);
		}

		void ifilChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)&wMajor, sizeof(wMajor));
			oFile.write((const char*)&wMinor, sizeof(wMinor));
		}

		ifilChunk::ifilChunk() : wMajor(0), wMinor(0) {
			naming = "ifil";
		}

		size_t isngChunk::getBodySize() {
			return soundEngine.length();
		}

		void isngChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write(soundEngine.data(), soundEngine.size());
		}

		isngChunk::isngChunk() {
			naming = "isng";
		}

		size_t INAMChunk::getBodySize() {
			return name.length();
		}

		void INAMChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write(name.data(), name.size());
		}

		INAMChunk::INAMChunk() {
			naming = "INAM";
		}

		size_t IPRDChunk::getBodySize() {
			return product.length();
		}

		void IPRDChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write(product.data(), product.size());
		}

		IPRDChunk::IPRDChunk() {
			naming = "IPRD";
		}

		size_t ISFTChunk::getBodySize() {
			return tools.length();
		}

		void ISFTChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write(tools.data(), tools.size());
		}

		ISFTChunk::ISFTChunk() {
			naming = "ISFT";
		}

		size_t smplChunk::getBodySize() {
			if (samples.size() == 0)
				throw std::exception("No samples set yet!");
			size_t allSize = 0;
			for (Wave& sample : samples)
				allSize += sample.sampleSize;
			return allSize * sizeof(int16_t);
		}

		void smplChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			for (Wave& sample : samples)
				oFile.write((const char*)sample.sample, sample.sampleSize * sizeof(int16_t));
		}

		smplChunk::smplChunk() {
			naming = "smpl";
		}

		size_t phdrChunk::getBodySize() {
			return phdr.size() * sizeof(phdrRec);
		}

		void phdrChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)phdr.data(), getBodySize());
		}

		phdrChunk::phdrChunk() {
			naming = "phdr";
		}

		size_t pbagChunk::getBodySize() {
			return pbag.size() * sizeof(pbagRec);
		}

		void pbagChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)pbag.data(), getBodySize());
		}

		pbagChunk::pbagChunk() {
			naming = "pbag";
		}

		size_t pmodChunk::getBodySize() {
			return pmod.size() * sizeof(pmodRec);
		}

		void pmodChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)pmod.data(), getBodySize());
		}

		pmodChunk::pmodChunk() {
			naming = "pmod";
		}

		size_t pgenChunk::getBodySize() {
			return pgen.size() * sizeof(pgenRec);
		}

		void pgenChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)pgen.data(), getBodySize());
		}

		pgenChunk::pgenChunk() {
			naming = "pgen";
		}

		size_t instChunk::getBodySize() {
			return inst.size() * sizeof(instRec);
		}

		void instChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)inst.data(), getBodySize());
		}

		instChunk::instChunk() {
			naming = "inst";
		}

		size_t ibagChunk::getBodySize() {
			return ibag.size() * sizeof(ibagRec);
		}

		void ibagChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)ibag.data(), getBodySize());
		}

		ibagChunk::ibagChunk() {
			naming = "ibag";
		}

		size_t imodChunk::getBodySize() {
			return imod.size() * sizeof(imodRec);
		}

		void imodChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)imod.data(), getBodySize());
		}

		imodChunk::imodChunk() {
			naming = "imod";
		}

		size_t igenChunk::getBodySize() {
			return igen.size() * sizeof(igenRec);
		}

		void igenChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)igen.data(), getBodySize());
		}

		igenChunk::igenChunk() {
			naming = "igen";
		}

		size_t shdrChunk::getBodySize() {
			return shdr.size() * sizeof(shdrRec);
		}

		void shdrChunk::save(std::ofstream& oFile) {
			Chunk::save(oFile);
			oFile.write((const char*)shdr.data(), getBodySize());
		}

		shdrChunk::shdrChunk() {
			naming = "shdr";
		}

		size_t INFOChunk::getBodySize() {
			return TopLevelChunk::getBodySize() + ifil.getSize() + isgn.getSize() + INAM.getSize() + /*IPRD.getSize() +*/ ISFT.getSize();
		}

		void INFOChunk::save(std::ofstream& oFile) {
			TopLevelChunk::save(oFile);
			ifil.save(oFile);
			isgn.save(oFile);
			INAM.save(oFile);
			//IPRD.save(oFile);
			ISFT.save(oFile);
		}

		INFOChunk::INFOChunk() {
			naming = "LIST";
			format = "INFO";
		}

		size_t sdtaChunk::getBodySize() {
			return TopLevelChunk::getBodySize() + sample.getSize();
		}

		void sdtaChunk::save(std::ofstream& oFile) {
			TopLevelChunk::save(oFile);
			sample.save(oFile);
		}

		sdtaChunk::sdtaChunk() {
			naming = "LIST";
			format = "sdta";
		}

		size_t pdtaChunk::getBodySize() {
			return TopLevelChunk::getBodySize() + phdr.getSize() + pbag.getSize() + pmod.getSize() + pgen.getSize() +
				inst.getSize() + ibag.getSize() + imod.getSize() + igen.getSize() + shdr.getSize();
		}

		void pdtaChunk::save(std::ofstream& oFile) {
			TopLevelChunk::save(oFile);
			phdr.save(oFile);
			pbag.save(oFile);
			pmod.save(oFile);
			pgen.save(oFile);
			inst.save(oFile);
			ibag.save(oFile);
			imod.save(oFile);
			igen.save(oFile);
			shdr.save(oFile);
		}

		pdtaChunk::pdtaChunk() : prepared(false) {
			naming = "LIST";
			format = "pdta";
		}

		size_t SoundFont::getBodySize() {
			return TopLevelChunk::getBodySize() + INFO.getSize() + sdta.getSize() + pdta.getSize();
		}

		void SoundFont::save(std::ofstream& oFile) {
			TopLevelChunk::save(oFile);
			INFO.save(oFile);
			sdta.save(oFile);
			pdta.save(oFile);
		}

		void SoundFont::saveSoundfont(std::string name) {
			std::ofstream oFile(name, std::ios::binary);
			save(oFile);
			oFile.close();
		}

		SoundFont::SoundFont() {
			naming = "RIFF";
			format = "sfbk";
		}

		//##################################
		//           Conversion
		//##################################
		SoundFont* SoundFont::FromVAB(VAB::VAB* vab) {
			size_t wCount = vab->Header.vs;
			size_t pCount = vab->Header.ps;

			SF2::Wave* waves = new Wave[wCount];
			int loopStart, loopEnd;
			bool looping;
			// Load Sounds
			size_t offset = vab->VAG_OffsetTable[0] << 3;
			for (int i = 0; i < wCount; i++) {
				size_t size = vab->VAG_OffsetTable[i + 1] << 3;
				waves[i].sampleSize = DecodeADPCM(vab->VAGs + offset, size, &waves[i].sample, &loopStart, &loopEnd, &looping);
				waves[i].sampleRate = 44100;
				waves[i].loopStart = loopStart;
				waves[i].loopEnd = loopEnd;
				waves[i].looping = looping;
				offset += size;
			}

			SoundFont* sf2 = new SoundFont();

			// ====================================
			// =              INFO                =
			// ====================================
			SF2::INFOChunk& info = sf2->INFO;
			info.ifil.wMajor = 2;
			info.ifil.wMinor = 1;
			info.isgn.soundEngine = "EMU8000";
			info.isgn.soundEngine.append("\0", 1);
			info.INAM.name = "Test Soundfont";
			info.ISFT.tools = "Sunnix Custom";
			info.ISFT.tools.append("\0", 1);

			// ====================================
			// =              sdta                =
			// ====================================
			SF2::sdtaChunk& sdta = sf2->sdta;
			for (int i = 0; i < wCount; i++)
				sdta.sample.samples.push_back(waves[i]);

			// ====================================
			// =              pdta                =
			// ====================================
			SF2::pdtaChunk& pdta = sf2->pdta;
			// ------------------------------------
			// -              phdr                -
			// ------------------------------------
			int pbagOffset = 0;
			for (int i = 0; i < pCount; i++) {
				SF2::phdrRec* phdr = &pdta.phdr.phdr.emplace_back();
				std::string name = std::format("Preset_{}", i);
				name = name.append("\0");
				memcpy(phdr->achPresetName, name.c_str(), name.size());
				// Midi Channel 9 is Dumkit and needs to be on Bank 128
				if (i == 9) {
					phdr->wPreset = 0;
					phdr->wBank = 128;
				} else {
					phdr->wPreset = i;
					phdr->wBank = 0;
				}
				phdr->wPresetBagNdx = pbagOffset; // points to pbag
				pbagOffset++;
				phdr->dwLibrary = 0;
				phdr->dwGenre = 0;
				phdr->dwMorphology = 0;
			}

			// ------------------------------------
			// -              pbag                -
			// ------------------------------------
			pbagOffset = 0;
			for (int i = 0; i < pCount; i++) {
				SF2::pbagRec* pbag = &pdta.pbag.pbag.emplace_back();
				pbag->wGenNdx = pbagOffset++;
			}
			SF2::pbagRec* pbag = &pdta.pbag.pbag.emplace_back();
			pbag->wGenNdx = pbagOffset;

			// ------------------------------------
			// -              pmod                -
			// ------------------------------------
			SF2::pmodRec* pmod = &pdta.pmod.pmod.emplace_back();

			// ------------------------------------
			// -              pgen                -
			// ------------------------------------
			for (int i = 0; i < pCount; i++)
				pdta.pgen.pgen.emplace_back(SF2::genInstrument, (int16_t)i);
			SF2::pgenRec* pgen = &pdta.pgen.pgen.emplace_back();

			// ------------------------------------
			// -              inst                -
			// ------------------------------------
			int instBagIndex = 0;
			for (int i = 0; i < pCount; i++) {
				SF2::instRec* inst = &pdta.inst.inst.emplace_back();
				std::string name = std::format("Instr_{}", i);
				name = name.append("\0");
				memcpy(inst->achInstName, name.c_str(), name.size());
				inst->wInstBagNdx = instBagIndex; // points to ibag
				instBagIndex += vab->ProgramAttrTable[i].tones;
			}

			// ------------------------------------
			// -              ibag                -
			// ------------------------------------
			int igenIndex = 0;
			for (int i = 0; i < pCount; i++) {
				int tones = vab->ProgramAttrTable[i].tones;
				for (int j = 0; j < tones; j++) {
					SF2::ibagRec* ibag = &pdta.ibag.ibag.emplace_back();
					ibag->wInstGenNdx = igenIndex;
					//igenIndex += 1;
					igenIndex += 13;
					if (vab->ToneAttrTable[i][j].mode == 4) // hall
						igenIndex++;
				}
			}
			SF2::ibagRec* ibag = &pdta.ibag.ibag.emplace_back();
			ibag->wInstGenNdx = igenIndex;

			// ------------------------------------
			// -              imod                -
			// ------------------------------------
			SF2::imodRec* imod = &pdta.imod.imod.emplace_back();

			// ------------------------------------
			// -              igen                -
			// ------------------------------------

			// Calculation functions
			auto calcInitialAttenuation = [](float vol) {
				if (vol == 0)
					return (int16_t)1440;
				float attenuationDb = -40.0f * std::log10(vol);
				return (int16_t)std::clamp((int)std::round(attenuationDb * 10.0f), 0, 1440);
				};

			auto seconds_to_timecents = [](float seconds) {
				if (seconds <= 0.001f) return (int16_t)-12000;
				return int16_t(1200.0f * log2(seconds));
				};

			// Convert a percent of volume value to it's attenuation in decibels.
			//  ex: ConvertPercentVolToAttenDB_SF2(0.5) returns -(-6.02db) = half perceived loudness
			auto convertPercentAmplitudeToAttenDB_SF2 = [](double percent) {
				if (percent == 0)
					return 100.0;        // assume 0 is -100.0db attenuation
				double atten = 20 * log10(percent);
				return std::min(-atten, 100.0);
				};

			// igens
			for (int i = 0; i < pCount; i++) {
				for (int j = 0; j < vab->ProgramAttrTable[i].tones; j++) {
					VAB::VagAtr& vag = vab->ToneAttrTable[i][j];
					VAB::ProgAtr prog = vab->ProgramAttrTable[vag.prog];
					SF2::Wave& wave = waves[std::max(0, vag.vag - 1)];

					// ADSR: https://problemkaputt.de/psxspx-spu-volume-and-adsr-generator.htm
					// Unused block for understanding the structure of ADSR
					auto SustainLevel = vag.adsr1 & 0xF; // (0..0Fh)  ;Level=(N+1)*800h
					auto DecayStep = -8; //(Fixed, always "-8")
					auto DecayShift = (vag.adsr1 >> 4) & 0xF; // (0..0Fh = Fast..Slow)
					// Decay Mode        (Fixed, always Exponential)
					// Decay Direction   (Fixed, always Decrease) (until Sustain Level)
					auto AttackStep = (vag.adsr1 >> 8) & 0x3; // (0..3 = "+7,+6,+5,+4")
					auto AttackShift = (vag.adsr1 >> 10) & 0x1F; // (0..1Fh = Fast..Slow)
					// Attack Direction  (Fixed, always Increase) (until Level 7FFFh)
					auto AttackMode = (vag.adsr1 >> 15); // (0=Linear, 1=Exponential)
					auto ReleaseStep = -8; // (Fixed, always "-8")
					auto ReleaseShift = vag.adsr2 & 0x1F; // (0..1Fh = Fast..Slow)
					// Release Direction (Fixed, always Decrease) (until Level 0000h)
					auto ReleaseMode = (vag.adsr2 >> 5) & 0x1; // (0=Linear, 1=Exponential)
					auto SustainStep = (vag.adsr2 >> 6) & 0x3; // (0..3 = "+7,+6,+5,+4" or "-8,-7,-6,-5") (inc/dec)
					auto SustainShift = (vag.adsr2 >> 8) & 0x1F; // (0..1Fh = Fast..Slow)
					auto NotUsed = (vag.adsr2 >> 13) & 0x1; // (should be zero)
					auto SustainDirection = (vag.adsr2 >> 14) & 0x1; // (0=Increase, 1=Decrease) (until Key OFF flag)
					auto SustainMode = (vag.adsr2 >> 15); // (0=Linear, 1=Exponential)


					double attack_time; //in seconds
					double decay_time; //in seconds
					double sustain_time; //in seconds
					double release_time; //in seconds
					double sustain_level; //as a percentage

					DecodeADSR(&vag.adsr1, &attack_time, &decay_time, &sustain_time, &release_time, &sustain_level);

					pdta.igen.igen.emplace_back(SF2::genKeyRange, vag.min, vag.max);
					//pdta.igen.igen.emplace_back(SF2::genVelRange, (int16_t)0);
					pdta.igen.igen.emplace_back(SF2::genInitialAttenuation, calcInitialAttenuation((vag.vol / 127.0f) * (prog.mvol / 127.0f) * (vab->Header.mvol / 127.0f)));
					pdta.igen.igen.emplace_back(SF2::genPan, (int16_t)(((vag.pan / 127.0f + prog.mpan / 127.0f + vab->Header.pan / 127.0f - 1.0f) - 0.5f) * 1000));
					pdta.igen.igen.emplace_back((SFGenerator)1000, vag.pbmin, vag.pbmax);

					// Looping detection
					bool hasLoop = wave.looping && wave.loopStart > 0 && wave.loopEnd > 0 && wave.loopStart < wave.loopEnd;
					pdta.igen.igen.emplace_back(SF2::genSampleModes, (int16_t)hasLoop);
					pdta.igen.igen.emplace_back(SF2::genOverrideRootKey, (int16_t)vag.center);
					pdta.igen.igen.emplace_back(SF2::genAttackVolEnv, seconds_to_timecents(attack_time));
					pdta.igen.igen.emplace_back(SF2::genHoldVolEnv, seconds_to_timecents(0.01f)); // probably to delete here
					pdta.igen.igen.emplace_back(SF2::genDecayVolEnv, seconds_to_timecents(decay_time));
					pdta.igen.igen.emplace_back(SF2::genSustainVolEnv, int16_t(convertPercentAmplitudeToAttenDB_SF2(sustain_level) * 10));
					pdta.igen.igen.emplace_back(SF2::genReleaseVolEnv, seconds_to_timecents(release_time));
					//pdta.igen.igen.emplace_back(SF2::genFineTune, (int16_t)(round(vag.shift * 0.773631676))); //doesn't seem to be right
					pdta.igen.igen.emplace_back(SF2::genFineTune, (int16_t)0); //doesn't seem to be right
					if (vag.mode == 4) // hall
						pdta.igen.igen.emplace_back(SF2::genReverbEffectsSend, (int16_t)250);

					int16_t sample = vag.vag - 1;
					if (sample < 0) {
						printf("Warning: Instrument %d - Tone %d: Invalid Sample ID: %d. Using 0 instead!\n", i, j, sample);
						sample = 0;
					}
					pdta.igen.igen.emplace_back(SF2::genSampleID, sample);
				}
			}
			SF2::igenRec* igen = &pdta.igen.igen.emplace_back();
			igen->sfGenOper = SF2::genEndOper;
			igen->genAmount.wAmount = 0;

			// ------------------------------------
			// -              shdr                -
			// ------------------------------------
			VAB::VagAtr default_ToneAttrTable;
			default_ToneAttrTable.center = 60;
			default_ToneAttrTable.shift = 64;

			offset = 0;
			for (int i = 0; i < wCount; i++) {
				SF2::Wave* wave = &waves[i];
				SF2::shdrRec* shdr = &pdta.shdr.shdr.emplace_back();
				std::string name = std::format("Sample_{}", i);

				VAB::VagAtr va = default_ToneAttrTable;
				for (int j = 0; j < vab->Header.ps; j++)
					for (int k = 0; k < vab->ProgramAttrTable[j].tones; k++)
						if (vab->ToneAttrTable[j][k].vag - 1 == i) {
							va = vab->ToneAttrTable[j][k];
							break;
						}

				name = name.append("\0");
				memcpy(shdr->achSampleName, name.c_str(), name.size());
				shdr->dwStart = offset;
				shdr->dwStartloop = offset + wave->loopStart;
				shdr->dwEndloop = offset + wave->loopEnd;
				offset += wave->sampleSize;
				shdr->dwEnd = offset;
				shdr->dwSampleRate = wave->sampleRate;
				shdr->byOriginalKey = va.center;
				shdr->chCorrection = round(va.shift * 0.773631676);
				shdr->wSampleLink = 0;
				shdr->sfSampleType = SF2::monoSample;
			}

			SF2::phdrRec* phdr = &pdta.phdr.phdr.emplace_back();
			memcpy(phdr->achPresetName, "EOP\0", 4);
			phdr->wPresetBagNdx = pbagOffset; // pbag count without this

			SF2::instRec* inst = &pdta.inst.inst.emplace_back();
			memcpy(inst->achInstName, "EOI\0", 4);
			inst->wInstBagNdx = pdta.ibag.ibag.size() - 1; // ibag count without this

			SF2::shdrRec* shdr = &pdta.shdr.shdr.emplace_back();
			memcpy(shdr->achSampleName, "EOS\0", 4);

			return sf2;
		}

	}
}

/* https://jsgroth.dev/blog/posts/ps1-spu-part-1/ */
void DecodeAdpcmBlock(const uint8_t block[16], int16_t decoded[28], int16_t* oldSmp, int16_t* olderSmpl) {
	// First byte is a header byte specifying the shift value (bits 0-3) and the filter value (bits 4-6).
	// A shift value of 13-15 is invalid and behaves the same as shift=9
	uint8_t shift = block[0] & 0x0F;
	if (shift > 12)
		shift = 9;

	// Filter values can only range from 0 to 4
	uint8_t filter = std::min(4, (block[0] >> 4) & 0x07);

	// The remaining 14 bytes are encoded sample values
	for (int sample_idx = 0; sample_idx < 28; sample_idx++) {
		// Read the raw 4-bit sample value from the block.
		// Samples are stored little-endian within a byte
		uint8_t sample_byte = block[2 + sample_idx / 2];
		uint8_t sample_nibble = (sample_byte >> (4 * (sample_idx % 2))) & 0x0F;

		// Sign extend from 4 bits to 32 bits
		int32_t raw_sample = (int8_t)((sample_nibble << 4)) >> 4;

		// Apply the shift; a shift value of N is decoded by shifting left (12 - N)
		int32_t shifted_sample = raw_sample << (12 - shift);

		// Apply the filter formula.
		// In real code you can do this with tables instead of a match
		int32_t old = static_cast<int32_t>(*oldSmp);
		int32_t older = static_cast<int32_t>(*olderSmpl);

		int32_t filtered_sample;
		switch (filter) {
		case 0: filtered_sample = shifted_sample;
			break;
		case 1: filtered_sample = shifted_sample + (60 * old + 32) / 64;
			break;
		case 2: filtered_sample = shifted_sample + (115 * old - 52 * older + 32) / 64;
			break;
		case 3: filtered_sample = shifted_sample + (98 * old - 55 * older + 32) / 64;
			break;
		case 4: filtered_sample = shifted_sample + (122 * old - 60 * older + 32) / 64;
			break;
		}

		// Finally, clamp to signed 16-bit
		int16_t clamped_sample = static_cast<int16_t>(std::clamp(filtered_sample, -0x8000, 0x7FFF));
		decoded[sample_idx] = clamped_sample;

		// Update sliding window for filter
		*olderSmpl = *oldSmp;
		*oldSmp = clamped_sample;
	}
}

const uint8_t FLAG_LOOP_END = 1;
const uint8_t FLAG_LOOP = 1 << 1;
const uint8_t FLAG_LOOP_START = 1 << 2;
const uint8_t FLAG_END = 1 << 3;

size_t DecodeADPCM(const uint8_t* data, size_t size, int16_t** outSamples, int* loopStart, int* loopEnd, bool* looping) {
	size_t blocks = size / 16;
	size_t sampleCount = blocks * 28;

	int16_t* samples = new int16_t[sampleCount];

	int16_t old1 = 0;
	int16_t old2 = 0;

	*loopStart = -1;
	*loopEnd = -1;
	*looping = false;

	size_t endBlockSize = 0;
	char* endBlock = nullptr;

	for (int i = 0; i < blocks; i++) {
		const uint8_t* block = data + 16 * i;
		uint8_t flags = block[1];

		DecodeAdpcmBlock(block, samples + 28 * i, &old1, &old2);

		if (*loopStart == -1 && (flags & FLAG_LOOP_START))
			*loopStart = i * 28;
		if (flags & FLAG_LOOP_END)
			*loopEnd = (i + 1) * 28;
		if (flags & FLAG_LOOP)
			*looping = true;

		if (flags & 1) {
			sampleCount = (i + 1) * 28;
			break;
		}
	}
	*outSamples = samples;

	if (*loopStart == -1)
		*loopStart = 0;
	if (*loopEnd == -1)
		*loopEnd = 0;

	return sampleCount;
}



int roundToZero(int val) {
	if (val < 0)
		val = 0;
	return val;
};

static unsigned long RateTable[160];
static bool bRateTableInitialized = false;

//InitADSR is shamelessly ripped from P.E.Op.S
static void initADSR() {
	// build the rate table according to Neill's rules
	memset(RateTable, 0, sizeof(unsigned long) * 160);

	uint32_t r = 3;
	uint32_t rs = 1;
	uint32_t rd = 0;

	// we start at pos 32 with the real values... everything before is 0
	for (int i = 32; i < 160; i++) {
		if (r < 0x3FFFFFFF) {
			r += rs;
			rd++;
			if (rd == 5) {
				rd = 1;
				rs *= 2;
			}
		}
		if (r > 0x3FFFFFFF)
			r = 0x3FFFFFFF;

		RateTable[i] = r;
	}
}

static double linearAmpDecayTimeToLinDBDecayTime(double secondsToFullAtten, int linearVolumeRange) {
	double expMinDecibel = -100.0;
	double linearMinDecibel = log10(1.0 / linearVolumeRange) * 20.0;
	double linearToExpScale = log(linearMinDecibel - expMinDecibel) / log(2.0);
	return secondsToFullAtten * linearToExpScale;
}

/// <summary>
/// Function from VGMTrans
/// </summary>
void DecodeADSR(uint16_t* adsr, double* attack_time, double* decay_time, double* sustain_time, double* release_time, double* sustain_level) {
	uint8_t Am = (adsr[0] & 0x8000) >> 15;
	uint8_t Ar = (adsr[0] & 0x7F00) >> 8;
	uint8_t Dr = (adsr[0] & 0x00F0) >> 4;
	uint8_t Sl = adsr[0] & 0x000F;
	uint8_t Rm = (adsr[1] & 0x0020) >> 5;
	uint8_t Rr = adsr[1] & 0x001F;

	uint8_t Sm = (adsr[1] & 0x8000) >> 15;
	uint8_t Sd = (adsr[1] & 0x4000) >> 14;
	uint8_t Sr = (adsr[1] >> 6) & 0x7F;

	double sampleRate = 44100;
	double samples{ 0 };
	int l;

	if (!bRateTableInitialized) {
		initADSR();
		bRateTableInitialized = true;
	}

	// to get the dls 32 bit time cents, take log base 2 of number of seconds * 1200 * 65536
	// (dls1v11a.pdf p25).

  //	if (RateTable[(Ar^0x7F)-0x10 + 32] == 0)
  //		realADSR->attack_time = 0;
  //	else
  //	{
	if ((Ar ^ 0x7F) < 0x10)
		Ar = 0;
	// if linear Ar Mode
	if (Am == 0) {
		uint32_t rate = RateTable[roundToZero((Ar ^ 0x7F) - 0x10) + 32];
		samples = ceil(0x7FFFFFFF / static_cast<double>(rate));
	} else if (Am == 1) {
		uint32_t rate = RateTable[roundToZero((Ar ^ 0x7F) - 0x10) + 32];
		samples = 0x60000000 / rate;
		uint32_t remainder = 0x60000000 % rate;
		rate = RateTable[roundToZero((Ar ^ 0x7F) - 0x18) + 32];
		samples += ceil(fmax(0, 0x1FFFFFFF - remainder) / static_cast<double>(rate));
	}
	*attack_time = samples / sampleRate;
	//	}

	  // Decay Time

	long envelope_level = 0x7FFFFFFF;

	bool bSustainLevFound = false;
	uint32_t realSustainLevel{ 0 };
	// DLS decay rate value is to -96db (silence) not the sustain level
	for (l = 0; envelope_level > 0; l++) {
		if (4 * (Dr ^ 0x1F) < 0x18)
			Dr = 0;
		switch ((envelope_level >> 28) & 0x7) {
		case 0: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 0) + 32]; break;
		case 1: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 4) + 32]; break;
		case 2: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 6) + 32]; break;
		case 3: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 8) + 32]; break;
		case 4: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 9) + 32]; break;
		case 5: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 10) + 32]; break;
		case 6: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 11) + 32]; break;
		case 7: envelope_level -= RateTable[roundToZero((4 * (Dr ^ 0x1F)) - 0x18 + 12) + 32]; break;
		default: break;
		}
		if (!bSustainLevFound && ((envelope_level >> 27) & 0xF) <= Sl) {
			realSustainLevel = envelope_level;
			bSustainLevFound = true;
		}
	}
	samples = l;
	*decay_time = samples / sampleRate;

	// Sustain Rate

	envelope_level = 0x7FFFFFFF;
	// increasing... we won't even bother
	if (Sd == 0) {
		*sustain_time = -1;
	} else {
		if (Sr == 0x7F)
			*sustain_time = -1;        // this is actually infinite
		else {
			// linear
			if (Sm == 0) {
				uint32_t rate = RateTable[roundToZero((Sr ^ 0x7F) - 0x0F) + 32];
				samples = ceil(0x7FFFFFFF / static_cast<double>(rate));
			} else {
				l = 0;
				// DLS decay rate value is to -96db (silence) not the sustain level
				while (envelope_level > 0) {
					long envelope_level_diff{ 0 };
					long envelope_level_target{ 0 };

					switch ((envelope_level >> 28) & 0x7) {
					case 0: envelope_level_target = 0x00000000; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 0) + 32]; break;
					case 1: envelope_level_target = 0x0fffffff; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 4) + 32]; break;
					case 2: envelope_level_target = 0x1fffffff; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 6) + 32]; break;
					case 3: envelope_level_target = 0x2fffffff; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 8) + 32]; break;
					case 4: envelope_level_target = 0x3fffffff; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 9) + 32]; break;
					case 5: envelope_level_target = 0x4fffffff; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 10) + 32]; break;
					case 6: envelope_level_target = 0x5fffffff; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 11) + 32]; break;
					case 7: envelope_level_target = 0x6fffffff; envelope_level_diff = RateTable[roundToZero((Sr ^ 0x7F) - 0x1B + 12) + 32]; break;
					default: break;
					}

					long steps = (envelope_level - envelope_level_target + (envelope_level_diff - 1)) / envelope_level_diff;
					envelope_level -= (envelope_level_diff * steps);
					l += steps;
				}
				samples = l;
			}
			double timeInSecs = samples / sampleRate;
			*sustain_time = /*Sm ? timeInSecs : */linearAmpDecayTimeToLinDBDecayTime(timeInSecs, 0x800);
		}
	}

	// Sustain Level
	//realADSR->sustain_level = (double)envelope_level/(double)0x7FFFFFFF;//(long)ceil((double)envelope_level * 0.030517578139210854);	//in DLS, sustain level is measured as a percentage
	if (Sl == 0)
		realSustainLevel = 0x07FFFFFF;
	*sustain_level = realSustainLevel / static_cast<double>(0x7FFFFFFF);

	// If decay is going unused, and there's a sustain rate with sustain level close to max...
	//  we'll put the sustain_rate in place of the decay rate.
	if ((*decay_time < 2 || (Dr == 0x0F && Sl >= 0x0C)) && Sr < 0x7E && Sd == 1) {
		*sustain_level = 0;
		*decay_time = *sustain_time;
		//realADSR->decay_time = 0.5;
	}

	// Release Time

	//sustain_envelope_level = envelope_level;

	// We do this because we measure release time from max volume to 0, not from sustain level to 0
	envelope_level = 0x7FFFFFFF;

	// if linear Rr Mode
	if (Rm == 0) {
		uint32_t rate = RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x0C) + 32];

		if (rate != 0)
			samples = ceil(static_cast<double>(envelope_level) / rate);
		else
			samples = 0;
	} else if (Rm == 1) {
		if ((Rr ^ 0x1F) * 4 < 0x18)
			Rr = 0;
		for (l = 0; envelope_level > 0; l++) {
			switch ((envelope_level >> 28) & 0x7) {
			case 0: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 0) + 32]; break;
			case 1: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 4) + 32]; break;
			case 2: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 6) + 32]; break;
			case 3: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 8) + 32]; break;
			case 4: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 9) + 32]; break;
			case 5: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 10) + 32]; break;
			case 6: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 11) + 32]; break;
			case 7: envelope_level -= RateTable[roundToZero((4 * (Rr ^ 0x1F)) - 0x18 + 12) + 32]; break;
			default: break;
			}
		}
		samples = l;
	}
	double timeInSecs = samples / sampleRate;

	//theRate = timeInSecs / sustain_envelope_level;
	//timeInSecs = 0x7FFFFFFF * theRate;	//the release time value is more like a rate.  It is the time from max value to 0, not from sustain level.
	//if (Rm == 0) // if it's linear
	//	timeInSecs *=  LINEAR_RELEASE_COMPENSATION;

	*release_time = /*Rm ? timeInSecs : */linearAmpDecayTimeToLinDBDecayTime(timeInSecs, 0x800);

	// We need to compensate the decay and release times to represent them as the time from full vol to -100db
	// where the drop in db is a fixed amount per time unit (SoundFont2 spec for vol envelopes, pg44.)
	//  We assume the psx envelope is using a linear scale wherein envelope_level / 2 == half loudness.
	//  For a linear release mode (Rm == 0), the time to reach half volume is simply half the time to reach 0.
	// Half perceived loudness is -10db. Therefore, time_to_half_vol * 10 == full_time * 5 == the correct SF2 time
	//realADSR->decay_time = LinAmpDecayTimeToLinDBDecayTime(realADSR->decay_time, 0x800);
	//realADSR->sustain_time = LinAmpDecayTimeToLinDBDecayTime(realADSR->sustain_time, 0x800);
	//realADSR->release_time = LinAmpDecayTimeToLinDBDecayTime(realADSR->release_time, 0x800);

	// Calculations are done, so now add the articulation data
	//artic->AddADSR(attack_time, Am, decay_time, sustain_lev, release_time, 0);
	return;
}