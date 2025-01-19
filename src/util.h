#include <stdio.h>

int decompressStreamToFile(FILE* stream, long from, long to, const char* toPath);

int decompressFileToFile(char* fromPath, char* toPath);