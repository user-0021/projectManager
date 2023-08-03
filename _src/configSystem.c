#include "configSystem.h"
#include "fileSystem.h"
#include "importantPath.h"
#include "minifunction.h"
#include "textList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <linux/limits.h>
#include <sys/stat.h>


bool CheckConfigDir()
{
  bool status = true;

  if(!CheckPath(PATH_CONFIG_DIR))
  {
    status = CreateDirProcess(PATH_CONFIG_DIR,0755);
  }

  return status;
}

bool AddProjectList(const ProjectList pData)
{
  bool status = false;

  struct stat sa;//まずディレクトリの安否確認
  if(stat(PATH_PROJECT_LIST_DIR,&sa))
  {
    status = CreateDirProcess(PATH_PROJECT_LIST_DIR,0755);
  }

  //ディレクトリ内のすべてのファイルを調べる
  DIR *projectListDir;
  struct dirent *dp;
  
  projectListDir = opendir(PATH_PROJECT_LIST_DIR);
  dp = readdir(projectListDir);

  //結合用
  char objectPath[PATH_MAX];
  snprintf(objectPath,PATH_MAX,"%s/",PATH_PROJECT_LIST_DIR);
  size_t directoryPathSize = strlen(objectPath);

  while (dp != NULL) 
  {//statでファイルかのチェックもする
    strcat(objectPath,dp->d_name);
    stat(objectPath,&sa);

    if(strcmp(dp->d_name,pData.projectName) == 0 && (sa.st_mode & __S_IFMT) == __S_IFREG)
    {
      printf("Err: Alredy created same name project.\n");
      exit(EXIT_FAILURE);
    }

    objectPath[directoryPathSize] = '\0';
    dp = readdir(projectListDir);
  }

  closedir(projectListDir);

  //生成
  char* filePath = (char*)malloc(strlen(PATH_PROJECT_LIST_DIR)+strlen(pData.projectName)+2);
  strcpy(filePath,PATH_PROJECT_LIST_DIR);
  strcat(filePath,"/");
  strcat(filePath,pData.projectName);

  size_t buffSize = strlen(pData.projectName)+strlen(pData.projectBrief)+strlen(pData.projectPath)+30;
  char* fileData = (char*)malloc(buffSize);
  snprintf(fileData,buffSize,"%s\n%s\n%s\n%u\n%d",pData.projectName,pData.projectBrief,pData.projectPath,pData.projectLang,(int)pData.useGithub);

  CreateAndWriteFileProcess(filePath,fileData);

  free(filePath);
  free(fileData);

  return status;
}

bool DeleteProject(const ProjectList* project)
{
  bool status = true;

  //プロジェクトフォルダ削除
  char command[PATH_MAX+15];
  strcpy(command,"rm -R ");
  strcat(command,project->projectPath);
  system(command);

  struct stat sa;//まずディレクトリの安否確認
  if(!stat(project->projectPath,&sa))
  {
    printf(TEXT_CREATE_DIRCTORY_NEED_SUDDO,project->projectPath);

    if(GetYESorNO())
    {
      strcpy(command,"sudo rm -R ");
      strcat(command,project->projectPath);
      status = system(command); 
    }
  }

  //プロジェクトファイル削除
  strcpy(command,"rm ");
  strcat(command,PATH_PROJECT_LIST_DIR);
  strcat(command,"/");
  strcat(command,project->projectName);
  system(command);

  if(!stat(project->projectPath,&sa))
  {
    printf(TEXT_CREATE_DIRCTORY_NEED_SUDDO,project->projectPath);

    if(GetYESorNO())
    {
      strcpy(command,"sudo rm ");
      strcat(command,PATH_PROJECT_LIST_DIR);
      strcat(command,"/");
      strcat(command,project->projectName);
      status = system(command);
    }
  }


  return status;
}

ProjectList* GetProjectList()
{
  ProjectList* head = NULL;

  DIR *projectListDir;
  struct dirent *dp;
  
  struct stat sa;//まずディレクトリの安否確認
  if(stat(PATH_PROJECT_LIST_DIR,&sa))
  {
    printf("Err:not found projectList directory\n");
    exit(EXIT_FAILURE);
  }

  //ディレクトリ内を探査する
  projectListDir = opendir(PATH_PROJECT_LIST_DIR);
  dp = readdir(projectListDir);

  char objectPath[PATH_MAX];
  snprintf(objectPath,PATH_MAX,"%s/",PATH_PROJECT_LIST_DIR);
  size_t directoryPathSize = strlen(objectPath);

  char* tmp[PATH_MAX];
  while (dp != NULL) 
  {//statでファイルかも確認
    strcat(objectPath,dp->d_name);
    stat(objectPath,&sa);

    if((sa.st_mode & __S_IFMT) == __S_IFREG)
    {
      FILE* fp = fopen(objectPath,"r");
      if(fp == NULL)
      {//開けなかったら
        perror(objectPath);
        exit(EXIT_FAILURE);
      }
      
      ProjectList* node = (ProjectList*)malloc(sizeof(ProjectList));

      size_t strSize;
      char* data; 
      //scan Name 
      fgets(tmp,PATH_MAX,fp);
      strSize = strlen(tmp);
      data = (char*)malloc(strSize);
      memcpy(data,tmp,strSize);
      data[strSize-1] = '\0';//\n削除
      node->projectName = data;
      //scan Brief
      fgets(tmp,PATH_MAX,fp);
      strSize = strlen(tmp);
      data = (char*)malloc(strSize);
      memcpy(data,tmp,strSize);
      data[strSize-1] = '\0';//\n削除
      node->projectBrief = data;
      //scan Path
      fgets(tmp,PATH_MAX,fp);
      strSize = strlen(tmp);
      data = (char*)malloc(strSize);
      memcpy(data,tmp,strSize);
      data[strSize-1] = '\0';//\n削除
      node->projectPath = data;
      //scan Lang
      fscanf(fp,"%u",&node->projectLang);
      //scan Github
      fscanf(fp,"%c",tmp);
      node->useGithub = tmp[0] == '1';
      //set Null next
      node->next = NULL;
  
      if(head == NULL)
      {
        head = node;
      }
      else
      {
        ProjectList* itr;
        itr = head;
        while(itr->next != NULL)
        {
          itr = itr->next;
        }
        
        itr->next = node;
      }
    }

    objectPath[directoryPathSize] = '\0';
    dp = readdir(projectListDir);
  }

  closedir(projectListDir);

  return head;
}

void PrintProjectList(ProjectList* head)
{
  while(head != NULL)
  {
    printf(TEXT_CREATE_PROFECT_INPUT_STATUS,head->projectName,head->projectBrief,TEXT_PROGRAM_LANG[head->projectLang],head->projectPath,TEXT_BOOL[head->useGithub]);
    head = head->next;
  }
}

void FreeProjectList(ProjectList* head)
{
  ProjectList* back = head;
  while(head->next != NULL)
  {
    head = head->next;
    free(back->projectName);
    free(back->projectBrief);
    free(back->projectPath);
    free(back);

    back = head;
  } 

  free(head); 
}

