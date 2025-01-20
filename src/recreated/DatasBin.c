#include "DatasBin.h"

int getTopheader(char* filepath, int** header) {
	FILE* datasBin = fopen(filepath, "rb");
	
	const unsigned int bytes = 0x7B8;
	const unsigned int arraySize = bytes / sizeof(unsigned int);
	*header = (int*)malloc(bytes);

	fread(*header, sizeof(unsigned int), arraySize, datasBin);

	fclose(datasBin);

	return arraySize;
}

int* getHeaderGlobals(char* filepath, int** section1, int** section2, int start1, int end1, int start2, int end2) {
	int sizes[2];

	FILE* datasBin = fopen(filepath, "rb");

	fseek(datasBin, start1, SEEK_SET);
	sizes[0] = end1 - start1;
	*section1 = (int*)malloc(sizes[0]);
	fread(*section1, sizeof(unsigned int), sizes[0], datasBin);

	fseek(datasBin, start2, SEEK_SET);
	sizes[1] = end2 - start2;
	*section2 = (int*)malloc(sizes[1]);
	fread(*section2, sizeof(unsigned int), sizes[1], datasBin);

	fclose(datasBin);

	return sizes;
}