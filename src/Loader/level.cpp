#include "level.h"
#include <cstdlib>
#include <cstdio>
#include <algorithm>

/*  _____                               _____  */
/* ( ___ )                             ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  _     _       _              |   |  */
/*  |   | | |   (_)_ __ (_)_ __   __ _  |   |  */
/*  |   | | |   | | '_ \| | '_ \ / _` | |   |  */
/*  |   | | |___| | | | | | | | | (_| | |   |  */
/*  |   | |_____|_|_| |_|_|_| |_|\__, | |   |  */
/*  |   |                        |___/  |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                             (_____) */

Lining::Lining(char* liningData, uint32_t liningSize)
{
    _data = liningData;
    _header = (LiningHeader*) _data;
    _tileSet = nullptr;
    _dataSize = liningSize;
}

int Lining::Init(Drawer* drawer)
{
    _infos = (LiningInfos*) (_data + 28);

    // Return if disabled
    if (!_infos->Enabled)
    {
        return 0;
    }

    // Load graphics
    _drawer = drawer;
    _tileSet = new Texture();
    _tileSet->TexFromData(_data + _header->Graphics + 256, (uint16_t*) (_data + _header->Graphics), 256);
    _drawer->LoadTexture(_tileSet->GetDataBits(), _tileSet->GetHeight(), 3);
    _drawer->LoadPalette(_tileSet->GetPalette()->GetData(), _tileSet->GetPalette()->GetCount(), 3);

    for (int layerID = 0; layerID < 2; layerID++)
    {
        _animFrameTimer[layerID] = 0;
        _animFrameCounter[layerID] = 0;
        _parallaxOffsetX[layerID] = 0;
        _parallaxOffsetY[layerID] = 0;
        _offsetX[layerID] = 0;
        _offsetY[layerID] = 0;
        _timerX[layerID] = 0;
        _timerY[layerID] = 0;

        switch (_infos->ModeLayer[layerID])
        {
            case 1 : InitScrollar(0); break;
            case 2 : InitCellular(0); break;
            default : break;
        }
    }

    if (_infos->BGColor.Alpha != 0)
    {
        // ADD FUNCTION
    }

    return 0;
}

void Lining::InitScrollar(int layerID)
{
    Scrollar* scrollar = (Scrollar*) (_data + _header->Layers[layerID] + 4);

    // Init scrollDirX
    if ((scrollar->ScrollXSpeed >= 0) && (scrollar->ScrollXPeriod < 0))
    {
        _scrollDirX[layerID] = -1;
    }
    else
    {
        _scrollDirX[layerID] = 1;
    }

    if (scrollar->ScrollXPeriod == 0)
    {
        _scrollDirX[layerID] = 0;
    }

    // Init scrollDirX
    if ((scrollar->ScrollYSpeed >= 0) && (scrollar->ScrollYPeriod < 0))
    {
        _scrollDirY[layerID] = -1;
    }
    else
    {
        _scrollDirY[layerID] = 1;
    }

    if (scrollar->ScrollYPeriod == 0)
    {
        _scrollDirY[layerID] = 0;
    }
}

int Lining::InitCellular(int layerID)
{
    // HIC SUNT DRACONES

    return 0;
}

int Lining::DrawLining(int camPosX, int camPosY)
{
    int primCount = 0;

    if (!_infos->Enabled)
    {
        return primCount;
    }

    // someBool = !someBool;
    // iFun5++;

    // if (alwaysZero != 0)
    // {
    //     UnusedFun();
    // }

    for (int layerID = 0; layerID < 2; layerID++)
    {
        switch (_infos->ModeLayer[layerID])
        {
            case 1: primCount += DrawScrollar(layerID, camPosX, camPosY); break;
            case 2: primCount += DrawCellular(layerID, camPosX, camPosY); break;
            default : break;
        }
    }

    if (_infos->BGColor.Alpha != 0)
    {
        // ADD FUNCTION
    }

    return primCount;
}

int Lining::DrawScrollar(int layerID, int camPosX, int camPosY)
{
    int primCount = 0;

    LayerInfos* layerInfos = (LayerInfos*) (_data + _header->Layers[layerID]);
    Scrollar* scrollar = (Scrollar*) (_data + _header->Layers[layerID] + 4);

    // Update timer
    _animFrameTimer[layerID]++;

    if (_animFrameTimer[layerID] >= scrollar->ScrollXPeriod + 1)
    {
        _animFrameCounter[layerID]++;
        _animFrameTimer[layerID] = 0;
    }

    if (_animFrameCounter[layerID] >= _infos->AnimNum)
    {
        _animFrameCounter[layerID] = 0;
    }

    // Compute parallax
    if (scrollar->FactorXDenom == 0 || scrollar->FactorYDenom == 0)
    {
        printf("Zero Division: Trying to prevent the world from collapsing\n");
        return primCount;
    }

    _parallaxOffsetX[layerID] = (camPosX * scrollar->FactorXNum) / scrollar->FactorXDenom;
    _parallaxOffsetY[layerID] = (camPosY * scrollar->FactorYNum) / scrollar->FactorYDenom;

    // Compute scrolling
    _offsetX[layerID] += scrollar->ScrollXSpeed;
    _offsetY[layerID] += scrollar->ScrollYSpeed;

    // Handle TimerX
    _timerX[layerID]++;
    int periodX = abs(scrollar->ScrollXPeriod);

    if (periodX > 0 && _timerX[layerID] >= periodX)
    {
        _offsetX[layerID] += _scrollDirX[layerID];
        _timerX[layerID] = 0;
    }

    // Handle TimerY
    _timerY[layerID]++;
    int periodY = abs(scrollar->ScrollYPeriod);

    if (periodY > 0 && _timerY[layerID] >= periodY)
    {
        _offsetY[layerID] += _scrollDirY[layerID];
        _timerY[layerID] = 0;
    }

    // Warp-around
    int screenPosX = _parallaxOffsetX[layerID] + _offsetX[layerID];
    int screenPosY = _parallaxOffsetY[layerID] + _offsetY[layerID];

         if (screenPosX < 0)    _offsetX[layerID] += 640;
    else if (screenPosX >= 640) _offsetX[layerID] -= 640;
         if (screenPosY < 0)    _offsetY[layerID] += 480;
    else if (screenPosY >= 480) _offsetY[layerID] -= 480;

    if (_infos->AnimNum == 0)
    {
        printf("Zero Division: Trying to prevent the world from collapsing\n");
        return primCount;
    }

    // 16x16 pixels tiles
    int tileX = (screenPosX < 0) ? (screenPosX + 15) : screenPosX;
    int tileY = (screenPosY < 0) ? (screenPosY + 15) : screenPosY;
    tileX >>= 4;
    tileY >>= 4;

    // Mapping
    bool sndScroll = (_infos->ModeLayer[0] == 1 && _infos->ModeLayer[1] == 1) * layerID;
    char* baseMap = (char*) (_data + _header->Graphics + 0x8100 + sndScroll * 0x960);

    if (tileY > 0x1D) tileY = 0;
    if (tileX > 0x27) tileX = 0;

    //tileX = 0;
    //tileY = 0;

    Rect srcRect;
    Quad dstQuad;
    char* mapRow = (char*) (baseMap + tileY * 0x50);

    for (int y = 0; y < 256; y += 16)
    {
        if (tileY > 0x1D)
        {
            tileY = 0;
            mapRow = baseMap;
        }

        int curTileX = tileX;

        for (int x = 0; x < 336; x += 16)
        {
            if (curTileX > 0x27) curTileX = 0;

            if (baseMap + tileY * 0x50 + (curTileX << 1) >= (char*) (_data + _dataSize))
            {
                printf("Read beyond data range!\n");
                continue;
            }

            char* tileData = (char*) (mapRow + (curTileX << 1));
            char tileVal = tileData[0];

            if (tileVal)
            {
                primCount++;

                int abe = (layerInfos->BlendMode != 0) + layerInfos->BlendMode;
                int palDex = tileData[1];
                int u0 = (tileVal & 0x0F) << 4;
                int v0 = (tileVal & 0xF0) + _animFrameCounter[layerID] * (0x100 / _infos->AnimNum);
                int x0 = x - screenPosX;// + (curTileX << 0);
                int y0 = y - screenPosY;// + (tileY << 0);

                srcRect =
                {
                    u0, v0,
                    16, 16
                };

                dstQuad =
                {
                    { x0, y0 },
                    { x0 + 16, y0 },
                    { x0 + 16, y0 + 16 },
                    { x0, y0 + 16 }
                };

                _drawer->AddBatch(dstQuad, srcRect, 3, palDex, 0);
            }

            curTileX++;
        }

        tileY++;
        mapRow += 0x50;
    }

    return primCount;
}

int Lining::DrawCellular(int layerID, int camPosX, int camPosY)
{
    // HIC SUNT DRACONES

    return 0;
}

Lining::~Lining()
{
    delete _tileSet;
}

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
    _lining = nullptr;
}

int Level::Init(Drawer* drawer)
{
    // Init tileset
    _tileSet = new Texture();
    _tileSet->TexFromData(GetTSetPtr(), GetProperties()->Palettes, 1024);
    drawer->LoadTexture(_tileSet->GetDataBits(), _tileSet->GetHeight(), 2);
    drawer->LoadPalette(_tileSet->GetPalette()->GetData(), _tileSet->GetPalette()->GetCount(), 2);

    // Init local entities
    _entities = new Entities(GetEntPtr(), GetSSheetPtr());
    drawer->LoadTexture(_entities->GetSpriteSheet()->GetDataBits(), _entities->GetSpriteSheet()->GetHeight(), 1);
    drawer->LoadPalette(_entities->GetSpriteSheet()->GetPalette()->GetData(), _entities->GetSpriteSheet()->GetPalette()->GetCount(), 1);

    // Init lining
    _lining = new Lining(GetLiningPtr(), GetLiningSize());
    _lining->Init(drawer);

    return 0;
}

bool Level::Visible(const Rect& view, const Rect& rect)
{
    return (view.X < rect.X + rect.Width &&
            view.X + view.Width > rect.X &&
            view.Y < rect.Y + rect.Height &&
            view.Y + view.Height > rect.Y);
}

void Level::DrawTile(Drawer* drawer, uint16_t texTile, Point pos)
{
    if (texTile == 0xFFFF) return;

    int pal = texTile >> 12;
    uint16_t tile = texTile & 0x0FFF;
    int xTex = (tile - 10 * floor(0.1f * tile)) * 24;
    int yTex = floor(0.1f * tile) * 16;

    int anim = (8 + (yTex >> 8)) << 1;
    yTex += _mapAnim[anim] * (256 >> GetProperties()->MapAnim[anim]);

    Rect rect =
    {
        xTex, yTex,
        24, 16
    };

    Quad quad =
    {
        { pos.X, pos.Y },
        { pos.X + 24, pos.Y },
        { pos.X + 24, pos.Y + 16 },
        { pos.X, pos.Y + 16 },
    };

    drawer->AddBatch(quad, rect, 2, pal, 0);
}

void Level::UpdateTimers()
{
    // Tile timers
    for (int i = 0; i < 6; i++)
    {
        int dex = (i + 8) << 1;

        if (GetProperties()->MapAnim[dex] != 0)
        {
            if (_mapAnim[dex + 1] <= 0)
            {
                _mapAnim[dex + 1] = GetProperties()->MapAnim[dex + 1];

                if (_mapAnim[dex] + 1 == 1 << GetProperties()->MapAnim[dex])
                _mapAnim[dex] = 0;

                else _mapAnim[dex]++;
            }

            else _mapAnim[dex + 1] -= 4;    // Tile animation rate
        }
    }
}

void Level::DrawScene(Drawer* drawer, const Rect& cam)
{
    UpdateTimers();

    // Set the background color
    drawer->SetBGColor((Color) GetProperties()->BGColor);

    int startX = std::max(0, cam.X / 24);
    int endX = std::min(startX + 15, 52);

    // Set the map
    for (int y = 0; y < 60; y++)
    {
        // Set the tiles
        for (int x = startX; x < endX; x++)
        {
            Rect dstRect = { x * 24, (y - GetTile(x, y)->ZPos) * 16, 24, 16 };

            // Checks for OOB
            if ((y - GetTile(x, y)->ZPos >= 0) && Visible(cam, dstRect))
            {
                DrawTile(drawer, GetTile(x, y)->TexTile, Point { dstRect.X - cam.X, dstRect.Y - cam.Y });
            }
        }

        /* TODO (Like, later)

        // Set the instances
        if (_inst->_instCount > 0)
        {
            while (((_inst->_living[_instSort[instDex]]->_YPos + _inst->_living[_instSort[instDex]]->_depth) >> 4) - 1 < y && (instDex < _inst->_instCount - 1))
            {
                _inst->_living[_instSort[instDex]]->Draw(_bmpData);
                instDex++;
                //printf("%d\n", _instSort[instDex]);
            }
        }*/

        for (int x = startX; x < endX; x++)
        {
            uint16_t wallPtr = GetTile(x, y)->WallPtr << 1;

            if (wallPtr < GetWallsSize())
            {
                int8_t wallZPos = GetTile(x, y)->ZPos + *((int8_t*) (GetWallsPtr() + wallPtr)) - 1;
                uint8_t wallCount = *((int8_t*) (GetWallsPtr() + wallPtr + 1));

                for (int i = 0; i < wallCount; i++)
                {
                    Rect dstRect = { x * 24, (y - wallZPos + i) * 16, 24, 16 };

                    // Checks for OOB
                    if ((y - wallZPos + i >= 0) && Visible(cam, dstRect))
                    {
                        uint16_t texTile = *((int16_t*) (GetWallsPtr() + wallPtr + ((i + 1) << 1)));
                        DrawTile(drawer, texTile, Point { dstRect.X - cam.X, dstRect.Y - cam.Y });
                    }
                }
            }
        }
    }

    _lining->DrawLining(cam.X, cam.Y);

    drawer->DrawFrame();
}

Level::~Level()
{
    free(_data);
    delete _tileSet;
    delete _entities;
    delete _lining;
}
