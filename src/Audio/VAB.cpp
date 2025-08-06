#include "VAB.h"
#include <cstring>

uint8_t readLE8(uint8_t** ptr) {
    uint8_t value = **ptr;
    *ptr += 1;
    return value;
}

uint16_t readLE16(uint8_t** ptr) {
    uint8_t* data = *ptr;
    uint16_t value = data[0] | (data[1] << 8);
    *ptr += 2;
    return value;
}

uint32_t readLE32(uint8_t** ptr) {
    uint8_t* data = *ptr;
    uint32_t value = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    *ptr += 4;
    return value;
}

namespace Alundra {
    namespace VAB {

        VAB* setupVAB(uint8_t* data, int meta) {
            size_t dataStartPos = (size_t)data; // for 0x800 cdrom padding calculation

            VAB* vab = new VAB;

            // Header
            std::memcpy(vab->Header.form, "VABp", 4);
            data += 4;
            vab->Header.ver = readLE32(&data);
            vab->Header.id = readLE32(&data);
            vab->Header.fsize = readLE32(&data);
            vab->Header.reserved0 = readLE16(&data);
            vab->Header.ps = readLE16(&data);
            vab->Header.ts = readLE16(&data);
            vab->Header.vs = readLE16(&data);
            vab->Header.mvol = readLE8(&data);
            vab->Header.pan = readLE8(&data);
            vab->Header.attr1 = readLE8(&data);
            vab->Header.attr2 = readLE8(&data);
            vab->Header.reserved1 = readLE32(&data);

            // Program Attribute Table
            for (int i = 0; i < 128; i++) {
                ProgAtr& atr = vab->ProgramAttrTable[i];
                atr.tones = readLE8(&data);
                atr.mvol = readLE8(&data);
                atr.prior = readLE8(&data);
                atr.mode = readLE8(&data);
                atr.mpan = readLE8(&data);
                atr.reserved0 = readLE8(&data);
                atr.attr = readLE16(&data);
                atr.reserved1 = readLE32(&data);
                atr.reserved2 = readLE32(&data);
            }

            // Tone Attribute Table
            for (int i = 0; i < vab->Header.ps; i++) {
                for (int j = 0; j < 16; j++) {
                    VagAtr& atr = vab->ToneAttrTable[i][j];
                    atr.prior = readLE8(&data);
                    atr.mode = readLE8(&data);
                    atr.vol = readLE8(&data);
                    atr.pan = readLE8(&data);
                    atr.center = readLE8(&data);
                    atr.shift = readLE8(&data);
                    atr.min = readLE8(&data);
                    atr.max = readLE8(&data);
                    atr.vibW = readLE8(&data);
                    atr.vibT = readLE8(&data);
                    atr.porW = readLE8(&data);
                    atr.porT = readLE8(&data);
                    atr.pbmin = readLE8(&data);
                    atr.pbmax = readLE8(&data);
                    atr.reserved1 = readLE8(&data);
                    atr.reserved2 = readLE8(&data);
                    atr.adsr1 = readLE16(&data);
                    atr.adsr2 = readLE16(&data);
                    atr.prog = readLE16(&data);
                    atr.vag = readLE16(&data);
                    for (int k = 0; k < 4; k++)
                        atr.reserved[k] = readLE16(&data);
                }
            }

            // VAG Offset Table
            for (int i = 0; i < 256; i++) {
                uint16_t offset = readLE16(&data);
                vab->VAG_OffsetTable[i] = offset;
            }

            //printf("VAG's (%d):\n", vab->Header.vs);

            // cd rom padding shift
            size_t offset = (size_t)data - dataStartPos;
            size_t alignShift = offset % 0x800;
            if (alignShift != 0)
                data += 0x800 - alignShift;

            data += (uint32_t)(vab->VAG_OffsetTable[0] << 3);
            size_t sizeToRead = 0;
            for (int i = 1; i < 256; i++)
                sizeToRead += vab->VAG_OffsetTable[i] << 3;
            vab->VAGs = new uint8_t[sizeToRead];
            memcpy(vab->VAGs, data, sizeToRead);

            return vab;
        }

    }
}