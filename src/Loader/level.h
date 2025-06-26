#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "entities.h"
#include <cstdint>

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

struct LiningHeader
{
    uint32_t    Graphics;
    uint32_t    Layers[2];
    uint32_t    Unknown1;
    uint32_t    Unknown2;
    uint32_t    Unknown3;
    uint32_t    Unknown4;
};

struct LiningInfos
{
    uint8_t     Enabled;
    uint8_t     AnimNum;
    uint8_t     ModeLayer[2];
    Color       BGColor;
};

struct LayerInfos
{
    uint8_t     Unused;     // Always 0
    uint8_t     AnimTimer;
    uint8_t     BlendMode;
    uint8_t     Ground;
};

struct Scrollar
{
    int8_t     FactorXNum; // How many pixels to scroll horizontal against camera movement (negative = scroll with camera)
    int8_t     FactorXDenom;
    int8_t     FactorYNum; // How many pixels to scroll vertical against camera movement (negative = scroll with camera)
    int8_t     FactorYDenom;
    int8_t     ScrollXSpeed; // How many pixels to scroll horizontal per tick
    int8_t     ScrollXPeriod; // How many ticks to skip for horizontal scrolling
    int8_t     ScrollYSpeed; // How many pixels to scroll vertical per tick
    int8_t     ScrollYPeriod; // How many ticks to skip for vertical scrolling
};

class Lining
{
    private:

    char*           _data;
    LiningHeader*   _header;
    uint32_t        _dataSize;
    LiningInfos*    _infos;

    Drawer*         _drawer;
    Texture*        _tileSet;

    int             _animFrameTimer[2];
    int             _animFrameCounter[2];
    int             _parallaxOffsetX[2];
    int             _parallaxOffsetY[2];
    int             _offsetX[2];
    int             _offsetY[2];
    int             _timerX[2];
    int             _timerY[2];
    int             _scrollDirX[2];
    int             _scrollDirY[2];

    /// Initializes a scrollar
    void InitScrollar(int layerID);

    /// Initializes a cellular
    int InitCellular(int layerID);

    /// Draws a layer in scrollar mode
    void updateParallaxLayer(int layerID, int camPosX, int camPosY, Scrollar* scrollar);
    int DrawScrollar(int layerID, int camPosX, int camPosY);
    int DrawScrollar_OLD(int layerID, int camPosX, int camPosY);

    /// Draws a layer in cellular mode
    int DrawCellular(int layerID, int camPosX, int camPosY);

    public:

    /// Constructor
    Lining(char* liningData, uint32_t liningSize);

    /// Initializes the lining
    int Init(Drawer* drawer);

    /// Draws the lining onto the screen
    int DrawLining(int camPosX, int camPosY);

    /// Destructor
    ~Lining();
};

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

struct Position
{
    uint8_t     X;
    uint8_t     Y;
    uint8_t     Z;
    uint8_t     Unused;
};

struct Zone
{
    uint8_t     Left;
    uint8_t     Top;
    uint8_t     Right;
    uint8_t     Bottom;
};

struct Portal
{
    Zone        TriggerZone;
    uint16_t    DestLevel;
    Position    DestXYZ;        // Calibrated on TriggerZone
    uint8_t     Transition;     // Strong 4bits Video | Weak 4bits Audio
    uint8_t     Direction;      // Strong 4bits Source | Weak 4bits Destination
};

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
    uint8_t     MapAnim[28];      // One for each 256x256 subset | ImgNum = 1 << Byte1 | Timer = Byte2
    Portal      Portals[64];      // Inactive is 00 00 FF FF 00 00 00 00 00 00 00 00
};

enum TILE_BEHAVIOR : uint8_t
{
    B_NONE           = 0,
    B_COLLISION      = 1,
    B_DESTRUCTIBLE   = 2,
    B_UNKNOWN        = 4,
    B_SLOW           = 8,
    B_UNKNOWN2       = 16,
    B_ICE            = 32,
    B_BLOCKED        = 64,
    B_DAMAGE         = 128
};

enum TILE_TYPE : uint8_t
{
    T_NONE          = 0,
    T_DAMAGE        = 1,
    T_WETFLOOR      = 2,
    T_SAND          = 6,
    T_LOWWATER      = 4,
    T_WALLCLIMB     = 12,
    T_DEEPWATER     = 8,
    T_UNKNOWN       = 16,
    T_NOSWORD       = 32,
    T_NOJUMP        = 64,
    T_OPENBOUNDS    = 128
};

enum TILE_CONTROL : uint8_t
{
    C_UPSTAIRS      = 1,
    C_RIGHTSTAIRS   = 3,
    C_LEFTSTAIRS    = 2,
    C_FLOORCTRL     = 4,
    C_AIRCTRL       = 8,
    C_UP            = 0,
    C_DOWN          = 16,
    C_RIGHT         = 48,
    C_LEFT          = 32,
    C_PUSH          = 64,
    C_LOCKED        = 128
};

struct Tile
{
    uint8_t     Behavior; // TILE_BEHAVIOR Flags
    uint8_t     Type;     // TILE_TYPE Flags
    uint8_t     Control;  // TILE_CONTROL Flags
    uint8_t     ZPos;
    uint16_t    TexTile;
    uint16_t    WallPtr;
};

#pragma pack(push, 2)
struct MapCopy
{
    uint8_t     FromX;
    uint8_t     FromY;
    uint8_t     Width;
    uint8_t     Height;
    uint8_t     ToX;
    uint8_t     ToY;
};

struct Map
{
    uint32_t    MapSize;          // Written twice | Always 0x343C343C
    MapCopy     MapCopies[256];   // Copy map parts to be drawn elsewhere | Barely used
    Tile        MapTiles[3120];   // 52 tiles wide | 60 tiles high
};  // Also contains WallData (variable in size, tho)

struct LevelHeader
{
    uint32_t    Properties;
    uint32_t    Map;
    uint32_t    TileSet;
    uint32_t    Entities;
    uint32_t    SpriteSheet;
    uint32_t    Lining;          // Scrolling, parallax along with other scene effects
    uint32_t    Strings;
};

class Level
{
    private:

    char*           _data;
    LevelHeader*    _header;
    uint32_t        _dataSize;
    Texture*        _tileSet;
    Lining*         _lining;
    Entities*       _entities;

    uint8_t         _mapAnim[28] = { 0 };

    // Getters/Setters
    Properties*  GetProperties()    { return (Properties*) (_data + _header->Properties); }
    Map*         GetMap()           { return (Map*) (_data + _header->Map); }
    Texture*     GetTileSet()       { return _tileSet; }

    char*        GetWallsPtr()      { return (char*) (_data + _header->Map + sizeof(Map)); }
    char*        GetTSetPtr()       { return (char*) (_data + _header->TileSet); }
    char*        GetSSheetPtr()     { return (char*) (_data + _header->SpriteSheet); }
    char*        GetEntPtr()        { return (char*) (_data + _header->Entities); }
    char*        GetLiningPtr()       { return (char*) (_data + _header->Lining); }

    uint16_t     GetWallsSize()     { return _header->TileSet - (_header->Map + sizeof(Map)); }
    uint32_t     GetLiningSize()    { return _header->Strings - _header->Lining; }

    /// Returns the tile at the position
    Tile* GetTile(int x, int y) { return &GetMap()->MapTiles[y * 52 + x]; }

    /// Returns true if a rect intersects the camera view
    bool Visible(const Rect& view, const Rect& rect);

    /// Draw a tile at the specified position
    void DrawTile(Drawer* drawer, uint16_t texTile, Point pos);

    /// Updates all the timers
    void UpdateTimers();

    public:

    /// Constructor
    Level(char* levelData, uint32_t levelSize);

    /// Initializes the level
    int Init(Drawer* drawer);

    /// Draw the scene
    void DrawScene(Drawer* drawer, const Rect& cam);

    // Getters/Setters
    int GetLvlID() { return GetProperties()->LevelID; }

    /// Destructor
    ~Level();
};

#endif // LEVEL_H_INCLUDED
