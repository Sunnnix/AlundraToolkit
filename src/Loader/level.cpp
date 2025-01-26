#include <cstdlib>
#include <cstdio>
#include "level.h"

/*  _____                         _____  */
/* ( ___ )                       ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  _                   _  |   |  */
/*  |   | | |    _____   _____| | |   |  */
/*  |   | | |   / _ \ \ / / _ \ | |   |  */
/*  |   | | |__|  __/\ V /  __/ | |   |  */
/*  |   | |_____\___| \_/ \___|_| |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                       (_____) */

Level::Level(char* ldata, uint32_t lsize)
{
    _data = ldata;
    _header = (LevelHeader*) _data;
    _dataSize = lsize;

    _tileSet = new Texture();
    _tileSet->SetTexture(GetTileset(), GetProperties()->Palettes);
}

Level::~Level()
{
    free(_data);
}
