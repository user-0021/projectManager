/** @file   promptChoose.h
 *  @brief  For CommandPromptChooseBox
 *  @author user-0021
 *  @date   2023/07/10
*/
#pragma once

#include <stdint.h>

typedef void(*ChooseCallBack)();

typedef struct _chooseList  ChooseList;

/**
 * @struct _chooseList 
 * @brief List For PromptChoos(typedef to ChooseList)
*/
struct _chooseList
{
  /// @brief choiseName
  char* choiseName;
  /// @brief choiseBrief
  char* choiseBrief;
  /// @brief callback for choise action
  ChooseCallBack callback; 
 
  /// @brief next Pointer(need zero clear)
  ChooseList* next;
};


/**
 * ************************************
 * @fn OutPutChooseList(const ChooseList* const list)
 * @brief outPut ChooseList to stdout
 * 
 * @param [in] list chooseList 
 * 
 * @return choise number
 * ************************************
 */
uint32_t OutPutChooseList(const ChooseList* const list);

/**
 * ************************************
 * @fn CreateChooseList(const char* const name,const char* const brief,const ChooseCallBack const callBack)
 * @brief CreateChooseList
 * @details Need to release by FreeChooseList(ChooseList*);
 * 
 * @param [in] name     ChoiseName
 * @param [in] brief    ChoiseBrief
 * @param [in] callBack ChoiseCallBack
 * 
 * @return pointer to List
 * 
 * ************************************
 */
ChooseList* CreateChooseList(const char* const name,const char* const brief,const ChooseCallBack const callBack);

/**
 * ************************************
 * @fn JoinChooseList(const ChooseList* const new,ChooseList* list)
 * @brief pushBack Object
 * 
 * @param [in]     new     object to join
 * @param [in,out] list    list for join
 *  
 * ************************************
 */
void JoinChooseList(const ChooseList* const new,ChooseList* list);

/**
 * ************************************
 * @fn FreeChooseList(ChooseList* head)
 * @brief free ChooseList
 * 
 * @param [in] head ListHead
 * ************************************
 */
void FreeChooseList(ChooseList* head);


