/*
 * A quick and dirty way to test things fast.
 * Acts as an entry point for the engine.
 */
#include <engine.h>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[])
{
    const char *dataPath = "DATAS.BIN";

    if (argc > 1)
    {
        dataPath = argv[1];
    }
    printf("Using data path: %s\n", dataPath);

    Engine engine(dataPath);

    if (engine.Init() != 0)
    {
        fprintf(stderr, "Error initializing engine\n");
        return EXIT_FAILURE;
    }

    engine.Run();

    return EXIT_SUCCESS;
}