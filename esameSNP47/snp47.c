#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void printargs(char* args);
void readargs1(char** args_str);
void readargs2(char** args_str);
const char* ARGS_PATH = "/proc/self/cmdline";
int main(){
	int fd;
	if ((fd = open("/proc/self/cmdline",  O_RDONLY)<0))
		exit(-1);
	char* str=malloc(512*sizeof(char));
	char* args = str;
	readargs2(&args);
	printargs(args);
	free(str);
}

void printargs(char* args){
	int i=0;
	while (strlen(args)>0){
		printf("argv[%d] = %s\n",i,args);	
		args=args+strlen(args)+1;
		i++;
	}
}

//lettura da file con funzioni di stdio.h
void readargs1(char** args_str){
	FILE* file = fopen(ARGS_PATH, "r");
	fgets(*args_str, 512, file);
}

//lettura dal file con system call
void readargs2(char** args_str){
	int fd;
	if((fd = open(ARGS_PATH, O_RDONLY))<0)
		exit(-1);
	if ((read(fd, *args_str, 512))<0)
		exit(-2);
}

