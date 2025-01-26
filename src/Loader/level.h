#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <cstdint>
#include "imaging.h"

struct Position
{
    uint8_t     X;
    uint8_t     Y;
    uint8_t     Z;
    uint8_t     Unused;
};

struct Rect
{
    uint8_t     Left;
    uint8_t     Top;
    uint8_t     Right;
    uint8_t     Bottom;
};

struct Portal
{
    Rect        TriggerZone;
    uint16_t    DestLevel;
    Position    DestXYZ;        // Calibrated on TriggerZone
    uint8_t     Transition;     // Strong 4bits Video | Weak 4bits Audio
    uint8_t     Direction;      // Strong 4bits Source | Weak 4bits Destination
};

struct MapCopy
{
    uint8_t     FromX;
    uint8_t     FromY;
    uint8_t     Width;
    uint8_t     Height;
    uint8_t     ToX;
    uint8_t     ToY;
}   __attribute__((packed, aligned(2)));

struct Properties
{
    uint32_t    LevelID;
    uint16_t    Gravity;          // Normal = 128 | Underwater = 3
    uint16_t    ZViscosity;       // Max value is 4096 | The lower, the more viscous
    uint8_t     XYResistance;     // Objects move slower as it increases | Modulo 32
    uint8_t     AnimDeleteWall;   // Plays as you destruct a wall | Always 2
    uint8_t     AnimLandFloor;    // Plays as you hit the floor | Always 7
    uint8_t     Unknown;          // Who TF are you?!
    uint32_t    BGColor;          // RGB
    uint16_t    Palettes[512];    // 32 palettes each containing 16 colors (15 bits depth)
    uint16_t    MapAnim[14];      // One for each 256x256 subset | ImgNum = 1 << Byte1 | Timer = Byte2
    Portal      Portals[64];      // Inactive is 00 00 FF FF 00 00 00 00 00 00 00 00
    uint32_t    MapSize;          // Written twice | Always 0x343C343C
    MapCopy     MapCopies[100];   // Copy map parts to be drawn elsewhere | Barely used
};

struct Tile
{
    uint8_t     Behavior;
    // Flags : Damage1 | Blocked | Ice | ???? | Slow | ???? | Destructibe | Collision
    uint8_t     Type;
    // Flags : WarpOOB | NoJump | NoSword | ???? | DeepWater <WallClimb> LowWater <Sand> WetFloor | Damage2
    uint8_t     Control;
    // Flags : Locked | Push | Left <Right> Down | AirCtrl | FloorCtrl | LeftStairs <RightStairs> UpStairs
    uint8_t     ZPos;
    uint16_t    TexTile;
    uint16_t    WallPtr;
};  // Default value for Push is Up | <MeansBothBits>

struct LevelHeader
{
    uint32_t    Properties;
    uint32_t    Map;
    uint32_t    Tileset;
    uint32_t    ObjGroup;
    uint32_t    Spritesheet;
    uint32_t    Layout;         // Background parallax along with other screen effects
    uint32_t    Strings;
};

class Level
{
    private:

    char*           _data;
    LevelHeader*    _header;
    uint32_t        _dataSize;

    Texture*        _tileSet;

    public:

    /// Constructor
    Level(char* levelData, uint32_t levelSize);

    // Getters

    char*        GetDataPtr()     { return _data; }
    Properties*  GetProperties()  { return (Properties*) (_data + _header->Properties); }
    char*        GetTileset()     { return (char*) (_data + _header->Tileset); }

    /// Destructor
    ~Level();
};

#endif // LEVEL_H_INCLUDED
