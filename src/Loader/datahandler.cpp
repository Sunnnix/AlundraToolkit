#include "datahandler.h"
#include <cstdlib>

/*  _____                                                            _____  */
/* ( ___ )                                                          ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  ____        _        _   _                 _ _            |   |  */
/*  |   | |  _ \  __ _| |_ __ _| | | | __ _ _ __   __| | | ___ _ __  |   |  */
/*  |   | | | | |/ _` | __/ _` | |_| |/ _` | '_ \ / _` | |/ _ \ '__| |   |  */
/*  |   | | |_| | (_| | || (_| |  _  | (_| | | | | (_| | |  __/ |    |   |  */
/*  |   | |____/ \__,_|\__\__,_|_| |_|\__,_|_| |_|\__,_|_|\___|_|    |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                                                          (_____) */

DataHandler::DataHandler()
{
    _dataFile = NULL;
    _dataHeader = NULL;
    _fileSize = 0;
    _globalData = NULL;
    _globalSize = 0;
    _level = NULL;
}

int DataHandler::ReadFile(const char* dataPath)
{
    // Open DATAS.BIN

    if (!_dataFile)
    {
        _dataFile = fopen(dataPath, "r+b");

        if (!_dataFile)
        {
            printf("Error: Unable to open DATAS.BIN\n");
            return 1;
        }
    }

    // Get file size

    fseek(_dataFile, 0L, SEEK_END);
    _fileSize = ftell(_dataFile);
    fseek(_dataFile, 0, SEEK_SET);

    // Read the header

    if (!_dataHeader)
    {
        _dataHeader = new DataHeader;
    }

    fread(_dataHeader, 4, 512, _dataFile);

    // Check for corruption

    if (_dataHeader->GlobalObjGroup != 2048)
    {
        printf("Error: Corrupted data in DATAS.BIN\n");
        return 1;
    }

    for (int i = 0; i < 493; i++)
    {
        if (*((uint32_t*) _dataHeader + i) > _fileSize
         || *((uint32_t*) _dataHeader + i) > *((uint32_t*) _dataHeader + i + 1))
        {
            printf("Error: Corrupted data in DATAS.BIN\n");
            return 1;
        }
    }

    if (_dataHeader->Levels[483] != _fileSize)
    {
        printf("Error: Corrupted data in DATAS.BIN\n");
        return 1;
    }

    printf("DataHandler: DATAS.BIN successfully loaded\n");

    return 0;
}


int DataHandler::LoadGlobal()
{
    _globalSize = _dataHeader->Levels[0] - _dataHeader->GlobalObjGroup;

    if (_globalData)
    {
        free(_globalData);
        _globalData = NULL;
    }

    _globalData = (char*) malloc(_globalSize);

    if (!_globalData)
    {
        fprintf(stderr, "Memory allocation failed: in DataHandler::LoadGlobal()\n");
        return 1;
    }

    fseek(_dataFile, _dataHeader->GlobalObjGroup, SEEK_SET);
    fread(_globalData, _globalSize, 1, _dataFile);

    return 0;
}

int DataHandler::LoadLevel(uint16_t dex)
{
    if (dex > 482)
    {
        printf("Invalid argument: in DataHandler::LoadLevel()\n"
               "-> Index must be strictly smaller than 483\n");
        return 1;
    }

    uint32_t levelSize = _dataHeader->Levels[dex + 1] - _dataHeader->Levels[dex];
    char* levelData = (char*) malloc(levelSize);

    if (!levelData)
    {
        fprintf(stderr, "Memory allocation failed: in DataHandler::LoadLevel()\n");
        return 1;
    }

    fseek(_dataFile, _dataHeader->Levels[dex], SEEK_SET);
    fread(levelData, levelSize, 1, _dataFile);

    if (_level)
    {
        delete _level;
        _level = NULL;
    }

    _level = new Level(levelData, levelSize);

    printf("Level %d : Loaded\n", _level->GetProperties()->LevelID);

    return 0;
}

DataHandler::~DataHandler()
{
    if (_dataFile) fclose(_dataFile);
    delete _dataHeader;
    free(_globalData);
    delete _level;
}
