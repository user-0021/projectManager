#include <stdio.h>
#include <stdlib.h>
#include <commands.h>
#include <stringLib/stringLib.h>


#define VERSION "v0.0.0"


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

