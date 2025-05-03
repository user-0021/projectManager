#pragma once

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#define S '\\'
#else
#define  _BSD_SOURCE 
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define _getcwd(a,b) getcwd(a,b)
#define S '/'
#endif

int init(char* current);
int build(char* projectName);