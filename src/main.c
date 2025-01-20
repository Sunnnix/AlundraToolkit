#include <stdio.h>
#include "util.h"
#include "recreated/DatasBin.h"

void main() {
	printf("Start decompiling...\n");
	if (decompressFileToFile("test/TILESET_C", "test/TILESET") == 0)
		printf("File decompiled!\n");
	else
		printf("Error decompiling file!\n");

	unsigned int* datasBinHeader = NULL;
	unsigned int datasBinheaderSize = 0;

	datasBinheaderSize = getTopheader("test/DATAS.BIN", &datasBinHeader);

	system("pause");
}