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
    _drawer = drawer;
    _drawer->EnableOvr(false);
    _infos = (LiningInfos*) (_data + 28);

    // Return if disabled
    if (!_infos->Enabled) return 0;

    _hasGraphx = 256 < _dataSize - _header->Graphics;

    if (_hasGraphx)
    {
        // Load graphics
        _tileSet = new Texture();
        _tileSet->TexFromData(_data + _header->Graphics + 256, (uint16_t*) (_data + _header->Graphics), 256);
        _drawer->LoadTexture(_tileSet->GetDataBits(), _tileSet->GetHeight(), 3);
        _drawer->LoadPalette(_tileSet->GetPalette()->GetData(), _tileSet->GetPalette()->GetCount(), 3);

        for (int layerID = 0; layerID < 2; layerID++)
        {
            _animFrameTimer[layerID]   = 0;
            _animFrameCounter[layerID] = 0;
            _parallaxOffsetX[layerID]  = 0;
            _parallaxOffsetY[layerID]  = 0;
            _offsetX[layerID]          = 0;
            _offsetY[layerID]          = 0;
            _timerX[layerID]           = 0;
            _timerY[layerID]           = 0;

            switch (_infos->LayerMode[layerID])
            {
                case 1 : InitScrollar(layerID); break;
                case 2 : InitCellular(layerID); break;
                default : break;
            }
        }
    }

    if (_infos->BGColor.Alpha != 0) InitOverlay();

    return 0;
}

void Lining::InitScrollar(int layerID)
{
    Scrollar* scrollar = (Scrollar*) (_data + _header->Layers[layerID] + 4);

    _scrollDirX[layerID] = 0;
    _scrollDirY[layerID] = 0;

    if (scrollar->ScrollXPeriod != 0)
    _scrollDirX[layerID] = ((scrollar->ScrollXSpeed >= 0) ^ (scrollar->ScrollXPeriod < 0)) ? +1 : -1;

    if (scrollar->ScrollYPeriod != 0)
    _scrollDirY[layerID] = ((scrollar->ScrollYSpeed >= 0) ^ (scrollar->ScrollYPeriod < 0)) ? +1 : -1;
}

void Lining::InitCellular(int layerID)
{
    Cellular* cellular = (Cellular*) (_data + _header->Layers[layerID] + 4);
    Cell* cells = (Cell*) (cellular + sizeof(Cellular));

    std::fill_n(_cellPosX[layerID], CELL_MAX, 0);
    std::fill_n(_cellPosY[layerID], CELL_MAX, 0);
    std::fill_n(_cellTickX[layerID], CELL_MAX, 0);
    std::fill_n(_cellTickY[layerID], CELL_MAX, 0);

    const int cellNum = cellular->Divisions;

    for (int i = 0; i < cellNum; ++i)
    {
        _cellPosX[layerID][i] = cells[i].X0;
        _cellPosY[layerID][i] = cells[i].Y0;
    }
}

void Lining::InitOverlay()
{
    _drawer->EnableOvr(true);
    _ovrOff = 0;
    _ovrTick = 0;
    _ovrHold = 0;
}

void Lining::DrawGround(int ground)
{
    auto& _queue = (ground == 0 ? _backQueue : _frontQueue);

    if (!_queue.empty())
    {
        for (const auto& b : _queue) _drawer->AddBatch(b);
        _queue.clear();
    }
}

int Lining::SetLining(int camPosX, int camPosY)
{
    int primCount = 0;

    if (!_infos->Enabled) return primCount;

    if (_hasGraphx)
    {
        for (int layerID = 1; layerID >= 0; layerID--)
        {
            switch (_infos->LayerMode[layerID])
            {
                case 1: primCount += SetScrollar(layerID, camPosX, camPosY); break;
                case 2: primCount += SetCellular(layerID, camPosX, camPosY); break;
                default : break;
            }
        }
    }

    if (_infos->BGColor.Alpha != 0) SetOverlay(_infos->BGColor.Alpha);

    return primCount;
}

int Lining::SetScrollar(int layerID, int camPosX, int camPosY)
{
    int primCount = 0;

    const LayerInfos* layerInfos = GetLayerInfos(layerID);
    const Scrollar*   scrollar   = GetScrollar(layerID);

    // Safeguard
    if (scrollar->FactorXDenom == 0 || scrollar->FactorYDenom == 0)
    { printf("Zero Division: Trying to prevent the world from collapsing\n"); return 0; }

    // Parallax
    _parallaxOffsetX[layerID] = (camPosX * scrollar->FactorXNum) / scrollar->FactorXDenom;
    _parallaxOffsetY[layerID] = (camPosY * scrollar->FactorYNum) / scrollar->FactorYDenom;

    // Animation
    int animNum = _infos->AnimNum <= 0 ? 1 : _infos->AnimNum;
    if (++_animFrameTimer[layerID] > layerInfos->AnimTimer)
    {
        if (++_animFrameCounter[layerID] >= animNum) _animFrameCounter[layerID] = 0;
        _animFrameTimer[layerID] = 0;
    }

    // X Scrolling
    _timerX[layerID]++;
    _offsetX[layerID] += scrollar->ScrollXSpeed;
    int periodX = abs(scrollar->ScrollXPeriod);

    if (periodX > 0 && _timerX[layerID] >= periodX)
    {
        _offsetX[layerID] += (_scrollDirX[layerID]);
        _timerX[layerID] = 0;
    }

    // Y Scrolling
    _timerY[layerID]++;
    _offsetY[layerID] += scrollar->ScrollYSpeed;
    int periodY = abs(scrollar->ScrollYPeriod);

    if (periodY > 0 && _timerY[layerID] >= periodY)
    {
        _offsetY[layerID] += (_scrollDirY[layerID]);
        _timerY[layerID] = 0;
    }

    // Returns if TickOnly
    if (!_drawer->IsReady()) return 0;

    // Warp-around
    int screenPosX = _offsetX[layerID] + _parallaxOffsetX[layerID];
    int screenPosY = _offsetY[layerID] + _parallaxOffsetY[layerID];

    while (screenPosX < 0)    { _offsetX[layerID] += 640; screenPosX += 640; }
    while (screenPosX >= 640) { _offsetX[layerID] -= 640; screenPosX -= 640; }
    while (screenPosY < 0)    { _offsetY[layerID] += 480; screenPosY += 480; }
    while (screenPosY >= 480) { _offsetY[layerID] -= 480; screenPosY -= 480; }

    // Tile indices & Remainders
    const int tileX = (screenPosX >> 4);
    const int tileY = (screenPosY >> 4);
    const int subX  = (screenPosX & 15);
    const int subY  = (screenPosY & 15);

    // Base map (second scroll only if both layers are Scrollars)
    const bool sndScroll = (_infos->LayerMode[0] == 1 && _infos->LayerMode[1] == 1) * layerID;
    char* baseMap = (char*) (_data + _header->Graphics + 0x8100 + (sndScroll ? 0x960 : 0));

    // Visible Grid
    const int xStart = -subX;
    const int yStart = -subY;
    const int cols   = (320 - xStart + 15) >> 4;
    const int rows   = (240 - yStart + 15) >> 4;

    // Common vars
    const int fAnim = _animFrameCounter[layerID];
    const int vAnim = (fAnim << 8) / animNum;
    const char abe = layerInfos->BlendMode;
    const char* dataEnd = (const char*) (_data + _dataSize);

    Rect srcRect;
    Quad dstQuad;

    // Mapping
    for (int y = 0; y < rows; ++y)
    {
        int ty = tileY + y;
        if (ty >= 0x1E) ty -= 0x1E;
        char* row = baseMap + ty * 0x50;

        for (int x = 0; x < cols; ++x)
        {
            int tx = tileX + x;
            if (tx >= 0x28) tx -= 0x28;

            char* entry = row + (tx << 1);
            if (entry + 1 >= dataEnd) continue;

            char tileVal = entry[0];
            char palDex = entry[1];
            if (tileVal == 0) continue;

            const int u0 = (tileVal & 0x0F) << 4;
            const int v0 = ((tileVal & 0xF0) + vAnim) & 0xFF;
            const int x0 = xStart + (x << 4);
            const int y0 = yStart + (y << 4);

            srcRect = { u0, v0, 16, 16 };
            dstQuad =
            {
                { x0,      y0      },
                { x0 + 16, y0      },
                { x0 + 16, y0 + 16 },
                { x0,      y0 + 16 }
            };

            Batch b = { dstQuad, srcRect, 3, palDex, abe };

            if (layerInfos->Ground) { _frontQueue.push_back(b); }
            else                    { _backQueue.push_back(b); }

            primCount++;
        }
    }

    return primCount;
}

int Lining::CellNormal(int layerID, int i, int phase, int camPosX, int camPosY)
{
    const Cellular* cellular = GetCellular(layerID);
    const Cell* cells = GetCells(layerID);
    const Cell& curCell = cells[i];

    // Persistent state for this cell
    int& posX  = _cellPosX[layerID][i];
    int& posY  = _cellPosY[layerID][i];
    int& tickX = _cellTickX[layerID][i];
    int& tickY = _cellTickY[layerID][i];

    // Continuous drift
    const int dx = curCell.DX;
    const int dy = curCell.DY;
    posX += dx;
    posY += dy;

    // Discrete steps when timers reach Period
    const int8_t pX = curCell.PeriodX;
    const int8_t pY = curCell.PeriodY;

    if (pX != 0)
    {
        const int stepX = (dx < 0 || pX < 0) ? -1 : +1;
        const int absPX = std::abs(pX);
        if (++tickX >= absPX) { posX += stepX; tickX = 0; }
    }

    if (pY != 0)
    {
        const int stepY = (dy < 0 || pY < 0) ? -1 : +1;
        const int absPY = std::abs(pY);
        if (++tickY >= absPY) { posY += stepY; tickY = 0; }
    }

    // Camera parallax base
    int baseX = 0, baseY = 0;
    if (curCell.CamXDen != 0) baseX = (camPosX * curCell.CamXNum) / curCell.CamXDen;
    if (curCell.CamYDen != 0) baseY = (camPosY * curCell.CamYNum) / curCell.CamYDen;

    // Screen-space coords before wrapping
    int sx = posX - baseX;
    int sy = posY - baseY;

    // Width/height-aware wrapping
    const int minX = curCell.U0 - curCell.U1;
    if (sx < minX)     { posX += 320 - minX; sx = posX - baseX; }
    else if (sx > 319) { posX += -320 + minX;  sx = posX - baseX; }

    const int minY = curCell.V0 - curCell.V1;
    if (sy < minY)     { posY += 240 - minY; sy = posY - baseY; }
    else if (sy > 239) { posY += -240 + minY;  sy = posY - baseY; }

    // Returns if TickOnly
    if (!_drawer->IsReady()) return 0;

    const int u0 = curCell.U0;
    const int v0 = (curCell.V0 + phase) & 0xFF;
    const int w  = curCell.U1 - curCell.U0 + 1;
    const int h  = curCell.V1 - curCell.V0 + 1;

    Rect src = { u0, v0, w, h };
    Quad dst = {
        { sx,      sy      },
        { sx + w,  sy      },
        { sx + w,  sy + h  },
        { sx,      sy + h  }
    };

    Batch b = { dst, src, 3, curCell.PalDex, GetLayerInfos(layerID)->BlendMode };

    if (GetLayerInfos(layerID)->Ground) _frontQueue.push_back(b);
    else                                _backQueue.push_back(b);

    return 1;
}

int Lining::CellScriptTrack(int layerID, int i, int phase)
{
    // Never used in Alundra
    return 0;
}


int Lining::CellFallRespawn(int layerID, int i, int phase, int camPosX, int camPosY)
{
    const Cellular* cellular = GetCellular(layerID);
    const Cell* cells = GetCells(layerID);
    const Cell& curCell = cells[i];

    // Persistent state for this cell
    int& posX  = _cellPosX[layerID][i];
    int& posY  = _cellPosY[layerID][i];
    int& tickX = _cellTickX[layerID][i];
    int& tickY = _cellTickY[layerID][i];

    // Continuous drift
    posX += curCell.DX;
    posY += curCell.DY;

    // Derive discrete step sign on the fly
    const int8_t pX = curCell.PeriodX, pY = curCell.PeriodY;
    const int8_t dx = curCell.DX,      dy = curCell.DY;

    int stepX = 0, stepY = 0;
    if (pX != 0) stepX = (dx < 0 || pX < 0) ? -1 : +1;
    if (pY != 0) stepY = (dy < 0 || pY < 0) ? -1 : +1;

    // Discrete step when timers reach Period
    const int absPX = std::abs(curCell.PeriodX);
    const int absPY = std::abs(curCell.PeriodY);

    if (absPX > 0 && ++tickX >= absPX) { posX += stepX; tickX = 0; }
    if (absPY > 0 && ++tickY >= absPY) { posY += stepY; tickY = 0; }

    // Camera parallax base
    int baseX = 0, baseY = 0;
    if (curCell.CamXDen != 0) baseX = (camPosX * curCell.CamXNum) / curCell.CamXDen;
    if (curCell.CamYDen != 0) baseY = (camPosY * curCell.CamYNum) / curCell.CamYDen;

    // Screen-space coords before wrapping
    int sx = posX - baseX;
    int sy = posY - baseY;

    // Horizontal wrap with sprite width-aware bounds
    const int minX = curCell.U0 - curCell.U1;
    if (sx < minX)       { posX += 320 - minX; sx = posX - baseX; }
    else if (sx > 319)   { posX += -320 + minX; sx = posX - baseX; }

    // Vertical "fall & respawn" at bottom
    if (sy > 239)
    {
        posX = std::rand() / 0x66;
        posY += -240 + (curCell.V0 - curCell.V1);
        sx = posX - baseX;
        sy = posY - baseY;
    }

    // Returns if TickOnly
    if (!_drawer->IsReady()) return 0;

    const int u0 = curCell.U0;
    const int v0 = (curCell.V0 + phase) & 0xFF;
    const int w  = curCell.U1 - curCell.U0 + 1;
    const int h  = curCell.V1 - curCell.V0 + 1;

    Rect src = { u0, v0, w, h };
    Quad dst = {
                   { sx    ,  sy      },
                   { sx + w,  sy      },
                   { sx + w,  sy + h  },
                   { sx    ,  sy + h  }
               };

    Batch b = { dst, src, 3, curCell.PalDex, GetLayerInfos(layerID)->BlendMode };

    if (GetLayerInfos(layerID)->Ground) { _frontQueue.push_back(b); }
    else                                { _backQueue.push_back(b); }

    return 1;
}

int Lining::CellWaveX(int layerID, int i, int phase)
{
    const Cellular* cellular = GetCellular(layerID);
    const Cell* cells = GetCells(layerID);
    const Cell& curCell = cells[i];

    // Source Rect
    const int w  = curCell.U1 - curCell.U0 + 1;
    const int h  = curCell.V1 - curCell.V0 + 1;
    const int u0 = curCell.U0;
    const int v0 = (curCell.V0 + phase) & 0xFF;

    // WaveLUT
    const int32_t* LUT = (const int32_t*) (_data + _header->WaveLUT);
    const int  y0  = curCell.Y0;
    const int  x0  = curCell.X0;

    // A-wave
    int idxA1 = (y0 * (int)(uint8_t)cellular->AWaveY) & 0xFF;
    int idxA2 = (_waveTick[layerID] * cellular->AWavePhase) & 0xFF;
    int aW = LUT[idxA1] * LUT[idxA2] * cellular->AWaveAmp;
    if (aW < 0) aW += 0x7F;

    // B-wave
    int idxB  = (y0 * cellular->BWaveY + _waveTick[layerID] * cellular->BWavePhase) & 0xFF;
    int bW = LUT[idxB] * cellular->BWaveWeight;

    // Combine, rescale and final X
    int tSum  = (aW >> 7) + bW;
    if (tSum < 0) tSum += 0x7F;
    int x = x0 + (tSum >> 7) - 8;
    int y = y0;

    // Returns if TickOnly
    if (!_drawer->IsReady()) return 0;

    Rect src = { u0, v0, w, h };
    Quad dst = {
                   { x    , y     },
                   { x + w, y     },
                   { x + w, y + h },
                   { x    , y + h }
               };

    Batch b = { dst, src, 3, curCell.PalDex, GetLayerInfos(layerID)->BlendMode };

    if (GetLayerInfos(layerID)->Ground) { _frontQueue.push_back(b); }
    else                                { _backQueue.push_back(b); }

    return 1;
}

int Lining::SetCellular(int layerID, int camPosX, int camPosY)
{
    int primCount = 0;

    const LayerInfos* layerInfos = GetLayerInfos(layerID);
    const Cellular* cellular = GetCellular(layerID);
    const Cell* cells = GetCells(layerID);

    // Animation
    int animNum = _infos->AnimNum <= 0 ? 1 : _infos->AnimNum;
    if (++_animFrameTimer[layerID] > layerInfos->AnimTimer)
    {
        if (++_animFrameCounter[layerID] >= animNum) _animFrameCounter[layerID] = 0;
        _animFrameTimer[layerID] = 0;
    }

    const int phase = (_animFrameCounter[layerID] << 8) / animNum;

    // Cell Loop
    int cellNum = cellular->CountBase + cellular->Divisions;
    _waveTick[layerID]++;

    for (int i = 0; i < cellNum; ++i)
    {
        switch ((CellType) cells[i].Type)
        {
            case CellType::Normal:       primCount += CellNormal(layerID, i, phase, camPosX, camPosY);          break;
            case CellType::ScriptTrack:  primCount += CellScriptTrack(layerID, i, phase);                       break;
            case CellType::FallRespawn:  primCount += CellFallRespawn(layerID, i, phase, camPosX, camPosY);     break;
            case CellType::WaveX:        primCount += CellWaveX      (layerID, i, phase);                       break;
            default:                                                                                            break;
        }
    }

    return primCount;
}

int Lining::SetOverlay(uint8_t flag)
{
    const bool extended = !(flag < 0x65);

    _ovrTick++;

    uint32_t ovrPtr = extended ? _header->OverlayExt : _header->Overlay;
    uint32_t ovrSize = extended ? sizeof(OverlayExt) : sizeof(Overlay);

    if(!ovrPtr || ovrPtr >= _dataSize) return 0;

    if (_ovrTick >= _ovrHold)
    {
        const uint8_t* base = (const uint8_t*) (_data + ovrPtr + _ovrOff);

        if ((base + ovrSize) > (uint8_t*) (_data + _dataSize))
        {
            _ovrOff = 0;
            base = (const uint8_t*) (_data + ovrPtr);
        }

        _ovrHold = extended ? ((const OverlayExt*) base)->hold : ((const Overlay*) base)->hold;
        _ovrTick = 0;
        _ovrOff += ovrSize;

        if(_ovrHold == 0) _ovrOff = 0; // loop
    }

    // Returns if TickOnly
    if (!_drawer->IsReady()) return 0;

    Rect src = { 0, 0, 0, 0 };
    Quad dst = {
                   { 0  , 0   },
                   { 320, 0   },
                   { 320, 240 },
                   { 0  , 240 }
               };

    // Emit lastest color
    const uint32_t prev = (_ovrOff >= ovrSize) ? (_ovrOff - ovrSize) : 0;
    const uint8_t* base = (const uint8_t*) (_data + ovrPtr + prev);

    if (!extended)
    {
        float rec[16];
        const auto* ovr = (Overlay*) base;
        float color[4] = { ovr->r * INV255, ovr->g * INV255, ovr->b * INV255, 0 };
        for (int i = 0; i < 16; ++i) rec[i] = color[i & 3];
        _drawer->UpdateOverlay(rec);
    }
    else
    {
        const auto* ovr = (OverlayExt*) base;
        float rec[16] = {
                            ovr->r00 * INV255, ovr->g00 * INV255, ovr->b00 * INV255, 0,
                            ovr->r10 * INV255, ovr->g10 * INV255, ovr->b10 * INV255, 0,
                            ovr->r01 * INV255, ovr->g01 * INV255, ovr->b01 * INV255, 0,
                            ovr->r11 * INV255, ovr->g11 * INV255, ovr->b11 * INV255, 0
                        };
        _drawer->UpdateOverlay(rec);
    }

    _overlay = { dst, src, -1, 0, flag };

    return 1;
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

    int startX = std::max(0, cam.X / 24);
    int endX = std::min(startX + 15, 52);

    _lining->SetLining(cam.X, cam.Y);

    // Returns if TickOnly
    if (!drawer->IsReady()) return;

    // Draws the Background
    _lining->DrawGround(0);

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

    // Draws the Overlay and the Foreground
    _lining->DrawOverlay();
    _lining->DrawGround(1);
}

Level::~Level()
{
    free(_data);
    delete _tileSet;
    delete _entities;
    delete _lining;
}

