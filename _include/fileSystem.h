#pragma once

#include <stdint.h>
#include <stdbool.h>

bool CreateDirProcess(const char* const path,int mode);
bool CreateAndWriteFileProcess(const char* const path,const char*const writeText);