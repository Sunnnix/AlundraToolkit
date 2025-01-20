#pragma once

#include <stdio.h>
#include <stdlib.h>

int getTopheader(char* filepath, int** header);
int* getHeaderGlobals(char* filepath, int** section1, int** section2, int start1, int end1, int start2, int end2);