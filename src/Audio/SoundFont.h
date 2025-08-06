#pragma once
#include "VAB.h"
#include <vector>
#include <fstream>

namespace Alundra {
	namespace SF2 {

		//##################################
		//           types
		//##################################
		struct rangesType {
			uint8_t byLo;			// Lower bound of range (e.g., lowest key or velocity)
			uint8_t byHi;			// Upper bound of range (e.g., highest key or velocity)
		};

		union genAmountType {
			rangesType ranges;		// Used for key/velocity ranges (e.g., keyRange, velRange)
			int16_t shAmount;		// Signed short (used for pitch, tuning, attenuation, etc.)
			uint16_t wAmount;		// Unsigned short (used for some IDs or flags)
		};

		enum SFSampleLink : uint16_t {
			monoSample = 1,			// Mono sample
			rightSample = 2,		// Right channel of stereo pair
			leftSample = 4,			// Left channel of stereo pair
			linkedSample = 8,		// Linked sample, e.g. part of split or articulation

			// ROM-based versions (used in SoundFont ROM banks)
			RomMonoSample = 0x8001,
			RomRightSample = 0x8002,
			RomLeftSample = 0x8004,
			RomLinkedSample = 0x8008
		};

		enum SFGenerator : uint16_t {
			genStartAddrOffset = 0,
			genEndAddrOffset = 1,
			genStartLoopAddrOffset = 2,
			genEndLoopAddrOffset = 3,
			genStartAddrCoarseOffset = 4,
			genModLfoToPitch = 5,
			genVibLfoToPitch = 6,
			genModEnvToPitch = 7,
			genInitialFilterFc = 8,
			genInitialFilterQ = 9,
			genModLfoToFilterFc = 10,
			genModEnvToFilterFc = 11,
			genEndAddrCoarseOffset = 12,
			genModLfoToVolume = 13,
			genUnused1 = 14,
			genChorusEffectsSend = 15,
			genReverbEffectsSend = 16,
			genPan = 17,
			genUnused2 = 18,
			genUnused3 = 19,
			genUnused4 = 20,
			genDelayModLFO = 21,
			genFreqModLFO = 22,
			genDelayVibLFO = 23,
			genFreqVibLFO = 24,
			genDelayModEnv = 25,
			genAttackModEnv = 26,
			genHoldModEnv = 27,
			genDecayModEnv = 28,
			genSustainModEnv = 29,
			genReleaseModEnv = 30,
			genKeynumToModEnvHold = 31,
			genKeynumToModEnvDecay = 32,
			genDelayVolEnv = 33,
			genAttackVolEnv = 34,
			genHoldVolEnv = 35,
			genDecayVolEnv = 36,
			genSustainVolEnv = 37,
			genReleaseVolEnv = 38,
			genKeynumToVolEnvHold = 39,
			genKeynumToVolEnvDecay = 40,
			genInstrument = 41,				// Points to an instrument in `inst`
			genReserved1 = 42,
			genKeyRange = 43,				// uses rangesType
			genVelRange = 44,				// uses rangesType
			genStartLoopAddrCoarseOffset = 45,
			genKeynum = 46,
			genVelocity = 47,
			genInitialAttenuation = 48,
			genReserved2 = 49,
			genEndLoopAddrCoarseOffset = 50,
			genCoarseTune = 51,
			genFineTune = 52,
			genSampleID = 53,				// Points to a sample in `shdr`
			genSampleModes = 54,
			genReserved3 = 55,
			genScaleTuning = 56,
			genExclusiveClass = 57,
			genOverrideRootKey = 58,
			genUnused5 = 59,
			genEndOper = 60					// Marks the end (always present)
		};

		enum SFModulator : uint16_t {
			modNone = 0,					// No modulation
			modNoteOnVelocity = 2,			// Note-on velocity
			modNoteOnKeyNumber = 3,			// Note-on key number
			modPolyPressure = 10,			// Polyphonic key pressure
			modChannelPressure = 13,		// Channel pressure
			modPitchWheel = 14,				// Pitch wheel
			modPitchWheelSensitivity = 16,  // Pitch wheel sensitivity
			modLink = 127					// Used internally to link modulators
		};

		enum SFTransform : uint16_t {
			transLinear = 0,				// Linear transform (default)
			transAbsoluteValue = 2			// Absolute value (|x| transform)
		};

		struct iverRec {
			uint16_t wMajor;				// Major version (e.g., 2)
			uint16_t wMinor;				// Minor version (e.g., 1)
		};

		struct phdrRec {
			int8_t achPresetName[20];		// Preset name (ASCII, null-terminated if shorter than 20 bytes)
			uint16_t wPreset;				// Preset number (0–127, corresponds to MIDI program number)
			uint16_t wBank;					// MIDI bank number (e.g., 0 for GM)
			uint16_t wPresetBagNdx;			// Index into the pbag list (first zone of this preset)
			uint32_t dwLibrary;				// Reserved (always 0, ignored)
			uint32_t dwGenre;				// Reserved (always 0, ignored)
			uint32_t dwMorphology;			// Reserved (always 0, ignored)
		};

		struct pbagRec {
			uint16_t wGenNdx;				// Index into the pgen list (first generator of this zone)
			uint16_t wModNdx;				// Index into the pmod list (first modulator of this zone)
		};

		struct pmodRec {
			SFModulator sfModSrcOper;      // Source modulator (e.g. velocity, controller)
			SFGenerator sfModDestOper;     // Destination generator affected by this modulator
			int16_t modAmount;             // Amount of modulation
			SFModulator sfModAmtSrcOper;   // Modulator that modifies the amount of this modulator
			SFTransform sfModTransOper;    // Transform applied to source (e.g. linear, concave)
		};

		struct pgenRec {
			SFGenerator sfGenOper;         // Generator type (e.g. keyRange, instrument ID)
			genAmountType genAmount;       // Value for this generator (can be a range or fixed)

			pgenRec() {
				sfGenOper = (SFGenerator)0;
				genAmount.shAmount = 0;
			}

			pgenRec(SFGenerator sfGenOper, int16_t shAmount) : sfGenOper(sfGenOper) {
				genAmount.shAmount = shAmount;
			}

			pgenRec(SFGenerator sfGenOper, uint16_t wAmount) : sfGenOper(sfGenOper) {
				genAmount.wAmount = wAmount;
			}

			pgenRec(SFGenerator sfGenOper, uint8_t byLo, uint8_t byHi) : sfGenOper(sfGenOper) {
				genAmount.ranges.byLo = byLo;
				genAmount.ranges.byHi = byHi;
			}
		};

		struct instRec {
			int8_t achInstName[20];			// Instrument name (ASCII, null-terminated if shorter than 20 bytes)
			uint16_t wInstBagNdx;			// Index into the ibag list (first zone of this instrument)
		};

		struct ibagRec {
			uint16_t wInstGenNdx;			// Index into the igen list (first generator of this zone)
			uint16_t wInstModNdx;			// Index into the imod list (first modulator of this zone)
		};

		struct imodRec {
			SFModulator sfModSrcOper;		// Source modulator
			SFGenerator sfModDestOper;		// Destination generator
			int16_t modAmount;				// Modulation amount
			SFModulator sfModAmtSrcOper;	// Amount source modulator
			SFTransform sfModTransOper;		// Transform operation on the source modulator
		};

		struct igenRec {
			SFGenerator sfGenOper;         // Generator type
			genAmountType genAmount;       // Generator value (range or fixed)

			igenRec() {
				sfGenOper = (SFGenerator)0;
				genAmount.shAmount = 0;
			}

			igenRec(SFGenerator sfGenOper, int16_t shAmount) : sfGenOper(sfGenOper) {
				genAmount.shAmount = shAmount;
			}

			igenRec(SFGenerator sfGenOper, uint16_t wAmount) : sfGenOper(sfGenOper) {
				genAmount.wAmount = wAmount;
			}

			igenRec(SFGenerator sfGenOper, uint8_t byLo, uint8_t byHi) : sfGenOper(sfGenOper) {
				genAmount.ranges.byLo = byLo;
				genAmount.ranges.byHi = byHi;
			}
		};

		struct shdrRec {
			int8_t achSampleName[20];		// Sample name (ASCII, null-terminated)
			uint32_t dwStart;				// Sample start index (in sample data)
			uint32_t dwEnd;					// Sample end index (non-inclusive)
			uint32_t dwStartloop;			// Loop start index
			uint32_t dwEndloop;				// Loop end index
			uint32_t dwSampleRate;			// Sample rate (Hz)
			uint8_t byOriginalKey;			// MIDI root key (e.g. 60 = middle C)
			int8_t chCorrection;			// Tuning adjustment (cents)
			uint16_t wSampleLink;			// Sample link index (used for stereo)
			SFSampleLink sfSampleType;		// Sample type (mono, stereo left/right, etc.)
		};

		struct Wave {
			size_t sampleSize;
			int16_t* sample;
			unsigned int sampleRate;
			size_t loopStart;
			size_t loopEnd;
			bool looping;
		};

		//##################################
		//           classes
		//##################################

		class Chunk {
		public:
			std::string naming;

			/// <summary>
			/// Returns size in bytes
			/// </summary>
			size_t getSize();
			/// <summary>
			/// Get only the Size from the body
			/// </summary>
			virtual size_t getBodySize() = 0;
			virtual void save(std::ofstream& oFile);
		};

		class TopLevelChunk : public Chunk {
		public:
			std::string format;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;
		};

		class ifilChunk : public Chunk {
		public:
			uint16_t wMajor;
			uint16_t wMinor;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			ifilChunk();
		};

		class isngChunk : public Chunk {
		public:
			std::string soundEngine;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			isngChunk();
		};

		class INAMChunk : public Chunk {
		public:
			std::string name;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			INAMChunk();
		};

		class IPRDChunk : public Chunk {
		public:
			std::string product;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			IPRDChunk();
		};

		class ISFTChunk : public Chunk {
		public:
			std::string tools;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			ISFTChunk();
		};

		class smplChunk : public Chunk {
		public:
			std::vector<Wave> samples;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			smplChunk();
		};

		class phdrChunk : public Chunk {
		public:
			std::vector<phdrRec> phdr;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			phdrChunk();
		};

		class pbagChunk : public Chunk {
		public:
			std::vector<pbagRec> pbag;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			pbagChunk();
		};

		class pmodChunk : public Chunk {
		public:
			std::vector<pmodRec> pmod;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			pmodChunk();
		};

		class pgenChunk : public Chunk {
		public:
			std::vector<pgenRec> pgen;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			pgenChunk();
		};

		class instChunk : public Chunk {
		public:
			std::vector<instRec> inst;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			instChunk();
		};

		class ibagChunk : public Chunk {
		public:
			std::vector<ibagRec> ibag;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			ibagChunk();
		};

		class imodChunk : public Chunk {
		public:
			std::vector<imodRec> imod;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			imodChunk();
		};

		class igenChunk : public Chunk {
		public:
			std::vector<igenRec> igen;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			igenChunk();
		};

		class shdrChunk : public Chunk {
		public:
			std::vector<shdrRec> shdr;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			shdrChunk();
		};

		class INFOChunk : public TopLevelChunk {
		public:
			ifilChunk ifil; // Refers to the version of the Sound Font RIFF file 
			isngChunk isgn; // Refers to the target Sound Engine 
			INAMChunk INAM; // Refers to the Sound Font Bank Name
			//iromChunk irom; // Refers to the Sound ROM Name [optional]
			//iverChunk iver; // Refers to the Sound ROM Version [optional]
			//ICRDChunk ICRD; // Refers to the Date of Creation of the Bank [optional]
			//IENGChunk IENG; // Sound Designers and Engineers for the Bank [optional]
			//IPRDChunk IPRD; // Product for which the Bank was intended [optional]
			//ICOPChunk ICOP; // Contains any Copyright message [optional]
			//ICMTChunk ICMT; // Contains any Comments on the Bank [optional]
			ISFTChunk ISFT; // The SoundFont tools used to create and alter the bank [optional]

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			INFOChunk();
		};

		class sdtaChunk : public TopLevelChunk {
		public:
			smplChunk sample;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			sdtaChunk();
		};

		class pdtaChunk : public TopLevelChunk {
		private:
			bool prepared; // add chunk end information
		public:
			phdrChunk phdr; // The Preset Headers
			pbagChunk pbag; // The Preset Index list
			pmodChunk pmod; // The Preset Modulator list
			pgenChunk pgen; // The Preset Generator list
			instChunk inst; // The Instrument Names and Indices
			ibagChunk ibag; // The Instrument Index list
			imodChunk imod; // The Instrument Modulator list
			igenChunk igen; // The Instrument Generator list
			shdrChunk shdr; // The Sample Headers

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			pdtaChunk();
		};

		class SoundFont : public TopLevelChunk {
		public:
			INFOChunk INFO;
			sdtaChunk sdta;
			pdtaChunk pdta;

			size_t getBodySize() override;
			void save(std::ofstream& oFile) override;

			void saveSoundfont(std::string name);

			SoundFont();

			static SoundFont* FromVAB(VAB::VAB* vab);
		};

	}
}