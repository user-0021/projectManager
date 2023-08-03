#include "minifunction.h"
#include "textList.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include <unistd.h>

bool GetYESorNO()
{
  char buf[PATH_MAX] = "";

  scanf("%s",buf);
  while((strcmp(buf,"yes") != 0)&&(strcmp(buf,"no") != 0))
  {
    printf(TEXT_REMOVE_CONFIG_PLEASE_RETYPE);
    scanf("%s",buf);
  }

  return !strcmp(buf,"yes");
}





bool CanUseFileName(const char* name)
{
  if(strlen(name) ==  0)
    return false;

  while ((*name) != '\0')
  {
    switch(*name)
    {
      default:
        break;
      case '\\':
      case '/':
      case ':':
      case '*':
      case '?':
      case '\"':
      case '>':
      case '<':
      case '|':
        return false;
    }
    name++;
  }
  return true;
}


bool CheckPath(char* path)
{
  struct stat st;
  printf(path);
  return !stat(path,&st);
}

char* scanStr()
{
  
  char* buff[PATH_MAX];//流石にPATHMAXあればいいという慢心
  buff[0] = (char*)malloc(256);
  uint8_t itr = 0;
  size_t offset = 0;

  //なんかうまく動かないのでnonblockingでごりおし(経験不足ですハイ) 
  int flags = fcntl(0, F_GETFL, 0);
  fcntl(0, F_SETFL, flags | O_NONBLOCK);

  while(getchar() != -1);

  //後始末
  fcntl(0, F_SETFL, flags);


  while(1)
  {
    char c = getchar();
    if(c == '\n')
      break;

    buff[offset][itr++] = c;

    if(itr == 0)//オーバーフローを利用した天才的☆プログラム    
    {
      buff[++offset] = (char*)malloc(256);
      if(offset == PATH_MAX)
      {
        printf("Err: String buff overflow\n");
        exit(EXIT_FAILURE);
      }    
    }
  }

  buff[offset][itr++] = '\0';
  char *ret = (char*)malloc(offset*256 + itr);

  if(ret == NULL)//大規模なメモリの確保になる可能性もあるので
  {
    perror("malloc for string ");
    exit(EXIT_FAILURE);
  }

  size_t i;
  for(i = 0;i < offset;i++)
  {
    memcpy(ret+(i*256),buff[i],256);
  }

  memcpy(ret+(i*256),buff[i],itr);
  
  for(i = 0;i < offset+1;i++)
  {
    free(buff[i]);
  }

  return ret;
}


char* scanPath()
{
  
  char* buff[PATH_MAX];//流石にPATHMAXあればいいという慢心
  buff[0] = (char*)malloc(256);
  uint8_t itr = 0;
  size_t offset = 0;

  //なんかうまく動かないのでnonblockingでごりおし(経験不足ですハイ) 
  int flags = fcntl(0, F_GETFL, 0);
  fcntl(0, F_SETFL, flags | O_NONBLOCK);

  while(getchar() != -1);

  //後始末
  fcntl(0, F_SETFL, flags);


  while(1)
  {
    char c = getchar();
    if(c == '\n')
      break;

    buff[offset][itr++] = c;

    if(itr == 0)//オーバーフローを利用した天才的☆プログラム    
    {
      buff[++offset] = (char*)malloc(256);
      if(offset == PATH_MAX)
      {
        printf("Err: String buff overflow\n");
        exit(EXIT_FAILURE);
      }    
    }
  }

  //終わりにNull文字挿入
  buff[offset][itr++] = '\0';
  
  //相対パスを絶対パス化する
  char cwd[PATH_MAX];
  size_t cwdSize = 0;
  uint8_t invalidChar = 0;
  
  
  if(buff[0][0] == '.')
  {//めんどくさいのでifネストで絶対パス化
    if(buff[0][1] == '.')
    {
      if(buff[0][2] == '/')
      {
        getcwd(cwd,PATH_MAX);
        cwdSize = strlen(cwd);
        invalidChar = 2;

        for(cwdSize--;cwdSize != 0;cwdSize--)
        {
          if(cwd[cwdSize] == '/')
            cwd[cwdSize] == '\0';//文字列の終端を一つ上のディレクトリのとこにする。
            break;
        }
      }
    }
    else if(buff[0][1] == '/' || buff[0][1] == '\0')
    {
      getcwd(cwd,PATH_MAX);
      cwdSize = strlen(cwd);
      invalidChar = 1;
    }
  }

  char *ret = (char*)malloc(offset*256 + itr + cwdSize+1);

  if(ret == NULL)//大規模なメモリの確保になる可能性もあるので
  {
    perror("malloc for string ");
    exit(EXIT_FAILURE);
  }

  memcpy(ret,cwd,cwdSize);

  size_t i;
  for(i = 0;i < offset;i++)
  {
    if(i == 0)
    {
      memcpy(ret+cwdSize,buff[0]+invalidChar,256-invalidChar);
    }
    else
    {
      memcpy(ret+(i*256)+cwdSize,buff[i]+invalidChar,256);
    }
  }

  if(i == 0)
  {
    memcpy(ret+cwdSize,buff[0]+invalidChar,itr-invalidChar);
  }
  else
  {
    memcpy(ret+(i*256)+cwdSize,buff[i]+invalidChar,itr);
  }
  
  if(ret[(i*256)+cwdSize+itr-2-invalidChar] != '/')
    strcat(ret,"/");

  for(i = 0;i < offset+1;i++)
  {
    free(buff[i]);
  }

  return ret;
}