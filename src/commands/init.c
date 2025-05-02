#include <stdio.h>
#include <stdlib.h>
#include <commands.h>
#include <stringLib/stringLib.h>

int check_current(const char* current,char* projectPath);
int check_project();

int generate_project_config();
int generate_target_list();

int init(char* current){
	char* projectPath;

	//check 
	if(check_current(current,projectPath) < 0)
		return -1;
	if(check_project() < 0)
		return 0;

	printf("Initialize project on %s\n",projectPath);

	//generate
	if(generate_project_config() < 0)
		return -1;
	if(generate_target_list() < 0)
		return -1;

	return 0;
}


int check_current(const char* current,char* projectPath){
	//change dir
	if(current != NULL){
		if(chdir(current) != 0){
			fprintf(stderr,"%s is invalid path.\n",current);
			return -1;
		}
	}

	//get current path
	projectPath = _getcwd(NULL,0);
	if(projectPath == NULL){
		fprintf(stderr,"Failed get current path.\n");
		return -1;
	}

	return 0;
}

int check_project(){
	//check project file
	FILE* fd = fopen(".projectManagerConfig","r");
	if(fd != NULL){
		fclose(fd);

		printf("This directory alrady generate project configuration file.\n");
		printf("Will be over write?(y/n)\n");

		char input[256];
		while (1)
		{
			scanf("%s",input);
			
			if(strlistOR(input,2,"y","Y") == 0){
				break;
			}else if(strlistOR(input,2,"n","N") == 0){
				return -1;
			}
		}
	}

	return 0;
}


int generate_project_config(){
	
	FILE* fd;
	if(NULL != (fd = fopen(".projectManagerConfig","w"))){
		#ifdef WIN32
		//mkdir
		_mkdir("src");
		_mkdir("include");
		
		//make file
		fprintf(fd,	
			"[default]\n"
			"TARGET = test\n"
			);
		#else
		//mkdir
		mkdir("src",0755);
		mkdir("include",0755);
		
		//make file
		fprintf(fd,	
			"[default]\n"
			"TARGET = test\n"
					);
		#endif			
		
		fclose(fd);
	}else{
		perror("generate_project_config()");
		return -1;
	}

	return 0;
}


int generate_target_list(){
	
	FILE* fd;
	if(NULL != (fd = fopen(".projectManagerTargets","w"))){
		#ifdef WIN32
		//make file
		fprintf(fd,	
			"[test]\n"
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
		//make file
		fprintf(fd,	
			"[default]\n"
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
	}else{
		perror("generate_target_list()");
		return -1;
	}

	return 0;
}

