#include "DataHandler.h"

DataHandler::DataHandler()
{
    _dataFile = NULL;
    _dataHeader = NULL;
    _fileSize = 0;
}

bool DataHandler::ReadFile(const char* DataPath)
{
    // Open DATAS.BIN

    if (!_dataFile)
    {
        fopen_s(&_dataFile, DataPath, "r+b");

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
        printf("Error: Corrupted data\n");
        return 1;
    }

    for (int i = 0; i < 493; i++)
    {
        if (*((uint32_t*) _dataHeader + i) > _fileSize
         || *((uint32_t*) _dataHeader + i) > *((uint32_t*) _dataHeader + i + 1))
        {
            printf("Error: Corrupted data\n");
            return 1;
        }
    }

    if (_dataHeader->EndOfFile != _fileSize)
    {
        printf("Error: Corrupted data\n");
        return 1;
    }

    printf("DataHandler: DATAS.BIN successfully loaded\n");

    return 0;
}

DataHandler::~DataHandler()
{
    if (_dataFile)
    {
        fclose(_dataFile);
    }

    if (_dataHeader)
    {
        delete _dataHeader;
    }
}
