#ifndef ENTITIES_H_INCLUDED
#define ENTITIES_H_INCLUDED

#include <cstdint>
#include "imaging.h"

#pragma pack(push, 2)
struct SubSprite
{
    uint8_t     SheetDex;       // & 0x07
    uint8_t     Palette;        // & 0x1F
    uint8_t     X;
    uint8_t     Y;
    uint8_t     Width;
    uint8_t     Height;
    int8_t      XTLStr;         // Top-Left stretch on the X axis
    int8_t      YTLStr;         // Top-Left on Y
    int8_t      XTRStr;         // Top-Right on X
    int8_t      YTRStr;         // Top-Right on Y
    int8_t      XBLStr;         // Bottom-Left on X
    int8_t      YBLStr;         // Bottom-Left on Y
    int8_t      XBRStr;         // Bottom-Right on X
    int8_t      YBRStr;         // Bottom-Right on Y
};

#pragma pack(push, 2)
struct Collision
{
    int8_t      XPos;   // default 0x7F
    int8_t      YPos;   // default 0x7F
    int8_t      ZPos;   // default 0x00
    uint8_t     XLen;   // default 0x00
    uint8_t     YLen;   // default 0x00
    uint8_t     ZLen;   // default 0x00
};

// Modes: 00 -> Doesn't loop (Next state follows OR nothing if < 0) | 01 -> Does loop
#pragma pack(push, 1)
struct Anim
{
    int8_t      Delay;          // Delay < 0 | Modes >= 0
    uint16_t    CollisionPtr;
    uint16_t    SpritePtr;      // Ptr << 1
};

#pragma pack(push, 2)
struct State
{
    uint16_t    DownAnim;
    uint16_t    UpAnim;
    uint16_t    LeftAnim;
    uint16_t    RightAnim;
    uint16_t    HSpeed;
    int16_t     VSpeed;
    uint8_t     Sound;
    uint8_t     Flags;      // DefHit Off | AttHit Off | Sound Off | Shadow Off | Last 4bits is for Inertia
};

struct ObjectHeader
{
    uint32_t    Dynamics;
    uint32_t    Animations;
    uint32_t    Collisions;
    uint32_t    Sprites;
};

struct EntitiesHeader
{
    uint32_t    ObjInst;
    uint32_t    AbstInst;
    uint32_t    EvntInst;
    uint32_t    IndexPtr;
    uint32_t    AbstractPtr;
    uint32_t    Palettes;
    uint32_t    LoadEventPtr;
    uint32_t    InteractEventPtr;
    uint32_t    StateEventPtr;
    uint32_t    CollisionEventPtr;
    uint32_t    UnknownEventPtr;
    uint32_t    DataEventPtr;      // Contains AbstractEventPtr
};

class Object
{
    private:

    char*            _base;
    char*            _data;
    ObjectHeader*    _header;
    uint8_t          _dexID;

    public:

    /// Constructor
    Object(char* basePtr, uint32_t dataPtr, uint8_t  dexID);

    /// Destructor
    ~Object();

    // Getters

    char*    GetDynamics()       { return (char*) (_base + _header->Dynamics); }
    char*    GetAnimations()     { return (char*) (_base + _header->Animations); }
    char*    GetCollisions()     { return (char*) (_base + _header->Collisions); }
    char*    GetSprites()        { return (char*) (_base + _header->Sprites); }
};

class Entities
{
    private:

    char*               _data;
    EntitiesHeader*     _header;
    Texture*            _spriteSheet;
    Object*             _objects[256];

    public:

    /// Constructor
    Entities(char* data, char* texPtr);

    /// Draw a sprite onto a bitmap
    void DrawSprite(Drawer* const& drawer, Point pos, uint8_t obj, uint16_t spritePtr);

    /// Destructor
    ~Entities();

    // Getters

    uint16_t*     GetPalPtr()              { return (uint16_t*) (_data + _header->Palettes); }
    uint32_t      GetPalSize()             { return _header->LoadEventPtr - _header->Palettes; }
    Object*       GetObject(uint8_t dex)   { return _objects[dex]; }
    Texture*      GetSpriteSheet()         { return _spriteSheet; }
};

#endif // OBJECTS_H_INCLUDED
