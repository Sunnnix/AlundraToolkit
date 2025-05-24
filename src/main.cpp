#include <stdio.h>
#include "util.h"
#include "Loader/engine.h"

int main(int argc, char* argv[]) {
	Engine engine("test/DATAS.BIN");
	engine.Init();
	engine.Run();
	return 0;
}

//void main() {
//	printf("Start decompiling...\n");
//	if (decompressFileToFile("test/TILESET_C", "test/TILESET") == 0)
//		printf("File decompiled!\n");
//	else
//		printf("Error decompiling file!\n");
//
//	unsigned int* datasBinHeader = NULL;
//	unsigned int datasBinheaderSize = 0;
//
//	datasBinheaderSize = getTopheader("test/DATAS.BIN", &datasBinHeader);
//
//	system("pause");
//}