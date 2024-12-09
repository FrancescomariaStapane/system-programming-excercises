//Francescomaria Stapane 2024/12/5
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int global_var=1;
void printargs(char* args);
void readargs_with_stdio(char** args_str);
void readargs_with_io_syscalls(char** args_str);
void readargs_from_mem(char** args_str, int len);
const char* ARGS_PATH = "/proc/self/cmdline";
int main(int argc, char** argv){
	int fd;
	if ((fd = open("/proc/self/cmdline",  O_RDONLY)<0))
		exit(-1);
	char* str=malloc(512*sizeof(char));
	char* args = str;
	int len=0;
	for(int i=0;i<argc;i++){
		len+=(strlen(argv[i])+1);
	}
	readargs_from_mem(&args,len);
	//readargs_with_stdio(&args);
	//readargs_with_io_syscalls(&args);
	printargs(args);
	free(str);
}

void printargs(char* args){
	int i=0;
	// Il controllo sull'indirizzo della env var SHELL serve per non
	// stampare anche tutte le variabili d'ambiente dopo gli argomenti
	// nel caso in cui si usi il metodo che legge gli argoenti dalla
	// memoria del processo. (Il -6 serve per tenere conto di SHELL=).
	while (strlen(args)>0 && args != getenv("SHELL") - 6){
		printf("argv[%d] = %s\n",i,args);	
		args=args+strlen(args)+1;
		i++;
	}
}

//lettura da file con funzioni di stdio.h
void readargs_with_stdio(char** args_str){
	FILE* file = fopen(ARGS_PATH, "r");
	fgets(*args_str, 512, file);
}

//lettura dal file con system call
void readargs_with_io_syscalls(char** args_str){
	int fd;
	if((fd = open(ARGS_PATH, O_RDONLY))<0)
		exit(-1);
	if ((read(fd, *args_str, 512))<0)
		exit(-2);
}
//lettura degli argomenti spostandosi indietro nella memoria 
//rispetto alla prima variaile d'ambiente
void readargs_from_mem(char** args_str, int len){
	char* first_env_var=getenv("SHELL");
	*args_str=first_env_var-(len+6);
}
