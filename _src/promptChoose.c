#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "promptChoose.h"
#include "textList.h"

uint32_t OutPutChooseList(const ChooseList* const list)
{
  printf("\n\n\n\n--------------------Please Selsect--------------------\n\n");
  uint32_t counter=0;
  const ChooseList* itr = list;
  while(itr != 0)
  {
    counter++;
    printf("%u.  %s \n",counter,itr->choiseName);
    if(itr->choiseBrief != NULL)
      printf("  --%s\n\n",itr->choiseBrief);
    else
      putchar('\n');
    
    itr = itr->next;
  }
  
  printf("------------------------------------------------------\n\n");

  printf("%s",TEXT_CHOOSE_LIST_TYPE_NUMBER);

  uint32_t inNum = 0;
  scanf("%u",&inNum);
  while(inNum == 0 || inNum > counter)
  {
    printf("%s",TEXT_CHOOSE_LIST_RETYPE_NUMBER);
    if(scanf("%u",&inNum) <= 0)
    {
      while(getchar() != '\n');
    }
  }

  printf(TEXT_CHOOSE_LIST_INFO_NUMBER,inNum);
  itr = list;
  for(int i = 1;i < inNum;i++)
  {
    itr = itr->next;
  }

  if(itr->callback != NULL)
  {
    itr->callback();
  }

  return inNum;
}


ChooseList* CreateChooseList(const char* const name,const char* const brief,const ChooseCallBack const callBack)
{
  ChooseList* ret  = (ChooseList*)malloc(sizeof(ChooseList));
  ret->choiseName  = (char*)malloc(strlen(name)+1);
  ret->callback    = callBack;
  ret->next        = NULL;

  strcpy(ret->choiseName,name);

  if(brief != NULL)
  {//briefがNULLじゃないなら
    ret->choiseBrief = (char*)malloc(strlen(brief)+1);
    strcpy(ret->choiseBrief,brief);
  }
  else
  {
    ret->choiseBrief = NULL;    
  }

  return ret;
}

void JoinChooseList(ChooseList* new,ChooseList* list)
{
  ChooseList* listEnd = list;

  while(listEnd->next != NULL)
  {
    listEnd = listEnd->next;
  }

  listEnd->next = new;
}

void FreeChooseList(ChooseList* head)
{
  ChooseList* back = head;
  while(head->next != NULL)
  {
    head = head->next;
    free(back->choiseName);
    free(back->choiseBrief);
    free(back);

    back = head;
  } 

  free(head);
}
