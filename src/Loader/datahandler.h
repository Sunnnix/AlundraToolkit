#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "level.h"
#include <cstdint>
#include <cstdio>

struct DataHeader
{
    uint32_t    GlobalObjGroup;
    uint32_t    GlobalSprites;
    uint32_t    GlobalSpritesCopy;    // Safety Copy of GlobalSprites
    uint32_t    GlobalStrings;
    uint32_t    GlobalStringsCopy;    // Safety Copy of GlobalStrings
    uint32_t    Unknown;              // Probably flags. Composed of 96 blocks of 2048 bytes
    uint32_t    LoadingScreen0;       // 320 width images
    uint32_t    LoadingScreen1;
    uint32_t    LoadingScreen2;
    uint32_t    LoadingScreen3;
    uint32_t    Levels[484];          // Pointer 483 is EndOfFile
    uint32_t    NullPointers[18];
};

class DataHandler
{
    private:

    uint32_t     _fileSize;
    FILE*        _dataFile;
    DataHeader*  _dataHeader;
    char*        _globalData;
    uint32_t     _globalSize;

    public:

    /// Constructor
    DataHandler();

    /// Reads to DATAS.BIN and returns 0 if succeed
    int ReadFile(const char* dataPath);

    /// Loads global data and returns 0 if succeed
    int LoadGlobal();

    /// Loads a level and returns 0 if succeed
    Level* LoadLevel(uint16_t dex);

    /// Destructor
    ~DataHandler();

    // Getters
    char* GetGlobPtr()  { return _globalData; }
};

#endif // DATAHANDLER_H
