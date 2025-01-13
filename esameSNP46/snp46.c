//Francescomaria Stapane 2024/12/8

#include "../include/utils.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
char three_chars[3];
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
int fd;
long size_of_file;
long characters_read;
void exit_err(int code, char* text){
	perror(text);
	exit(code);
}
int is_alphanum(char c){
	return ((c > 47 && c < 58) ||  (c > 64 && c < 91) || (c > 96 && c < 123));
}
static void * threadFunc(void *arg)
{
    	int code = *((int *) arg);
	int s;
	//printf("ftell: %ld",ftell(fdopen(fd,"r")));
    	//while(characters_read < file_size(fd)){
	for(;;){		
		s = pthread_mutex_lock(&mtx);
		if (s != 0)
			exit_err(s, "error with mutex lock\n");
		char ch;
		s = read(fd, &ch, 1);
		//se ho effettivamente letto un nuovo byte
		if ( s > 0){
			characters_read++;
			lseek(fd, -1, SEEK_CUR);
			write(fd, "\000",1);
			//printf("thread %d read this character: %c\n", code, ch);
			if (characters_read % 10000 == 0){
			printf("%d KB have been read so far \n", (int)characters_read/1000);
			}
			three_chars[0]=three_chars[1];
			three_chars[1]=three_chars[2];
			three_chars[2]=ch;
			if (three_chars[0] == three_chars[1] && three_chars[1] == three_chars [2] && is_alphanum(ch)){
				printf("Thread %d found match in position %ld, character is %c\n", code, characters_read, ch);
			}

		}
		if (s < 0)
			exit_err(s, "error reading file\n");
		s = pthread_mutex_unlock(&mtx);
		if (s != 0)
			exit_err(s, "error with mutex unlock\n");

	}
        
    	return NULL;
}

int main(){
	characters_read=0;
	file_copy("dump.txt", "copy.txt");
	//fd=open("copy.txt",O_RDWR);	
	fd=open("dump.txt",O_RDWR);		
	size_of_file = file_size(fd);
	if (fd < 0)
		exit_err(fd, "error opening file\n");
	pthread_t t1, t2;
	int s;
	int th1=1, th2=2;
	s = pthread_create(&t1, NULL, threadFunc, &th1);
    	if (s != 0)
		exit_err(s, "thred creation error\n");
        	
    	s = pthread_create(&t2, NULL, threadFunc, &th2);
    	if (s != 0)
		exit_err(s, "thred creation error\n");
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	
}


/*
sources:

kerrisk/threads/thread_incr_mutex.c
*/
