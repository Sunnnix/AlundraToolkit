#include <stdio.h>
#include "util.h"

void main() {
	printf("Start decompiling...\n");
	if (decompressFileToFile("res/TILESET_C", "TILESET") == 0)
		printf("File decompiled!\n");
	else
		printf("Error decompiling file!\n");
	system("pause");
}