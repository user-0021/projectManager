#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include "textList.h"
#include "fileSystem.h"
#include "configSystem.h"
#include "promptChoose.h"
#include "importantPath.h"
#include "minifunction.h"

#include <unistd.h>

void OpenProject();
void CreateProject();
void RemoveProject();

void CreateProjectDir(const char* const basepath,uint32_t programLangIndex);

const char* PATH_CONFIG_DIR;
const char* PATH_CONFIG_FILE;
const char* PATH_PROJECT_LIST_DIR;


int main(int argc,char* argv[])
{  
  
  //user
  char* user;
  if((user = getlogin()) == NULL)
  {//failed getlogin
    printf(TEXT_SETUP_FAILED_USER_GET);
    exit(EXIT_FAILURE);
  }
  
  //make configdir path
  char* tmp = (char*)malloc(PATH_MAX);
  strcpy(tmp,"/home/");
  strcat(tmp,user);
  strcat(tmp,"/.projectManeger");
  
  char* configDir = (char*)malloc(strlen(tmp)+1);
  strcpy(configDir,tmp);
  PATH_CONFIG_DIR = configDir;

  //make configfile path
  strcat(tmp,"/.conf");
  char* configfile = (char*)malloc(strlen(tmp)+1);
  strcpy(configfile,tmp);
  PATH_CONFIG_FILE = configfile;

  //make projectlistdir path
  strcpy(tmp,PATH_CONFIG_DIR);
  strcat(tmp,"/projectList");
  char* projectListDir = (char*)malloc(strlen(tmp)+1);
  strcpy(projectListDir,tmp);
  PATH_PROJECT_LIST_DIR = projectListDir;

  //free
  free(tmp);
  
  if(!CheckConfigDir())
  {
    exit(EXIT_FAILURE);
  }

  ChooseList* actionList = CreateChooseList(TEXT_ACTION_NAME_CREATE_PROJECT,TEXT_ACTION_BRIEF_CREATE_PROJECT,&CreateProject);
  JoinChooseList(CreateChooseList(TEXT_ACTION_NAME_OPEN_PROJECT  ,TEXT_ACTION_BRIEF_OPEN_PROJECT  ,&OpenProject) ,actionList);
  JoinChooseList(CreateChooseList(TEXT_ACTION_NAME_REMOVE_CONFIG  ,TEXT_ACTION_BRIEF_REMOVE_CONFIG,&RemoveProject) ,actionList);

  OutPutChooseList(actionList);

  FreeChooseList(actionList);
  return 0;
}







void OpenProject()
{
  ProjectList* head = GetProjectList();
  if(head != NULL)
  {
    ChooseList* projectList = CreateChooseList(head->projectName,head->projectBrief,NULL);

    ProjectList* itr = head;
    while(itr->next != NULL)
    {
      itr = itr->next;
      JoinChooseList(CreateChooseList(itr->projectName,itr->projectBrief,NULL),projectList);
    }

    uint32_t openProjectIndex = OutPutChooseList(projectList) - 1;
    while(openProjectIndex > 0)
    {
      openProjectIndex--;
      head = head->next;
    }

    char* command = (char*)malloc(strlen(head->projectPath)+6);
    strcpy(command,"code ");
    strcat(command,head->projectPath);
    system(command);
  }
}

void RemoveProject()
{
  ProjectList* head = GetProjectList();
  
  if(head != NULL)
  {
    ChooseList* projectList = CreateChooseList(head->projectName,head->projectBrief,NULL);

    ProjectList* itr = head;
    while(itr->next != NULL)
    {
      itr = itr->next;
      JoinChooseList(CreateChooseList(itr->projectName,itr->projectBrief,NULL),projectList);
    }

    uint32_t openProjectIndex = OutPutChooseList(projectList) - 1;
    while(openProjectIndex > 0)
    {
      openProjectIndex--;
      head = head->next;
    }

    if(DeleteProject(head))
    {
      printf(TEXT_REMOVE_PROJECT_SUCCESS);
    }
    else
    {
      printf(TEXT_REMOVE_PROJECT_FAILED);
    }
  }
}


void CreateProject()//クソコードを生成してしまった気がする
{//I need more smart code.
  char* projectName = NULL,*projectBrief = NULL,*projectPath = NULL;
  char* projectLanguage = (char*)malloc(sizeof(TEXT_PROGRAM_LANG[0]));
  projectLanguage[0] = '\0';

  bool useGithub = false;

  printf(TEXT_ROLLE_OUT);//flash prompt
  
  //print data
  printf(TEXT_CREATE_PROFECT_INPUT_STATUS,projectName,projectBrief,projectLanguage,projectPath,TEXT_BOOL[useGithub]);
  
  //get project name
  printf(TEXT_CREATE_PROFECT_INPUT_NAME);

  projectName = scanStr();

  while(!CanUseFileName(projectName))
  {
    printf(TEXT_CREATE_PROFECT_RETYPE_NAME);
    free(projectName);
    
    projectName = scanStr();
  }
  
  printf(TEXT_ROLLE_OUT);//flash prompt
  
  //print data
  printf(TEXT_CREATE_PROFECT_INPUT_STATUS,projectName,projectBrief,projectLanguage,projectPath,TEXT_BOOL[useGithub]);

  //get project brief
  printf(TEXT_CREATE_PROFECT_INPUT_BRIEF);
  projectBrief = scanStr();
  
  //print data
  printf(TEXT_CREATE_PROFECT_INPUT_STATUS,projectName,projectBrief,projectLanguage,projectPath,TEXT_BOOL[useGithub]);
  
  //get program lang
  ChooseList* langList = CreateChooseList(TEXT_PROGRAM_LANG[0],NULL,NULL);
  size_t langListSize = sizeof(TEXT_PROGRAM_LANG)/sizeof(TEXT_PROGRAM_LANG[0]);
  size_t i;
  for(i = 1;i < langListSize;i++)
  {
    JoinChooseList(CreateChooseList(TEXT_PROGRAM_LANG[i],NULL,NULL),langList);
  }

  uint32_t langListIndex = OutPutChooseList(langList) - 1;
  strcpy(projectLanguage,TEXT_PROGRAM_LANG[langListIndex]);

  //print data
  printf(TEXT_CREATE_PROFECT_INPUT_STATUS,projectName,projectBrief,projectLanguage,projectPath,TEXT_BOOL[useGithub]);

  printf(TEXT_CREATE_PROFECT_INPUT_PATH);

  //get program path
  projectPath = scanPath();

  while(!CheckPath(projectPath))
  {
    printf(TEXT_CREATE_PROFECT_RETYPE_PATH);
    free(projectPath);
    
    projectPath = scanPath();
  }
  realloc(projectPath,strlen(projectPath)+strlen(projectName)+1);
  strcat(projectPath,projectName);
    
  //print data
  printf(TEXT_CREATE_PROFECT_INPUT_STATUS,projectName,projectBrief,projectLanguage,projectPath,TEXT_BOOL[useGithub]);
  

  //get use github 
  printf(TEXT_CREATE_PROFECT_INPUT_GITHUB);
  useGithub = GetYESorNO();
    
  //print data
  printf(TEXT_CREATE_PROFECT_INPUT_STATUS,projectName,projectBrief,projectLanguage,projectPath,TEXT_BOOL[useGithub]);

  ProjectList pl;

  pl.projectName  = projectName;
  pl.projectBrief = projectBrief;
  pl.projectPath  = projectPath;
  pl.projectLang  = langListIndex;
  pl.useGithub    = useGithub;
  pl.next = NULL;

  AddProjectList(pl);


  CreateProjectDir(projectPath,langListIndex);

  if(useGithub)
  {
    char command[256];
    strcpy(command,"git init ");
    strcat(command,projectPath);
    
    system(command);
  }

  free(projectName);
  free(projectBrief);
  free(projectPath);
}

void CreateProjectDir_C_LANG(const char* const basepath);


void CreateProjectDir(const char* const basepath,uint32_t programLangIndex)
{
  if(CreateDirProcess(basepath,0755) == false)
  {
    printf("Err: Failed create project directory\n");
    exit(EXIT_FAILURE);
  }
  
  switch(programLangIndex)
  {
    case 0:
    CreateProjectDir_C_LANG(basepath);
  }
}

void CreateProjectDir_C_LANG(const char* const basepath)
{
  char tmpPath[256];
  bool status = true;

  //createSrcDirectory
  memcpy(tmpPath,basepath,strlen(basepath)+1);
  strcat(tmpPath,TEXT_NAME_OF_SRC_DIR);
  status &= CreateDirProcess(tmpPath,0755);

  //createIncludeDirectory
  tmpPath[strlen(basepath)] = '\0';
  strcat(tmpPath,TEXT_NAME_OF_INCLUDE_DIR);
  status &= CreateDirProcess(tmpPath,0755);

  //createmakefile
  tmpPath[strlen(basepath)] = '\0';
  strcat(tmpPath,"/makefile");
  status &= CreateAndWriteFileProcess(tmpPath,TEXT_C_LANG_MAKE_FILE);


  if(status == false)
  {
    printf("Err: Failed create project directory\n");
    exit(EXIT_FAILURE);
  } 
}

