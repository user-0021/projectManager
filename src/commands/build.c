#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linear_list/linear_list.h>
#include <stringLib/stringLib.h>
#include <commands.h>
#include <tokenizer.h>

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

const char* settingName[8] = { 
	"CC",
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
} TARGET;
					
typedef struct{
	char* NAME;
	char** TARGET;
} PROJECT;

typedef struct
{
	char* NAME;
	char* DEPEND;
	char* BODY;
} MAKE_ENTRY;


TOKEN* read_file_token(const char* const fileName);
PROJECT* parse_project(TOKEN* project_token);
TARGET* parse_target(TOKEN* target_token);
PROJECT* generateProject(char* name);
TARGET* generateTarget(char* name);
int getSettingNameId(char* name);
int get_obj_entry(MAKE_ENTRY* list,TARGET* target);
int get_exe_entry(MAKE_ENTRY* list,TARGET* target,MAKE_ENTRY* obj_entry,int counter);


int build(char* projectName){
	//tokenize
	TOKEN* configToken = read_file_token(".projectManagerConfig");
	TOKEN* targetsToken = read_file_token(".projectManagerTargets");

	if(configToken == NULL || targetsToken == NULL)
		return -1;

	//各種環境変数 
	PROJECT* projectArray = parse_project(configToken);
	TARGET* targetArray = parse_target(targetsToken);
	if(targetArray == NULL || projectArray == NULL)
		return -1;

	PROJECT* itr;
	PROJECT* cur = NULL;
	
	LINEAR_LIST_FOREACH_R(projectArray,itr){
		if(projectName != NULL && itr->NAME != NULL && strcmp(itr->NAME,projectName) == 0){
			cur = itr;
			break;
		}else if(projectName == NULL && (itr->NAME == NULL || strcmp(itr->NAME,"default") == 0)){
			cur = itr;
			break;
		}

	}
	
	if(cur == NULL || cur->NAME == NULL){
		fprintf(stderr,"build rule is not found.\n");
		return -1;
	}
	else{
		//check target
		char** i;
		LINEAR_LIST_FOREACH_R(cur->TARGET,i){
			TARGET* iter;
			int f = 0;
			LINEAR_LIST_FOREACH(targetArray,iter){
				if(iter->NAME != NULL && strcmp(iter->NAME,*i) == 0)
					f = 1;
			}

			if(!f){
				fprintf(stderr,"%s is not found.\n",*i);
				return -1;
			}
		}
		
	}

	FILE* fd;
	if((fd = fopen("Makefile","w")) != NULL){

		MAKE_ENTRY* makeObj = LINEAR_LIST_CREATE(MAKE_ENTRY);
		MAKE_ENTRY* makeExe = LINEAR_LIST_CREATE(MAKE_ENTRY);

		int counter = 0;
		char** targetName;
		LINEAR_LIST_FOREACH(cur->TARGET,targetName){
			TARGET* iter;
			LINEAR_LIST_FOREACH(targetArray,iter){
				if(iter->NAME != NULL && strcmp(iter->NAME,*targetName) == 0){
					get_obj_entry(makeObj,iter);
					counter = get_exe_entry(makeExe,iter,makeObj,counter);
				}
			}
		}

		
		#ifdef WIN32
		fprintf(fd,"RM = cmd.exe /C del\n\n",outFile);
		#endif

		//generate make build
		fprintf(fd,"#build executable file\nbuild:");
		MAKE_ENTRY* iter;
		LINEAR_LIST_FOREACH(makeExe,iter){
			fprintf(fd,"%s ",iter->NAME);
		}
		fprintf(fd,"\n\n#make objs\nobj:\n\t@mkdir obj\n\n");

		//obj
		LINEAR_LIST_FOREACH(makeObj,iter){
			fprintf(fd,"%s:%s\n%s",iter->NAME,iter->DEPEND,iter->BODY);
		}

		//exe
		LINEAR_LIST_FOREACH(makeExe,iter){
			fprintf(fd,"%s:%s\n%s",iter->NAME,iter->DEPEND,iter->BODY);
		}		

		//generate make all
		fprintf(fd,"all: clean ");
		LINEAR_LIST_FOREACH(makeExe,iter){
			fprintf(fd,"%s ",iter->NAME);
		}
		LINEAR_LIST_FOREACH(makeObj,iter){
			fprintf(fd,"%s ",iter->NAME);
		}


		//generate make clean
		fprintf(fd,"\n\nclean:\n\t$(RM) obj/*.o ");
		LINEAR_LIST_FOREACH(makeExe,iter){
			fprintf(fd,"%s ",iter->NAME);
		}

		fclose(fd);
	}else{
		perror("open Makefile");
		return -1;
	}

	return 0;
}


TOKEN* read_file_token(const char* const fileName){
	
	struct stat stbuf;

	//open
	int fd = open(fileName,O_RDONLY);
	if(fd < 0){
		fprintf(stderr,"Filed open %s.\n",fileName);
		return NULL;
	}

	//get file status
	if (fstat(fd, &stbuf) == -1) {
		fprintf(stderr,"Filed get file status.\n");
		return NULL;
	}

	//get config buffer
	char* buffer = (char*)malloc(stbuf.st_size);
	read(fd,buffer,stbuf.st_size);
	return tokenize(buffer);
}


PROJECT* parse_project(TOKEN* project_token){
	//generate head
	PROJECT* profectArray = LINEAR_LIST_CREATE(PROJECT);

	//add null name
	//今のところは意味ないけど、いつかターゲットを指定されなかった場合に読み込まれるデフォルト設定にする
	PROJECT* nullProject = generateProject(NULL);
	LINEAR_LIST_PUSH(profectArray,*nullProject);
	free(nullProject);

	//load targets
	project_token  = project_token->next;
	while(project_token->type != TOKEN_EOF){
		
		switch (project_token->type)
		{
		case TOKEN_RESURVED:{
			if(*project_token->str == '['){
				//check next token
				project_token = project_token->next;
				if(project_token->type != TOKEN_STR){
					//error
					fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*project_token->str);
					return NULL;
				}
				//generate
				PROJECT* addData = generateProject(project_token->str);

				//check next token
				project_token = project_token->next;
				if(project_token->type != TOKEN_RESURVED || *project_token->str != ']'){
					//error
					fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*project_token->str);
					return NULL;
				}

				//push list
				LINEAR_LIST_PUSH(profectArray,*addData);
				free(addData);
			}else{
				fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*project_token->str);
				return NULL;
			}

			break;
		}
		case TOKEN_STR:{
			//num
			char* str = project_token->str;

			//check next token
			project_token = project_token->next;
			if(project_token->type != TOKEN_RESURVED || *project_token->str != '='){
				//error
				fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*project_token->str);
				return NULL;
			}

			project_token = project_token->next;

			if(project_token->type == TOKEN_STR){
				PROJECT* data = LINEAR_LIST_LAST(profectArray);
				
				if(strcmp(str,"TARGET") != 0){
					//error
					fprintf(stdout,"予期しないトークン \'%s\'周辺に構文エラーがあります\n",project_token->str);
					return NULL;
				}

				//トークン列の切り分け
				if(project_token->type == TOKEN_PUN)
					continue;
				
				do{
					LINEAR_LIST_PUSH(data->TARGET,project_token->str);
					project_token = project_token->next;
				}while (project_token->type == TOKEN_STR);

				
				if(project_token->type != TOKEN_PUN && project_token->type != TOKEN_EOF){
					//error
					fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*project_token->str);
					return NULL;
				}

				
			}
			break;
		}	
		default:
			break;
		}

		project_token = project_token->next;
	}

	return profectArray;
}


TARGET* parse_target(TOKEN* target_token){
	//generate head
	TARGET* targetArray = LINEAR_LIST_CREATE(TARGET);

	//add null name
	//今のところは意味ないけど、いつかターゲットを指定されなかった場合に読み込まれるデフォルト設定にする
	TARGET* nullTarget = generateTarget(NULL);
	LINEAR_LIST_PUSH(targetArray,*nullTarget);
	free(nullTarget);

	//load targets
	target_token  = target_token->next;
	while(target_token->type != TOKEN_EOF){
		
		switch (target_token->type)
		{
		case TOKEN_RESURVED:{
			if(*target_token->str == '['){
				//check next token
				target_token = target_token->next;
				if(target_token->type != TOKEN_STR){
					//error
					fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*target_token->str);
					return NULL;
				}
				//generate
				TARGET* addData = generateTarget(target_token->str);

				//check next token
				target_token = target_token->next;
				if(target_token->type != TOKEN_RESURVED || *target_token->str != ']'){
					//error
					fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*target_token->str);
					return NULL;
				}

				//push list
				LINEAR_LIST_PUSH(targetArray,*addData);
				free(addData);
			}else{
				fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*target_token->str);
				return NULL;
			}

			break;
		}
		case TOKEN_STR:{
			//num
			int res = getSettingNameId(target_token->str);

			//check next token
			target_token = target_token->next;
			if(target_token->type != TOKEN_RESURVED || *target_token->str != '='){
				//error
				fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*target_token->str);
				return NULL;
			}

			target_token = target_token->next;

			if(target_token->type == TOKEN_STR){
				TARGET* data = LINEAR_LIST_LAST(targetArray);
				char** list;

				switch (res)
				{
				case ID_CC:
					list = data->CC;
					break;
				case ID_CFLAG:
					list = data->C_FLAG;
					break;
				case ID_OBJFLAG:
					list = data->OBJ_FLAG;
					break;
				case ID_SOURCEDIR:
					list = data->SOURCE_DIR;
					break;
				case ID_INCLUDEDIR:
					list = data->INCLUDE;
					break;
				case ID_OBJDIR:
					list = data->OBJ_DIR;
					break;
				case ID_OUTPUT:
					list = data->OUTPUT;
					break;
				case ID_LIB:
					list = data->LIB;
					break;
				default:
					//error
					fprintf(stdout,"予期しないトークン \'%s\'周辺に構文エラーがあります\n",target_token->str);
					return NULL;
				}

				//トークン列の切り分け
				if(target_token->type == TOKEN_PUN)
					continue;
				
				do{
					LINEAR_LIST_PUSH(list,target_token->str);

					target_token = target_token->next;
				}while (target_token->type == TOKEN_STR);
				
				if(target_token->type != TOKEN_PUN && target_token->type != TOKEN_EOF){
					//error
					fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*target_token->str);
					return NULL;
				}
			}
			break;
		}	
		default:
			break;
		}

		target_token = target_token->next;
	}

	return targetArray;
}

PROJECT* generateProject(char* name){
	//malloc
	PROJECT* data = malloc(sizeof(PROJECT));
	memset(data,0,sizeof(PROJECT));

	data->NAME			= name;
	data->TARGET		= LINEAR_LIST_CREATE(char*);

	return data;
}

TARGET* generateTarget(char* name){

	//malloc
	TARGET* data = malloc(sizeof(TARGET));
	memset(data,0,sizeof(TARGET));

	data->NAME			= name;
	data->CC 			= LINEAR_LIST_CREATE(char*);
	data->C_FLAG		= LINEAR_LIST_CREATE(char*);
	data->OBJ_FLAG		= LINEAR_LIST_CREATE(char*);
	data->SOURCE_DIR	= LINEAR_LIST_CREATE(char*);
	data->INCLUDE		= LINEAR_LIST_CREATE(char*);
	data->OBJ_DIR		= LINEAR_LIST_CREATE(char*);
	data->OUTPUT		= LINEAR_LIST_CREATE(char*);
	data->LIB			= LINEAR_LIST_CREATE(char*);

	return data;
}

int getSettingNameId(char* name){
	int nameLimit = sizeof(settingName)/sizeof(char*);
	if(name == NULL)
		return -1;

	int i;
	for(i = 0;i < nameLimit;i++){
		if(strcmp(settingName[i],name) == 0){
			return i;
		}
	}
	
	return -1;
}

int get_obj_entry(MAKE_ENTRY* list,TARGET* target){
	//
	char* compiler = *LINEAR_LIST_LAST(target->CC);
	char* objDir = *LINEAR_LIST_LAST(target->OBJ_DIR);


	//
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
			if(dp->d_type == DT_REG && p != NULL && (strcmp(p,".c") == 0 || strcmp(p,".s") == 0 || strcmp(p,".S") == 0)){
				MAKE_ENTRY entry;
				char * inFile;
				
				//depend
				entry.NAME = malloc(strlen(target->NAME) + strlen(*itr) + strlen(dp->d_name) + strlen(objDir) + 4);
				strcpy(entry.NAME,objDir);
				strcat(entry.NAME,"/");
				strcat(entry.NAME,target->NAME);
				strcat(entry.NAME,"_");
				strcat(entry.NAME,*itr);
				strcat(entry.NAME,"/");
				strcat(entry.NAME,dp->d_name);
				inFile = entry.NAME + strlen(target->NAME) + strlen(objDir) + 2;

				//name
				entry.DEPEND = malloc(strlen(inFile)  + 1);
				strcpy(entry.DEPEND,inFile);

				//set exp
				entry.NAME[strlen(entry.NAME) - 1] = 'o';
				char* p,*head = entry.NAME + strlen(objDir) + 1;
				while (p = strchr(head,'/'))
					*p = '_';				

				

				//body
				entry.BODY = malloc(strlen(compiler) + 3);
				strcpy(entry.BODY,"\t");
				strcat(entry.BODY,compiler);
				strcat(entry.BODY," ");
				
				char** libs;
				LINEAR_LIST_FOREACH(target->LIB,libs){
					entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + strlen(*libs) + 2);
					strcat(entry.BODY,*libs);
					strcat(entry.BODY," ");
				}			
				
				// -o
				entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + strlen(entry.DEPEND) + strlen(entry.NAME) + 7);
				strcat(entry.BODY,"-o ");
				strcat(entry.BODY,entry.NAME);
				strcat(entry.BODY," ");
				strcat(entry.BODY,entry.DEPEND);
				strcat(entry.BODY," ");

				//-I
				char** include;
				LINEAR_LIST_FOREACH(target->INCLUDE,include){
					entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + strlen(*include) + 5);
					strcat(entry.BODY,"-I ");
					strcat(entry.BODY,*include);
					strcat(entry.BODY," ");
				}			

				// flag
				char** objFlags;
				LINEAR_LIST_FOREACH(target->OBJ_FLAG,objFlags){
					entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + strlen(*objFlags) + 2);
					strcat(entry.BODY,*objFlags);
					strcat(entry.BODY," ");
				}
			
				// -c
				entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + 5);
				strcat(entry.BODY,"-c\n\n");

				LINEAR_LIST_PUSH(list,entry);
			}else if(dp->d_type == DT_DIR && strlistOR(dp->d_name,2,".","..") != 0){
				char* path = malloc(strlen(dp->d_name) + strlen(*itr) + 2);
				strcpy(path,*itr);
				strcat(path,"/");
				strcat(path,dp->d_name);
				
				LINEAR_LIST_PUSH(target->SOURCE_DIR,path);
			}
		}

		closedir(srcD);
	}

	return 0;
}

int get_exe_entry(MAKE_ENTRY* list,TARGET* target,MAKE_ENTRY* obj_entry,int counter){
	char* compiler = *LINEAR_LIST_LAST(target->CC);
	
	//name
	MAKE_ENTRY entry;
	entry.NAME = *LINEAR_LIST_LAST(target->OUTPUT);

	//depend
	entry.DEPEND = malloc(5);
	strcpy(entry.DEPEND,"obj ");
	
	MAKE_ENTRY* itr;
	int i = 0;
	LINEAR_LIST_FOREACH(obj_entry,itr){
		if(!(i < counter)){
			entry.DEPEND = realloc(entry.DEPEND,strlen(entry.DEPEND) + strlen(itr->NAME) + 2);
			strcat(entry.DEPEND,itr->NAME);
			strcat(entry.DEPEND," ");
		}
		i++;
	}

	//body
	entry.BODY = malloc(strlen(compiler) + 3);
	strcpy(entry.BODY,"\t");
	strcat(entry.BODY,compiler);
	strcat(entry.BODY," ");

	//lib
	char** libs;
	LINEAR_LIST_FOREACH(target->LIB,libs){
		entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + strlen(*libs) + 2);
		strcat(entry.BODY,*libs);
		strcat(entry.BODY," ");
	}			
	
	// -o
	entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + strlen(entry.NAME) + strlen(entry.DEPEND) + 1);
	strcat(entry.BODY,"-o ");
	strcat(entry.BODY,entry.NAME);
	strcat(entry.BODY," ");
	strcat(entry.BODY,entry.DEPEND + 4);

	// flag
	char** cFlags;
	LINEAR_LIST_FOREACH(target->C_FLAG,cFlags){
		entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + strlen(*cFlags) + 2);
		strcat(entry.BODY,*cFlags);
		strcat(entry.BODY," ");
	}

	entry.BODY = realloc(entry.BODY,strlen(entry.BODY) + 3);
	strcat(entry.BODY,"\n\n");

	LINEAR_LIST_PUSH(list,entry);

	return i;
}