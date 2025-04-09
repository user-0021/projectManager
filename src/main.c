#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <linear_list/linear_list.h>
#include <stringLib/stringLib.h>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#define S '\\'
#else
#include <unistd.h>
#include <sys/stat.h>
#define _getcwd(a,b) getcwd(a,b)
#define S '/'
#endif

#define VERSION "v0.0.0"

typedef enum{
	ID_CC 			= 0,
	ID_CFLAG 		= 1,
	ID_OBJFLAG 		= 2,
	ID_SOURCEDIR 	= 3,
	ID_INCLUDEDIR 	= 4,
	ID_OBJDIR 		= 5,
	ID_OUTPUT 		= 6,
	ID_LIB 			= 7
} settingNameId;

const char* settingName[8] = {  "CC",
								"C_FLAG",
								"OBJ_FLAG",
								"SOURCE_DIR",
								"INCLUDE_DIR",
								"OBJ_DIR",
								"OUTPUT",
								"LIB"
							};
							
typedef struct{
	char* NAME;
	char** CC;
	char** C_FLAG;
	char** OBJ_FLAG;
	char** SOURCE_DIR;
	char** INCLUDE;
	char** OBJ_DIR;
	char** OUTPUT;
	char** LIB;
} NAME_SPACE;


void init(char* current);
void build(char* name);


int main(int argc,char** argv){

	//show version
	if(argc == 1 || strlistOR(argv[1],4,"version","v","-v","/v") == 0 ){
		printf("%s\n",VERSION);
		exit(0);
	}

	//project init
	else if(strlistOR(argv[1],4,"init","i","-i","/i") == 0){
		init(argv[2]);
		exit(0);
	}

	//build project
	else if(strlistOR(argv[1],4,"build","b","-b","/b") == 0){
		build(argv[2]);
		exit(0);
	}


}

void init(char* current){
	char* projectPath;

	if(current != NULL){//change dir
		if(chdir(current) != 0){
			fprintf(stderr,"%s is invalid path.\n",current);
			exit(0);
		}
	}

	//get current path
	projectPath = _getcwd(NULL,0);
	if(projectPath == NULL){
		fprintf(stderr,"Project directory path invalid.\n");
	}
	
	printf("Initialize project on %s\n",projectPath);

	//check project file
	FILE* fd = fopen(".projectManagerConfig","r");
	if(fd != NULL){
		fclose(fd);//Because un used.

		printf("This directory has project configuration file.\n");
		printf("Will be over write?(y/n)\n");

		char input[256];
		while (1)
		{
			scanf("%s",input);
			
			if(strlistOR(input,2,"y","Y") == 0){
				break;
			}else if(strlistOR(input,2,"n","N") == 0){
				return;
			}
		}
	}

	//make project file
	if(NULL != (fd = fopen(".projectManagerConfig","w"))){
		#ifdef WIN32
		//mkdir
		_mkdir("src");
		_mkdir("include");
		_mkdir("obj");
		
		//make file
		fprintf(fd,	"#Project info\n"
					"CC = gcc\n"
					"C_FLAG =\n"
					"OBJ_FLAG =\n"
					"LIBRARY=\n"
					"SOURCE_DIR = src\n"
					"INCLUDE_DIR = include\n"
					"OBJ_DIR = obj\n"
					"LIB =\n\n\n"
					"#Output\n"
					"OUTPUT = out.exe\n"
					);
		#else
		//mkdir
		mkdir("src",0755);
		mkdir("include",0755);
		mkdir("obj",0755);
		
		//make file
		fprintf(fd,	"#Project info\n"
					"CC = gcc\n"
					"C_FLAG =\n"
					"OBJ_FLAG =\n"
					"LIBRARY=\n"
					"SOURCE_DIR = src\n"
					"INCLUDE_DIR = include\n"
					"OBJ_DIR = obj\n"
					"LIB =\n\n\n"
					"#Output\n"
					"OUTPUT = out\n"
					);
		#endif			
		
		fclose(fd);
	}
}

void build(char* targetName){
	FILE* fd = fopen(".projectManagerConfig","r");
	if(fd == NULL){
		fprintf(stderr,".projectManagerConfig not found.\n");
		return;
	}

	//各種環境変数 
	NAME_SPACE* nameSpaceArray = LINEAR_LIST_CREATE(NAME_SPACE);

	//add null name
	NAME_SPACE nullName;
	nullName.NAME		= NULL;
	nullName.CC 		= LINEAR_LIST_CREATE(char*);
	nullName.C_FLAG		= LINEAR_LIST_CREATE(char*);
	nullName.OBJ_FLAG	= LINEAR_LIST_CREATE(char*);
	nullName.SOURCE_DIR	= LINEAR_LIST_CREATE(char*);
	nullName.INCLUDE	= LINEAR_LIST_CREATE(char*);
	nullName.OBJ_DIR	= LINEAR_LIST_CREATE(char*);
	nullName.OUTPUT		= LINEAR_LIST_CREATE(char*);
	nullName.LIB		= LINEAR_LIST_CREATE(char*);
	LINEAR_LIST_PUSH(nameSpaceArray,nullName);

	//めんどいので最大文字数をテケトーに決める
	char line[4096 * 128];

	while(memset(line,0,sizeof(line)) && fgets(line,sizeof(line),fd) != NULL){
		while((line[strlen(line) - 2] == '\\') && (fgets(line + strlen(line) - 2,sizeof(line) - strlen(line),fd) != NULL));
		line[strlen(line) - 1] = '\0';
		
		if(strchr(line,'#') != NULL){
			*strchr(line,'#') = '\0';
		}

		char* arg = strchr(line,'=');
		char* p = strtok(line," \t\r\n=");

		if(p != NULL && *p == '['){
			
			p = strtok(++p,"]");
			NAME_SPACE addData;
			addData.NAME		= malloc(strlen(p) + 1);
			strcpy(addData.NAME,p);
			

			addData.CC 			= LINEAR_LIST_CREATE(char*);
			addData.C_FLAG		= LINEAR_LIST_CREATE(char*);
			addData.OBJ_FLAG	= LINEAR_LIST_CREATE(char*);
			addData.SOURCE_DIR	= LINEAR_LIST_CREATE(char*);
			addData.INCLUDE		= LINEAR_LIST_CREATE(char*);
			addData.OBJ_DIR		= LINEAR_LIST_CREATE(char*);
			addData.OUTPUT		= LINEAR_LIST_CREATE(char*);
			addData.LIB			= LINEAR_LIST_CREATE(char*);

			LINEAR_LIST_PUSH(nameSpaceArray,addData);
			continue;
		}
		
		if(p == NULL || arg == NULL)
			continue;//エラー回避

		arg++;
		char** res = strGetMatchPattern(p,sizeof(settingName) / sizeof(settingName[0]),
				&settingName[0],&settingName[1],&settingName[2],&settingName[3],
				&settingName[4],&settingName[5],&settingName[6],&settingName[7]);

				
		if(res != NULL){
			NAME_SPACE* data = LINEAR_LIST_LAST(nameSpaceArray);
			char** list;
			switch ((((void*)res-(void*)settingName)/sizeof(settingName[0])))
			{
			case 0:
				list = data->CC;
				break;
			case 1:
				list = data->C_FLAG;
				break;
			case 2:
				list = data->OBJ_FLAG;
				break;
			case 3:
				list = data->SOURCE_DIR;
				break;
			case 4:
				list = data->INCLUDE;
				break;
			case 5:
				list = data->OBJ_DIR;
				break;
			case 6:
				list = data->OUTPUT;
				break;
			case 7:
				list = data->LIB;
				break;
			}

			//トークン列の切り分け
			p = strtok(arg," ");
			if(p == NULL)
				continue;
				
			do{
				char* tmp = malloc(strlen(p) + 1);
				strcpy(tmp,p);

				LINEAR_LIST_PUSH(list,tmp);
			}while ((p = strtok(NULL," ")) != NULL);
			
		}
	}
	
	fclose(fd);

	NAME_SPACE* itr;
	NAME_SPACE* target = NULL;
	
	LINEAR_LIST_FOREACH_R(nameSpaceArray,itr){
		if(targetName != NULL && strcmp(itr->NAME,targetName) == 0){
			target = itr;
			break;
		}else if(targetName == NULL && (itr->NAME == NULL || strcmp(itr->NAME,"default") == 0)){
			target = itr;
			break;
		}
	}

	if(target == NULL){
		fprintf(stderr,"build rule is not found.\n");
		return;
	}

	if((fd = fopen("Makefile","w")) != NULL){
		printf("C\n");
		char* outFile 			= *LINEAR_LIST_NEXT(target->OUTPUT);
		char* compiler 			= *LINEAR_LIST_NEXT(target->CC);
		char* objectDirectory 	= *LINEAR_LIST_NEXT(target->OBJ_DIR);
		
		#ifdef WIN32
		fprintf(fd,"RM = cmd.exe /C del\n\n",outFile);
		#endif

		//generate make build
		fprintf(fd,"#build executable file\nbuild:%s\n\n",outFile);
		fprintf(fd,"#make objs\nobj:\n\t@mkdir obj\n\n");

		//create file list
		char*** files = LINEAR_LIST_CREATE(char**);
		
		//add files
		char** itr;
		LINEAR_LIST_FOREACH(target->SOURCE_DIR,itr){
			DIR* srcD = opendir(*itr);
			if(srcD == NULL){
				perror("open source dir");
				exit(1);
			}

			struct dirent *dp;
			while ((dp = readdir(srcD)) != NULL) {//src
				char* p = strrchr(dp->d_name,'.');
				if(p != NULL && strcmp(p,".c") == 0){
					char** tmp = malloc(sizeof(char*)*2);
					tmp[0] = malloc(strlen(*itr) + strlen(dp->d_name) + 2);
					tmp[1] = malloc(strlen(objectDirectory) + strlen(dp->d_name) + 2);
					
					memset(tmp[0],0,strlen(*itr) + strlen(dp->d_name) + 2);
					memset(tmp[1],0,strlen(objectDirectory) + strlen(dp->d_name) + 2);
					
					strcpy(tmp[0],*itr);
					strcpy(tmp[1],objectDirectory);

					if((*itr)[strlen(*itr)-1] != S)
						tmp[0][strlen(*itr)] = S;

					if(objectDirectory[strlen(objectDirectory)-1] != S)
						tmp[1][strlen(objectDirectory)] = S;

					strcat(tmp[0],dp->d_name);
					strcat(tmp[1],dp->d_name);

					p = strrchr(tmp[1],'.');
					p[1] = 'o';
					p[2] = '\0';

					LINEAR_LIST_PUSH(files,tmp);
				}
			}

			closedir(srcD);
		}
		
		char*** iter;
		//object file
		LINEAR_LIST_FOREACH(files,iter){
			fprintf(fd,"%s: %s\n",(*iter)[1],(*iter)[0]);
			

			fprintf(fd,"\t%s",compiler);
			
			char** libs;
			LINEAR_LIST_FOREACH(target->LIB,libs){
				fprintf(fd," %s",*libs);
			}			
			
			fprintf(fd," -o %s %s",(*iter)[1],(*iter)[0]);

			char** includeFiles;
			LINEAR_LIST_FOREACH(target->INCLUDE,includeFiles){
				fprintf(fd," -I %s",*includeFiles);
			}			

			char** objFlags;
			LINEAR_LIST_FOREACH(target->OBJ_FLAG,objFlags){
				fprintf(fd," %s",*objFlags);
			}
			
			fprintf(fd," -c\n\n");
		}
		
		//generate executable
		fprintf(fd,"%s: obj",outFile);
		LINEAR_LIST_FOREACH(files,iter){
			fprintf(fd," %s",(*iter)[1]);
		}

		fprintf(fd,"\n\t%s",compiler);
			
		char** libs;
		LINEAR_LIST_FOREACH(target->LIB,libs){
			fprintf(fd," %s",*libs);
		}			
			
		fprintf(fd," -o %s",outFile);
		
		LINEAR_LIST_FOREACH(files,iter){
			fprintf(fd," %s",(*iter)[1]);
		}

		char** cFlags;
		LINEAR_LIST_FOREACH(target->C_FLAG,cFlags){
			fprintf(fd," %s",*cFlags);
		}

		//generate make all
		fprintf(fd,"\n\nall: clean %s",outFile);


		//generate make clean
		fprintf(fd,"\n\nclean:\n\t$(RM) %s",outFile);
		LINEAR_LIST_FOREACH(files,iter){
			fprintf(fd," %s",(*iter)[1]);
		}

		fclose(fd);
	}else{
		perror("open Makefile");
		return;
	}
}