/** @file   configSystem.h
 *  @brief  For ProjectList
 *  @author user-0021
 *  @date   2023/07/13
*/
#pragma once
#include <stdint.h>
#include <stdbool.h>


typedef struct _projectList  ProjectList;

/**
 * @struct _projectList
 * @brief List For Project(typedef to ProjectList)
*/
struct _projectList
{
  /// @brief projectName
  char* projectName;
  /// @brief projectBrief
  char* projectBrief;
  /// @brief projectPath
  char* projectPath;
  /// @brief projectLang
  uint32_t projectLang;
  /// @brief useGithub
  bool useGithub;
  
  /// @brief next Pointer(need zero clear)
  ProjectList* next;
};


/**
 * ************************************
 * @fn CheckConfigDir()
 * @brief Check Config Dir.
 * @details if not found directory this function make new directory
 *  
 * @return return false,If failed create directory
 * ************************************
 */
bool CheckConfigDir();

/**
 * ************************************
 * @fn AddProjectList(const ProjectList pData)
 * @brief Add project to project roster
 * 
 * @param [in] pdata projectData 
 * 
 * @return return false if projectData invalid
 * ************************************
 */
bool AddProjectList(const ProjectList pData);

/**
 * ************************************
 * @fn DeleteProject(const ProjectList* project)
 * @brief Delete project
 * 
 * @param [in] pList Project tp delete
 * 
 * @return return false if failed Delete
 * ************************************
 */
bool DeleteProject(const ProjectList* project);

/**
 * ************************************
 * @fn GetProjectList()
 * @brief GetProjectList
 * 
 * @return projectList(If project count zero,return NULL)
 * ************************************
 */
ProjectList* GetProjectList();

/**
 * ************************************
 * @fn PrintProjectList(ProjectList* head)
 * @brief Print stdout project list;
 * 
 * @param [in] head projecList head
 * 
 * ************************************
 */
void PrintProjectList(ProjectList* head);

/**
 * ************************************
 * @fn FreeProjectList(ProjectList* head)
 * @brief Free project list;
 * 
 * @param [in] head projecList head to free
 * 
 * ************************************
 */
void FreeProjectList(ProjectList* head);
