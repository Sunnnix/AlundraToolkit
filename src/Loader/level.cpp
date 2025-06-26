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

int _parallaxX_skipCounter[2];
int _parallaxY_skipCounter[2];

void Lining::updateParallaxLayer(int layerID, int camPosX, int camPosY, Scrollar* scrollar)
{
    _animFrameCounter[layerID]++;
    if (_animFrameTimer[layerID] >= scrollar->ScrollXPeriod + 1) {
        _animFrameCounter[layerID]++;
        _animFrameTimer[layerID] = 0;
    }

    if (_animFrameCounter[layerID] >= _infos->AnimNum)
        _animFrameCounter[layerID] = 0;

    camPosX = camPosX * scrollar->FactorXNum;
    if ((int)scrollar->FactorXDenom == 0)
        throw std::exception("0x1c00");
    if (((int)scrollar->FactorXDenom == -1) && (camPosX == -0x80000000))
        throw std::exception("0x1800");
    _offsetX[layerID] = camPosX / (int)scrollar->FactorXDenom;
    camPosY = camPosY * scrollar->FactorYNum;

    if ((int)scrollar->FactorYDenom == 0) {
        throw std::exception("0x1c00");
    }
    if (((int)scrollar->FactorYDenom == -1) && (camPosY == -0x80000000)) {
        throw std::exception("0x1800");
    }
    _offsetY[layerID] = camPosY / (int)scrollar->FactorYDenom;
    _parallaxOffsetX[layerID] += (int)scrollar->ScrollXSpeed;
    _parallaxOffsetY[layerID] += (int)scrollar->ScrollYSpeed;
    _parallaxX_skipCounter[layerID]++;

    if ((int)scrollar->ScrollXPeriod < 0) {
        *(int*)&scrollar->ScrollXPeriod = -(int)scrollar->ScrollXPeriod;
    }
    if ((int)scrollar->ScrollXPeriod + -1 <= (int)_parallaxX_skipCounter[layerID]) {
        _parallaxOffsetX[layerID] += _scrollDirX[layerID];
        _parallaxX_skipCounter[layerID] = 0;
    }
    _parallaxY_skipCounter[layerID]++;

    if ((int)scrollar->ScrollYPeriod < 0) {
        *(int*)&scrollar->ScrollYPeriod = -(int)scrollar->ScrollYPeriod;
    }
    if ((int)scrollar->ScrollYPeriod + -1 <= (int)_parallaxY_skipCounter[layerID]) {
        _parallaxOffsetY[layerID] += _scrollDirY[layerID];
        _parallaxY_skipCounter[layerID] = 0;
    }
    //if (BufferIndex == 0) {
    //    CurrentParallaxGPUData_Layer_Ptr = &ParallaxGPUData_L1;
    //}
    //else {
    //    CurrentParallaxGPUData_Layer_Ptr = &ParallaxGPUData_L2;
    //}
}

int Lining::DrawScrollar(int layerID, int camPosX, int camPosY)
{
    LayerInfos* layerInfos;
    Scrollar* scrollar;
    int tileByteOffset;
    int tileScrollXAdjusted;
    int tileScrollY;
    int* finalOffsetXPtr;
    int tileY;
    uint32_t* scrollOffsetX;
    int tileScrollX;
    uint32_t* scrollOffsetY;
    int tileX;
    int x;
    int y;
    int prims;
    int layerOffsets[4];
    short tileScrollYAdjusted;
    int tmp;
    Rect srcRect;
    Quad dstQuad;

    layerInfos = (LayerInfos*)(_data + _header->Layers[layerID]);
    scrollar = (Scrollar*)(_data + _header->Layers[layerID] + 4);

    prims = 0;
    layerOffsets[0] = 0;
    layerOffsets[1] = 0;
    layerOffsets[2] = 0;
    layerOffsets[3] = 0;
    updateParallaxLayer(layerID, camPosX, camPosY, scrollar);
    scrollOffsetX = (uint32_t*) &_parallaxOffsetX[layerID];
    finalOffsetXPtr = layerOffsets;
    *finalOffsetXPtr = _offsetX[layerID] + *scrollOffsetX;
    scrollOffsetY = (uint32_t*) &_parallaxOffsetY[layerID];
    layerOffsets[2] = _offsetY[layerID] + *scrollOffsetY;
    if (*finalOffsetXPtr < 0) {
        *scrollOffsetX = *scrollOffsetX + 0x280;
    }
    if (0x27f < *finalOffsetXPtr) {
        *scrollOffsetX = *scrollOffsetX - 0x280;
    }
    tmp = layerOffsets[2];
    if (tmp < 0) {
        *scrollOffsetY = *scrollOffsetY + 0x1e0;
        tmp = layerOffsets[2];
    }
    if (0x1df < tmp) {
        *scrollOffsetY = *scrollOffsetY - 0x1e0;
    }
    //tileDataBase = DAT_801dda68;
    //animFrameCount = *(uint8_t*)((int)ParallaxAnimFrameCount ? +0x1d);
    //if (animFrameCount == 0) {
    //    throw std::exception("0x1c00");
    //}
    //currentAnimFrame = PLI.animationIndex;
    tileScrollX = *finalOffsetXPtr;
    tileScrollXAdjusted = tileScrollX;
    if (tileScrollX < 0) {
        tileScrollXAdjusted = tileScrollX + 15;
    }
    tileScrollY = layerOffsets[2];
    tileY = tileScrollY;
    if (tileScrollY < 0)
        tileY = tileScrollY + 15;
    tileY /= 16;//tileRowIndex = tileRowIndex >> 4;
    tileScrollYAdjusted = (short)tileY;
    //layerAnimDataStart = *ParallaxAnimFrameCount ? +layerIndex * DAT_801e6368 * 0x960 + 0x8100;
    if (0x1d < tileY)
        tileY = 0;

    bool sndScroll = (_infos->ModeLayer[0] == 1 && _infos->ModeLayer[1] == 1) * layerID;
    char* baseMap = (char*)(_data + _header->Graphics + 0x8100 + sndScroll * 0x960);

    char* mapRow = (char*)(baseMap + tileY * 0x50);
    for(y = 0; y < 256; y += 0x10) {
        /*if (29 < tileRowIndex) {
            tileRowIndex = 0;
            rowStartPtr = layerAnimDataStart;
        }*/
        if (29 < tileY) {
            tileY = 0;
            mapRow = baseMap;
        }
        //tilePrimPtr = (short*)((int)primPtr + 10);
        tileX = tileScrollXAdjusted >> 4;

        for(x = 0; x < 336; x += 16) {
            int curTileX = tileX;

            tileByteOffset = tileX << 1;
            if (39 < tileX) {
                tileX = 0;
                tileByteOffset = 0;
            }


            //tileDataPtr = (uint8_t*)(tileDataBase + rowStartPtr + tileByteOffset);
            //tileID = *tileDataPtr;
            /*if (tileID != 0) {
                prims = prims + 1;
                tileClutIndex = *(short*)(((unsigned int)tileDataPtr[1] + ParallaxClutBaseOffset ? ) * 2 + ParallaxClutTable ? );
                *(uint8_t*)((int)tilePrimPtr + 3) = (tileID & 240) + (char)currentAnimFrame * (char)(256 / animFrameCount);
                *(char*)(tilePrimPtr + 1) = (char)((tileID & 15) << 4);
                tilePrimPtr[-1] = (short)screenTileX - ((short)tileScrollX + (short)(tileScrollXAdjusted >> 4) * -16);
                *tilePrimPtr = (short)screenTileY - ((short)tileScrollY + tileScrollYAdjusted * -16);
                tilePrimPtr[2] = tileClutIndex;
                // Probable PsyQ macro: addPrim().
                *primPtr = *primPtr & 0xff000000 | *orderingTable & 0xffffff;
                *orderingTable = *orderingTable & 0xff000000 | (uint)primPtr & 0xffffff;
            }*/

            char* tileData = (char*)(mapRow + (curTileX << 1));
            char tileVal = *tileData;

            if (tileVal) {
                prims++;
                
                int abe = (layerInfos->BlendMode != 0) + layerInfos->BlendMode;
                int palDex = tileData[1];
                int u0 = (tileVal & 0x0F) << 4;
                int v0 = (tileVal & 0xF0) + _animFrameCounter[layerID] * (0x100 / _infos->AnimNum);
                int x0 = (short)x - ((short)tileScrollX + (short)(tileScrollXAdjusted >> 4) * -16);
                int y0 = (short)y -((short)tileScrollY + tileScrollYAdjusted * -16);

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
            //tilePrimPtr = tilePrimPtr + 8;
            //primPtr = primPtr + 4;
            tileX += 1;
        };
        tileY = tileY + 1;
        mapRow += 80;
    };
    return prims;
}

// ==========================================================================================
// new try to decode Ghidra (don't work properly)        TODO
// ==========================================================================================

/*
void Lining::updateParallaxLayer(int camPosX, int camPosY, int layerID)
{
    int tmp;
    int parallaxSkipCounter;
    int* parallaxSkipXounterPtr;
    int* parallaxOffsetXPtr;
    int* animFrameTimerPtr;
    int* parallaxOffsetYPtr;

    Scrollar* scrollar = (Scrollar*)(_data + _header->Layers[layerID] + 4);

    animFrameTimerPtr = _animFrameTimer + layerID;
    tmp = *animFrameTimerPtr;
    *animFrameTimerPtr = tmp + 1;
    if ((int)(uint32_t)(scrollar->ScrollXPeriod + 1) <= tmp) {
        _animFrameCounter[layerID] = _animFrameCounter[layerID] + 1;
        *animFrameTimerPtr = 0;
    }
    if ((int)(uint32_t)(_infos->AnimNum) <= _animFrameCounter[layerID]) {
        _animFrameCounter[layerID] = 0;
    }

    camPosX = camPosX * scrollar->FactorXNum;
    tmp = (int)scrollar->FactorXDenom;
    if (tmp == 0) {
        throw std::exception("0x1c00");
    }
    if ((tmp == -1) && (camPosX == -0x80000000)) {
        throw std::exception("0x1800");
    }
    _offsetX[layerID] = camPosX / tmp;
    camPosY = camPosY * scrollar->FactorYNum;
    tmp = (int)scrollar->FactorYDenom;
    if (tmp == 0) {
        throw std::exception("0x1c00");
    }
    if ((tmp == -1) && (camPosY == -0x80000000)) {
        throw std::exception("0x1800");
    }
    _offsetY[layerID] = camPosY / tmp;
    parallaxOffsetXPtr = _parallaxOffsetX + layerID;
    *parallaxOffsetXPtr = *parallaxOffsetXPtr + (int)scrollar->ScrollXSpeed;
    parallaxOffsetYPtr = _parallaxOffsetY + layerID;
    *parallaxOffsetYPtr = *parallaxOffsetYPtr + (int)scrollar->ScrollYSpeed;
    parallaxSkipXounterPtr = _parallaxX_skipCounter + layerID;
    parallaxSkipCounter = *parallaxSkipXounterPtr;
    *parallaxSkipXounterPtr = parallaxSkipCounter + 1;
    tmp = (int)scrollar->ScrollXPeriod;
    if (tmp < 0) {
        tmp = -tmp;
    }
    if (tmp + -1 <= parallaxSkipCounter) {
        *parallaxOffsetXPtr = *parallaxOffsetXPtr + _scrollDirX[layerID];
        *parallaxSkipXounterPtr = 0;
    }
    parallaxSkipXounterPtr = _parallaxY_skipCounter + layerID;
    parallaxSkipCounter = *parallaxSkipXounterPtr;
    *parallaxSkipXounterPtr = parallaxSkipCounter + 1;
    tmp = (int)scrollar->ScrollYPeriod;
    if (tmp < 0) {
        tmp = -tmp;
    }
    if (tmp + -1 <= parallaxSkipCounter) {
        *parallaxOffsetYPtr = *parallaxOffsetYPtr + _scrollDirY[layerID];
        *parallaxSkipXounterPtr = 0;
    }
    //if (BufferIndex == 0) {
    //    CurrentParallaxGPUData_Layer_Ptr = &ParallaxGPUData_L1;
    //}
    //else {
    //    CurrentParallaxGPUData_Layer_Ptr = &ParallaxGPUData_L2;
    //}
}

int Lining::DrawScrollar(int layerID, int camPosX, int camPosY)
{
    //int tileDataBase;
    int tileByteOffset;
    //byte* tileDataPtr;
    int animFrameCounter;
    int adjustedOffsetX;
    int offsetY;
    int* offsetXPtr;
    int adjustedOffsetY;
    int* pOffsetX;
    int offsetX;
    //short* tilePrimPtr;
    int* pOffsetY;
    int tileX;
    //uint* primPtr;
    int x;
    //char* mapRow;
    int y;
    int prims;
    //char* baseMap;
    int layerOffsets[4];
    uint8_t animNum;
    short tileClutIndex;
    //uint8_t tileID;
    short tileScrollYAdjusted;
    int tmp;
    Rect srcRect;
    Quad dstQuad;

    prims = 0;
    layerOffsets[0] = 0;
    layerOffsets[1] = 0;
    layerOffsets[2] = 0;
    layerOffsets[3] = 0;
    updateParallaxLayer(camPosX, camPosY, layerID);
    pOffsetX = _parallaxOffsetX + layerID;
    offsetXPtr = layerOffsets + layerID;
    *offsetXPtr = _offsetX[layerID] + *pOffsetX;
    pOffsetY = _parallaxOffsetY + layerID;
    layerOffsets[layerID + 2] = _offsetY[layerID] + *pOffsetY;
    if (*offsetXPtr < 0) {
        *pOffsetX = *pOffsetX + 640;
    }
    if (639 < *offsetXPtr) {
        *pOffsetX = *pOffsetX + -640;
    }
    tmp = layerOffsets[layerID + 2];
    if (tmp < 0) {
        *pOffsetY = *pOffsetY + 480;
        tmp = layerOffsets[layerID + 2];
    }
    if (479 < tmp) {
        *pOffsetY = *pOffsetY + -480;
    }
    //tileDataBase = DAT_801dda68;
    animNum = _infos->AnimNum;
    if (animNum == 0) {
        throw std::exception("0x1c00");
    }
    animFrameCounter = _animFrameCounter[layerID];
    offsetX = *offsetXPtr;
    adjustedOffsetX = offsetX;
    if (offsetX < 0) {
        adjustedOffsetX = offsetX + 15;
    }
    offsetY = layerOffsets[layerID + 2];
    adjustedOffsetY = offsetY;
    if (offsetY < 0) {
        adjustedOffsetY = offsetY + 15;
    }
    adjustedOffsetY = adjustedOffsetY >> 4;
    tileScrollYAdjusted = (short)adjustedOffsetY;
    //baseMap = *_infos->AnimNum + layerID * DAT_801e6368 * 0x960 + 0x8100;
    if (0x1d < adjustedOffsetY) {
        adjustedOffsetY = 0;
    }
    //primPtr = (uint*)(CurrentParallaxGPUData_Layer_Ptr + layerID * 0x1540);
    //mapRow = baseMap + adjustedOffsetY * 80;


    // Custom memory access
    LayerInfos* layerInfos = (LayerInfos*)(_data + _header->Layers[layerID]);
    bool sndScroll = (_infos->ModeLayer[0] == 1 && _infos->ModeLayer[1] == 1) * layerID;
    char* baseMap = (char*)(_data + _header->Graphics + 0x8100 + sndScroll * 0x960);
    char* mapRow = (char*)(baseMap + offsetY * 80);

    y = 0;
    do {
        if (29 < adjustedOffsetY) {
            adjustedOffsetY = 0;
            mapRow = baseMap;
        }
        x = 0;
        //tilePrimPtr = (short*)((int)primPtr + 10);
        tileX = adjustedOffsetX >> 4;
        do {
            tileByteOffset = tileX << 1;
            if (39 < tileX) {
                tileX = 0;
                tileByteOffset = 0;
            }
            //tileDataPtr = (byte*)(tileDataBase + mapRow + tileByteOffset);
            //tileID = *tileDataPtr;
            //if (tileID != 0) {
            //    prims = prims + 1;
            //    tileClutIndex =
            //        *(short*)(((uint)tileDataPtr[1] + ParallaxClutBaseOffset ? ) * 2 + ParallaxClutTable ? );
            //    *(byte*)((int)tilePrimPtr + 3) =
            //        (tileID & 240) + (char)animFrameCounter * (char)(256 / animNum);
            //    *(char*)(tilePrimPtr + 1) = (char)((tileID & 15) << 4);
            //    tilePrimPtr[-1] = (short)x - ((short)offsetX + (short)(adjustedOffsetX >> 4) * -16);
            //    *tilePrimPtr = (short)y - ((short)offsetY + tileScrollYAdjusted * -16);
            //    tilePrimPtr[2] = tileClutIndex;
            //    /* Probable PsyQ macro: addPrim(). */
            //    *primPtr = *primPtr & 0xff000000 | *orderingTable & 0xffffff;
            //    *orderingTable = *orderingTable & 0xff000000 | (uint)primPtr & 0xffffff;
            //}

            /*char* tileData = (char*)(mapRow + tileByteOffset);
            char tileVal = tileData[0];

            if (tileVal) {
                prims++;

                int abe = (layerInfos->BlendMode != 0) + layerInfos->BlendMode;
                int palDex = tileData[1];
                int u0 = (tileVal & 0x0F) << 4;
                int v0 = (tileVal & 0xF0) + _animFrameCounter[layerID] * (0x100 / _infos->AnimNum);
                int x0 = (short)x - ((short)offsetX + (short)(adjustedOffsetX >> 4) * -16);
                int y0 = (short)y - ((short)offsetY + tileScrollYAdjusted * -16);

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


            //tilePrimPtr = tilePrimPtr + 8;
            //primPtr = primPtr + 4;
            x = x + 16;
            tileX = tileX + 1;
        } while (x < 336);
        adjustedOffsetY = adjustedOffsetY + 1;
        y = y + 16;
        mapRow = mapRow + 80;
    } while (y < 256);
    return prims;
}*/

// ==========================================================================================
// End
// ==========================================================================================

int Lining::DrawScrollar_OLD(int layerID, int camPosX, int camPosY)
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
