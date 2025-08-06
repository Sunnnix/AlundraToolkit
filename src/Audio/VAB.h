#pragma once
#include <cstdint>

namespace Alundra {
    namespace VAB {

        struct VabHdr {
            char form[4];				// always “VABp”
            int32_t ver;				// format version number
            int32_t id;					// bank ID
            uint32_t fsize;				// file size
            uint16_t reserved0;			// system reserved
            uint16_t ps;				// total number of programs in this bank
            uint16_t ts;				// total number of effective tones
            uint16_t vs;				// number of waveforms (VAG)
            uint8_t mvol;				// master volume
            uint8_t pan;				// master pan
            uint8_t attr1;				// bank attribute
            uint8_t attr2;				// bank attribute
            uint32_t reserved1;			// system reserved
        };

#pragma pack(push, 1)
        struct ProgAtr {
            uint8_t tones;				// number of effective tones which compose the program
            uint8_t mvol;				// program volume
            uint8_t prior;				// program priority
            uint8_t mode;				// program mode
            uint8_t mpan;				// program pan
            int8_t reserved0;			// system reserved
            int16_t attr;				// program attribute
            uint32_t reserved1;			// system reserved
            uint32_t reserved2;			// system reserved
        };

#pragma pack(push, 1)
        struct VagAtr {
            uint8_t prior;				// tone priority (0 – 127); used for controlling allocation when more voices than can be keyed on are requested
            uint8_t mode;				// tone mode (0 = normal; 4 = reverb applied 
            uint8_t vol;				// tone volume
            uint8_t pan;				// tone pan
            uint8_t center;				// center note (0~127)
            uint8_t shift;				// pitch correction (0~127,cent units)
            uint8_t min;				// minimum note limit (0~127)
            uint8_t max;				// maximum note limit (0~127, provided min < max)
            uint8_t vibW;				// vibrato width (1/128 rate,0~127)
            uint8_t vibT;				// 1 cycle time of vibrato (tick units)
            uint8_t porW;				// portamento width (1/128 rate, 0~127)
            uint8_t porT;				// portamento holding time (tick units)
            uint8_t pbmin;				// pitch bend (-0~127, 127 = 1 octave)
            uint8_t pbmax;				// pitch bend (+0~127, 127 = 1 octave)
            uint8_t reserved1;			// system reserved
            uint8_t reserved2;			// system reserved
            uint16_t adsr1;				// ADSR1
            uint16_t adsr2;				// ADSR2
            int16_t prog;				// parent program
            int16_t vag;				// waveform (VAG) used
            int16_t reserved[4];		// system reserved
        };

        struct Sample {
            uint32_t size;
            uint8_t* data;
        };

        struct VAB {
            VabHdr Header;
            ProgAtr ProgramAttrTable[128];	// 16 x 128 (Max programs)
            VagAtr ToneAttrTable[128][16];	// 32 x 16 (Max tones) x number of programs
            uint16_t VAG_OffsetTable[256];
            uint8_t* VAGs;
        };

        VAB* setupVAB(uint8_t* data, int meta = 0);
    }
}