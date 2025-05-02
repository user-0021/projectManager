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
} TARGET;

TOKEN* read_file_token(const char* const fileName);
TARGET* parse_target(TOKEN* target_token);
TARGET* generateTarget(char* name);
int getSettingNameId(char* name);


int build(char* targetName){
	
	TOKEN* configToken = read_file_token(".projectManagerConfig");
	TOKEN* targetsToken = read_file_token(".projectManagerTargets");

	if(configToken == NULL || targetsToken == NULL)
		return -1;

	//各種環境変数 
	TARGET* targetArray = parse_target(targetsToken);
	if(targetArray == NULL)
		return -1;

	TARGET* itr;
	TARGET* target = NULL;
	
	printf("A\n");
	LINEAR_LIST_FOREACH_R(targetArray,itr){
		printf("%s\n",itr->NAME);

		if(targetName != NULL && itr->NAME != NULL && strcmp(itr->NAME,targetName) == 0){
			target = itr;
			break;
		}else if(targetName == NULL && (itr->NAME == NULL || strcmp(itr->NAME,"default") == 0)){
			target = itr;
			break;
		}

	}
	return 0;
	if(target == NULL || target->NAME == NULL){
		fprintf(stderr,"build rule is not found.\n");
		return -1;
	}

	// if((fd = fopen("Makefile","w")) != NULL){
	// 	char* outFile 			= *LINEAR_LIST_NEXT(target->OUTPUT);
	// 	char* compiler 			= *LINEAR_LIST_NEXT(target->CC);
	// 	char* objectDirectory 	= *LINEAR_LIST_NEXT(target->OBJ_DIR);
		
	// 	#ifdef WIN32
	// 	fprintf(fd,"RM = cmd.exe /C del\n\n",outFile);
	// 	#endif

	// 	//generate make build
	// 	fprintf(fd,"#build executable file\nbuild:%s\n\n",outFile);
	// 	fprintf(fd,"#make objs\nobj:\n\t@mkdir obj\n\n");

	// 	//create file list
	// 	char*** files = LINEAR_LIST_CREATE(char**);
		
	// 	//add files
	// 	char** itr;
	// 	LINEAR_LIST_FOREACH(target->SOURCE_DIR,itr){
	// 		DIR* srcD = opendir(*itr);
	// 		if(srcD == NULL){
	// 			perror("open source dir");
	// 			exit(1);
	// 		}

	// 		struct dirent *dp;
	// 		while ((dp = readdir(srcD)) != NULL) {//src
	// 			char* p = strrchr(dp->d_name,'.');
	// 			if(p != NULL && (strcmp(p,".c") == 0 || strcmp(p,".s") == 0 || strcmp(p,".S") == 0)){
	// 				char** tmp = malloc(sizeof(char*)*2);
	// 				tmp[0] = malloc(strlen(*itr) + strlen(dp->d_name) + 2);
	// 				tmp[1] = malloc(strlen(objectDirectory) + strlen(dp->d_name) + 2);
					
	// 				memset(tmp[0],0,strlen(*itr) + strlen(dp->d_name) + 2);
	// 				memset(tmp[1],0,strlen(objectDirectory) + strlen(dp->d_name) + 2);
					
	// 				strcpy(tmp[0],*itr);
	// 				strcpy(tmp[1],objectDirectory);

	// 				if((*itr)[strlen(*itr)-1] != S)
	// 					tmp[0][strlen(*itr)] = S;

	// 				if(objectDirectory[strlen(objectDirectory)-1] != S)
	// 					tmp[1][strlen(objectDirectory)] = S;

	// 				strcat(tmp[0],dp->d_name);
	// 				strcat(tmp[1],dp->d_name);

	// 				p = strrchr(tmp[1],'.');
	// 				p[1] = 'o';
	// 				p[2] = '\0';

	// 				LINEAR_LIST_PUSH(files,tmp);
	// 			}
	// 		}

	// 		closedir(srcD);
	// 	}
		
	// 	char*** iter;
	// 	//object file
	// 	LINEAR_LIST_FOREACH(files,iter){
	// 		fprintf(fd,"%s: %s\n",(*iter)[1],(*iter)[0]);
			

	// 		fprintf(fd,"\t%s",compiler);
			
	// 		char** libs;
	// 		LINEAR_LIST_FOREACH(target->LIB,libs){
	// 			fprintf(fd," %s",*libs);
	// 		}			
			
	// 		fprintf(fd," -o %s %s",(*iter)[1],(*iter)[0]);

	// 		char** includeFiles;
	// 		LINEAR_LIST_FOREACH(target->INCLUDE,includeFiles){
	// 			fprintf(fd," -I %s",*includeFiles);
	// 		}			

	// 		char** objFlags;
	// 		LINEAR_LIST_FOREACH(target->OBJ_FLAG,objFlags){
	// 			fprintf(fd," %s",*objFlags);
	// 		}
			
	// 		fprintf(fd," -c\n\n");
	// 	}
		
	// 	//generate executable
	// 	fprintf(fd,"%s: obj",outFile);
	// 	LINEAR_LIST_FOREACH(files,iter){
	// 		fprintf(fd," %s",(*iter)[1]);
	// 	}

	// 	fprintf(fd,"\n\t%s",compiler);
			
	// 	char** libs;
	// 	LINEAR_LIST_FOREACH(target->LIB,libs){
	// 		fprintf(fd," %s",*libs);
	// 	}			
			
	// 	fprintf(fd," -o %s",outFile);
		
	// 	LINEAR_LIST_FOREACH(files,iter){
	// 		fprintf(fd," %s",(*iter)[1]);
	// 	}

	// 	char** cFlags;
	// 	LINEAR_LIST_FOREACH(target->C_FLAG,cFlags){
	// 		fprintf(fd," %s",*cFlags);
	// 	}

	// 	//generate make all
	// 	fprintf(fd,"\n\nall: clean %s",outFile);


	// 	//generate make clean
	// 	fprintf(fd,"\n\nclean:\n\t$(RM) %s",outFile);
	// 	LINEAR_LIST_FOREACH(files,iter){
	// 		fprintf(fd," %s",(*iter)[1]);
	// 	}

	// 	fclose(fd);
	// }else{
	// 	perror("open Makefile");
	// 	return -1;
	// }

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

TARGET* parse_target(TOKEN* target_token){
	//generate head
	TARGET* targetArray = LINEAR_LIST_CREATE(TARGET);

	//add null name
	//今のところは意味ないけど、いつかターゲットを指定されなかった場合に読み込まれるデフォルト設定にする
	TARGET* nullTarget = generateTarget(NULL);
	LINEAR_LIST_PUSH(targetArray,*nullTarget);

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
					if(target_token->type == TOKEN_STR){
						LINEAR_LIST_PUSH(list,target_token->str);
					}else{
						//error
						fprintf(stdout,"予期しないトークン \'%c\'周辺に構文エラーがあります\n",*target_token->str);
						return NULL;
					}

					target_token = target_token->next;
				}while (target_token->type != TOKEN_PUN);
				
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