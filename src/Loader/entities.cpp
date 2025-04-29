#include "entities.h"
#include <cstdio>
#include <cstdlib>

/*  _____                                     _____  */
/* ( ___ )                                   ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  _____       _   _ _   _            |   |  */
/*  |   | | ____|_ __ | |_(_) |_(_) ___  ___  |   |  */
/*  |   | |  _| | '_ \| __| | __| |/ _ \/ __| |   |  */
/*  |   | | |___| | | | |_| | |_| |  __/\__ \ |   |  */
/*  |   | |_____|_| |_|\__|_|\__|_|\___||___/ |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                                   (_____) */

Entities::Entities(char* data, char* texPtr)
{
    _data = data;
    _header = (EntitiesHeader*) _data;

    // Set the textures

    _spriteSheet = new Texture();
    _spriteSheet->TexFromData(texPtr, GetPalPtr(), GetPalSize());

    // Create objects from the table

    uint32_t* index = (uint32_t*) (_data + _header->IndexPtr);

    for (int i = 0; i < 256; i++)
    {
        if (index[i] != 0xFFFFFFFF)
        {
            _objects[i] = new Object(_data, index[i], i);
        }
        else
        {
            _objects[i] = NULL;
        }
    }
}

void Entities::DrawSprite(Drawer* const& drawer, Point pos, uint8_t obj, uint16_t spritePtr)
{
    uint8_t* dataPtr = (uint8_t*) (_objects[obj]->GetSprites() + (spritePtr << 1));
    uint8_t subCount = dataPtr[1];
    subCount += subCount == 0;

    // Draw each subsprite
    for (int i = subCount - 1; i >= 0; i--)
    {
        SubSprite* sub = (SubSprite*) (dataPtr + (i * sizeof(SubSprite)) + 2);
        uint32_t sheet = (sub->SheetDex & 7) << 8;
        int bmode = (sub->SheetDex & 8) * (((sub->SheetDex >> 4) & 3) + 1); // Blend mode

        // Correction for modern GPU
        // Whenever the sprite is flipped, srcPos is shifted by one pixel
        bool flipX = sub->XTRStr < sub->XTLStr;
        bool flipY = sub->YBLStr < sub->YTLStr;

        Rect rect =
        {
            sub->X + flipX, sheet + sub->Y + flipY,
            sub->Width, sub->Height
        };

        Quad quad =
        {
            { sub->XTLStr + pos.X, sub->YTLStr + pos.Y },
            { sub->XTRStr + pos.X, sub->YTRStr + pos.Y },
            { sub->XBRStr + pos.X, sub->YBRStr + pos.Y },
            { sub->XBLStr + pos.X, sub->YBLStr + pos.Y },
        };

        drawer->AddBatch(quad, rect, 1, sub->Palette, bmode);
    }
}

Entities::~Entities()
{
    delete _spriteSheet;

    for (int i = 0; i < 256; i++)
    {
        delete _objects[i];
    }
}

/*  _____                               _____  */
/* ( ___ )                             ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |   ___  _     _           _    |   |  */
/*  |   |  / _ \| |__ (_) ___  ___| |_  |   |  */
/*  |   | | | | | '_ \| |/ _ \/ __| __| |   |  */
/*  |   | | |_| | |_) | |  __/ (__| |_  |   |  */
/*  |   |  \___/|_.__// |\___|\___|\__| |   |  */
/*  |   |           |__/                |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                             (_____) */

Object::Object(char* basePtr, uint32_t dataPtr, uint8_t dexID)
{
    _base = basePtr;
    _data = (char*) (_base + dataPtr);
    _header = (ObjectHeader*) _data;
    _dexID = dexID;
}

Object::~Object()
{

}
