#include <stdio.h>
#include <stdlib.h>

int decompressStreamToFile(FILE* stream, long from, long to, const char* toPath) {
    FILE* toStream = fopen(toPath, "wb+");
    if (toStream == NULL) {
        perror("Can't open export file stream");
        return -1;
    }

    fseek(stream, from, SEEK_SET);

    unsigned char cByte;  // Current read byte
    unsigned char pSize;  // Pattern size (length of bytes to jump back)
    unsigned char length; // bytes to copy
    long writePos;        // Position in the output stream

    while ((to == -1 || ftell(stream) < to) && fread(&cByte, 1, 1, stream) == 1) {
        if (cByte == 0xAD) { // compression byte
            if (fread(&pSize, 1, 1, stream) != 1) {
                perror("Couldn't read pattern size");
                fclose(toStream);
                return -1;
            }

            if (pSize == 0) { // 0xAD is a literal and no compression byte
                fwrite(&cByte, 1, 1, toStream);
                continue;
            }

            if (fread(&length, 1, 1, stream) != 1) {
                perror("Couldn't read length of bytes to copy");
                fclose(toStream);
                return -1;
            }

            writePos = ftell(toStream);
            long copyStart = writePos - pSize;

            if (copyStart < 0) {
                perror("Invalid pattern start position");
                fclose(toStream);
                return -1;
            }

            for (int i = 0; i < length; i++) {
                long readPos = copyStart + (i % pSize);
                fseek(toStream, readPos, SEEK_SET);
                if (fread(&cByte, 1, 1, toStream) != 1) {
                    perror("Error reading pattern from output stream");
                    fclose(toStream);
                    return -1;
                }
                fseek(toStream, writePos + i, SEEK_SET);
                fwrite(&cByte, 1, 1, toStream);
            }

            fseek(toStream, writePos + length, SEEK_SET);
        }
        else {
            fwrite(&cByte, 1, 1, toStream);
        }
    }

    fclose(toStream);
    return 0;
}

int decompressFileToFile(char* fromPath, char* toPath)
{
    FILE* fromFile = fopen(fromPath, "rb");
    int r = decompressStreamToFile(fromFile, 0, -1, toPath);
    fclose(fromFile);
	return r;
}