#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>

#ifdef WIN32
#include <direct.h>
#define S "\\" 
#else
#include <sys/stat.h>
#include <unistd.h>
#define S "/"
#endif

#define VERSION "v0.0.0"

int strsCmpOR(char const* str1,int argc,...);
void parseToken(char* buffer,char* head);

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
		projectPath = getcwd(NULL,0);
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
			fprintf(fd,"#Project info\nCC = gcc\nCFLAG =\nOBJ_FLAG =\nSOURCE_DIR = src\n"
			"INCLUDE_DIR = include\nOBJ_DIR = obj\n\n\n"
			"#Output\nOUTPUT = out.exe\n");
			#else
			//mkdir
			mkdir("src",0777);
			mkdir("include",0777);
			mkdir("obj",0777);
			
			//make file
			fprintf(fd,"#Project info\nCC = gcc\nCFLAG =\nOBJ_FLAG =\nSOURCE_DIR = src\n"
			"INCLUDE_DIR = include\nOBJ_DIR = obj\n\n\n"
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
		char cC[512] = {};
		char cFlag[512] = {};
		char objFlag[512] = {};
		char srcDir[512] = {};
		char incDir[512] = {};
		char objDir[512] = {};
		char outFile[512] = {};

		//めんどいので最大文字数1024文字でやる
		char line[1024];

		while(fgets(line,sizeof(line),fd) != NULL){
			char* p = strtok(line," \t\r\n=");
			if(p == NULL || *p == '#'){
				continue;
			}

			if(strcmp(p,"CC") == 0){
				parseToken(cC,"");	
			}else if(strcmp(p,"CFLAG") == 0){
				parseToken(cFlag,"");	
			}else if(strcmp(p,"OBJ_FLAG") == 0){
				parseToken(objFlag,"");
			}else if(strcmp(p,"SOURCE_DIR") == 0){
				parseToken(srcDir,"");
			}else if(strcmp(p,"INCLUDE_DIR") == 0){
				parseToken(incDir,"-I");
			}else if(strcmp(p,"OBJ_DIR") == 0){
				parseToken(objDir,"");
			}else if(strcmp(p,"OUTPUT") == 0){
				parseToken(outFile,"");
			}else{
				fprintf(stderr,"%s is unknown token.\n",p);
			}
		}
		
		fclose(fd);

		if((fd = fopen("Makefile","w")) != NULL){

			#ifdef WIN32
			fprintf(fd,"RM = cmd.exe /C del\n\n");
			#else
			fprintf(fd,"RM = rm\n\n");
			#endif

			//generate make build
			fprintf(fd,"#build executable file\nbuild:%s\n\n",outFile);
			fprintf(fd,"#make objs\n");

			DIR* srcD = opendir(srcDir);
			if(srcD == NULL){
				perror("open src dir");
				exit(1);
			}

    		struct dirent *dp;
			//object file
			while ((dp = readdir(srcD)) != NULL) {
				char* p = strrchr(dp->d_name,'.');
				if(p != NULL && strcmp(p,".c") == 0){
					*p = '\0';
					fprintf(fd,"%s"S"%s.o: %s"S"%s.c\n",objDir,dp->d_name,srcDir,dp->d_name);
					
					fprintf(fd,"\t%s -o %s"S"%s.o %s"S"%s.c %s %s -c\n\n",cC,objDir,dp->d_name,srcDir,dp->d_name,objFlag,incDir);
				}
			}

			rewinddir(srcD);//reset

			fprintf(fd,"%s: ",outFile);
			//generate executable
			while ((dp = readdir(srcD)) != NULL) {
				char* p = strrchr(dp->d_name,'.');
				if(p != NULL && strcmp(p,".c") == 0){
					*p = '\0';
					fprintf(fd,"%s"S"%s.o ",objDir,dp->d_name);
				}
			}
			fprintf(fd,"\n\t%s -o %s",cC,outFile);
			rewinddir(srcD);
			while ((dp = readdir(srcD)) != NULL) {
				char* p = strrchr(dp->d_name,'.');
				if(p != NULL && strcmp(p,".c") == 0){
					*p = '\0';
					fprintf(fd," %s"S"%s.o",objDir,dp->d_name);
				}
			}
			fprintf(fd," %s",cFlag);

			//generate make all
			fprintf(fd,"\n\nall: clean %s",outFile);

			//generate make clean
			fprintf(fd,"\n\nclean:\n\t$(RM) %s",outFile);
			rewinddir(srcD);
			while ((dp = readdir(srcD)) != NULL) {
				char* p = strrchr(dp->d_name,'.');
				if(p != NULL && strcmp(p,".c") == 0){
					*p = '\0';
					fprintf(fd," %s"S"%s.o",objDir,dp->d_name);
				}
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

void parseToken(char* buffer,char* head){
	char *p;
	if((p = strtok(NULL," \t\r=")) != NULL){
		char* pos;

		while((pos = strchr(p,'\n')) == NULL){
			if(strlen(buffer) != 0)
				strcat(buffer," ");
			
			if(strlen(p) != 0){
				strcat(buffer,head);
				strcat(buffer,p);
			}

			p = strtok(NULL," \t\r=");
		} 
					
		if(pos != p){	
			*pos = '\0';

			if(strlen(buffer) != 0)
				strcat(buffer," ");
	
			if(strlen(p) != 0){
				strcat(buffer,head);
				strcat(buffer,p);
			}
		}
	}		
}

