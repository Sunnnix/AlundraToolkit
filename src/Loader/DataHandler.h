#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <cstdint>
#include <cstdio>

struct DataHeader
{
    uint32_t GlobalObjGroup;
    uint32_t GlobalSprites;
    uint32_t GlobalSpritesCopy;    // Safety Copy of GlobalSprites
    uint32_t GlobalStrings;
    uint32_t GlobalStringsCopy;    // Safety Copy of GlobalStrings
    uint32_t Unknown;              // Probably flags. Composed of 96 blocks of 2048 bytes
    uint32_t LoadingScreen0;       // 320 width images
    uint32_t LoadingScreen1;
    uint32_t LoadingScreen2;
    uint32_t LoadingScreen3;
    uint32_t Levels[483];
    uint32_t EndOfFile;
    uint32_t NullPointers[18];
};

class DataHandler
{
    private:

    uint32_t _fileSize;
    FILE* _dataFile;
    DataHeader* _dataHeader;

    public:

    /// Constructor
    DataHandler();

    /// Reads to DATAS.BIN and returns 0 if succeed
    bool ReadFile(const char*);

    /// Destructor
    ~DataHandler();
};

#endif // DATAHANDLER_H
