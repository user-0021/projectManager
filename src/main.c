#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <linear_list.h>

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

int strsCmpOR(char const* str1,int argc,...);
char* strGetMatchPattern(char const* str1,int argc,...);

const char settingName[8][12] = {  {"CC"},
									{"CFLAG"},
									{"OBJ_FLAG"},
									{"SOURCE_DIR"},
									{"INCLUDE_DIR"},
									{"OBJ_DIR"},
									{"OUTPUT"},
									{"LIB"}
								};

int main(int argc,char** argv){

	//show version
	if(argc == 1 || strsCmpOR(argv[1],4,"version","v","-v","/v") == 0 ){
		printf("%s\n",VERSION);
		exit(0);
	}

	//project init
	if(strsCmpOR(argv[1],4,"init","i","-i","/i") == 0){
		char* projectPath;


		if(argv[2] != NULL){//change dir
			if(chdir(argv[2]) != 0){
				fprintf(stderr,"%s is invalid path.\n",argv[2]);
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
				
				if(strsCmpOR(input,2,"y","Y") == 0){
					break;
				}else if(strsCmpOR(input,2,"n","N") == 0){
					exit(0);
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
			fprintf(fd,"#Project info\nCC = gcc\nCFLAG =\nOBJ_FLAG =\nLIBRARY=\nSOURCE_DIR = src\n"
			"INCLUDE_DIR = include\nOBJ_DIR = obj\nLIB =\n\n\n"
			"#Output\nOUTPUT = out.exe\n");
			#else
			//mkdir
			mkdir("src",0755);
			mkdir("include",0755);
			mkdir("obj",0755);
			
			//make file
			fprintf(fd,"#Project info\nCC = gcc\nCFLAG =\nOBJ_FLAG =\nLIBRARY=\nSOURCE_DIR = src\n"
			"INCLUDE_DIR = include\nOBJ_DIR = obj\nLIB =\n\n\n"
			"#Output\nOUTPUT = out\n");
			#endif			
			
			fclose(fd);
		}

		exit(0);
	}

	//build project
	if(strsCmpOR(argv[1],4,"build","b","-b","/b") == 0){
		FILE* fd = fopen(".projectManagerConfig","r");
		if(fd == NULL){
			fprintf(stderr,".projectManagerConfig not found.\n");
		}

		//各種環境変数 
		char** settingData[sizeof(settingName) / sizeof(settingName[0])] = {};//三重ポインタしてしまった
		int i;
		for(i = 0;i < (sizeof(settingName) / sizeof(settingName[0]));i++){
			settingData[i] = LINEAR_LIST_CREATE(char*);
		}

		//めんどいので最大文字数1024文字でやる
		char line[1024];

		while(fgets(line,sizeof(line),fd) != NULL){
			char* p = strtok(line," \t\r\n=");

			if(p == NULL)
				continue;//エラー回避

			p = strGetMatchPattern(p,sizeof(settingName) / sizeof(settingName[0]),
					settingName[0],settingName[1],settingName[2],settingName[3],
					settingName[4],settingName[5],settingName[6],settingName[7]);

			if(p != NULL){
				char** list = settingData[(p-settingName[0])/sizeof(settingName[0])];

				//トークン列の切り分け
				while((p = strtok(NULL," \t\r\n=")) != NULL){
					char* tmp = malloc(strlen(p) + 1);
					strcpy(tmp,p);

					LINEAR_LIST_PUSH(list,tmp);
				}
			}
		}
		
		fclose(fd);

		if((fd = fopen("Makefile","w")) != NULL){
			char* outFile = *LINEAR_LIST_NEXT(settingData[6]);
			char* compiler = *LINEAR_LIST_NEXT(settingData[0]);
			char* objectDirectory = *LINEAR_LIST_NEXT(settingData[5]);
			
			#ifdef WIN32
			fprintf(fd,"RM = cmd.exe /C del\n\n",outFile);
			#endif

			//generate make build
			fprintf(fd,"#build executable file\nbuild:%s\n\n",outFile);
			fprintf(fd,"#make objs\nobj\n\t@mkdir obj\n\n");

			//create file list
			char*** files = LINEAR_LIST_CREATE(char**);
			
			//add files
			char** itr;
			LINEAR_LIST_FOREACH(settingData[3],itr){
				DIR* srcD = opendir(*itr);
				if(srcD == NULL){
					perror("open object dir");
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
				LINEAR_LIST_FOREACH(settingData[7],libs){
					fprintf(fd," %s",*libs);
				}			
				
				fprintf(fd," -o %s %s",(*iter)[1],(*iter)[0]);

				char** includeFiles;
				LINEAR_LIST_FOREACH(settingData[4],includeFiles){
					fprintf(fd," -I %s",*includeFiles);
				}			

				char** objFlags;
				LINEAR_LIST_FOREACH(settingData[2],objFlags){
					fprintf(fd," %s",*objFlags);
				}
				
				fprintf(fd," -c\n\n");
			}
			
			//generate executable
			fprintf(fd,"%s:",outFile);
			LINEAR_LIST_FOREACH(files,iter){
				fprintf(fd," %s",(*iter)[1]);
			}
	
			fprintf(fd,"\n\t%s",compiler);
				
			char** libs;
			LINEAR_LIST_FOREACH(settingData[7],libs){
				fprintf(fd," %s",*libs);
			}			
				
			fprintf(fd," -o %s",outFile);
			
			LINEAR_LIST_FOREACH(files,iter){
				fprintf(fd," %s",(*iter)[1]);
			}
	
			char** cFlags;
			LINEAR_LIST_FOREACH(settingData[1],cFlags){
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
			exit(1);
		}

		exit(0);
	}


}

int strsCmpOR(char const* str1,int argc,...){
    va_list args;
    va_start(args, argc);

	int i;
	for(i = 0;i < argc;i++){
		if(strcmp(str1,va_arg(args,char*)) == 0){
			va_end(args);
			return 0;
		}
	}

    va_end(args);

	return 1;
}

char* strGetMatchPattern(char const* str1,int argc,...){
	va_list args;
    va_start(args, argc);

	int i;
	for(i = 0;i < argc;i++){
		char* tmp = va_arg(args,char*);
		if(strcmp(str1,tmp) == 0){
			va_end(args);
			return tmp;
		}
	}

    va_end(args);

	return NULL;
}
