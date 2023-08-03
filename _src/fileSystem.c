#include "fileSystem.h"
#include "minifunction.h"
#include "textList.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

bool CreateDirProcess(const char* const path,int mode)
{
  bool status = false;
  
  if(mkdir(path,mode))
  {//filed make directory
    if(errno == EACCES)
    {//if becouse of parmision
      printf(TEXT_CREATE_DIRCTORY_NEED_SUDDO,path);

      if(GetYESorNO())
      {//yes
        char* tmp = (char*)malloc(strlen(path)+12);
        strcpy(tmp,"sudo mkdir ");
        strcat(tmp,path);

        status = !system(tmp);
        free(tmp);
      }
    }
    else
    {//ather
      char* tmp = (char*)malloc(strlen(path)+8);
      strcpy(tmp,"Create ");
      strcat(tmp,path);
      perror(tmp);
      free(tmp);
    }
  }
  else
  {//no problem
    status = true;
  }

  return status;
}


bool CreateAndWriteFileProcess(const char* const path,const char*const writeText)
{

  bool status = false;

  FILE* fp;

  if((fp = fopen(path,"w")) == NULL)
  {//filed make directory
    if(errno == EACCES)
    {//if becouse of parmision
      printf(TEXT_CREATE_DIRCTORY_NEED_SUDDO,path);

      if(GetYESorNO())
      {//yes
        char* tmp = (char*)malloc(strlen(path)+12);
        strcpy(tmp,"sudo touch ");
        strcat(tmp,path);

        status = !system(tmp);

        free(tmp);

        tmp = (char*)malloc(strlen(writeText)+strlen(path)+17);
        strcpy(tmp,"sudo echo \"");
        strcat(tmp,writeText);
        strcat(tmp,"\" > ");
        strcat(tmp,path);
        
        status = !system(tmp);

        free(tmp);
      }
    }
    else
    {//ather
      char* tmp = (char*)malloc(strlen(path)+8);
      strcpy(tmp,"Create ");
      strcat(tmp,path);
      perror(tmp);
      free(tmp);
    }
  }
  else
  {//no problem

    fprintf(fp, writeText);
    fclose(fp);
    status = true;
  }

  return status;
}