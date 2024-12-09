//Francescomaria Stapane 2024/12/8

#include <error.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include "../include/utils.h"
void file_copy_(char* origin_path, char* destination_path);
long file_size_(int fd);


int main(){
	file_copy("dump.txt", "copy.txt");
}




long file_size_(int fd){
	FILE* fp = fdopen(fd, "r");
	if (fseek(fp, 0, SEEK_END)<0)
		exit(-1);
	long size = ftell(fp);
	//fclose(fp);
	return(size);
}

void file_copy_(char* origin_path, char* destination_path){
	int origin_fd;
	int destination_fd;
	if ((origin_fd=open(origin_path,O_RDWR))<0){
		perror("error opening origin file");
		exit(-1);
	}
	if ((destination_fd=open(destination_path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR))<0){
		perror("error opening destination file");
		exit(-2);
	}
	long size = file_size_(origin_fd);
	char* copy = malloc(size);
	printf("size: %ld\n",size);
	lseek(origin_fd, 0, SEEK_SET);
	if(read(origin_fd, copy, size)<0){
		perror("error reading origin file");
		exit(-3);
	}
	if(write(destination_fd, copy, size) != size){
		perror("error writing on destination file");
		exit(-4);
	}
	free(copy);
	close(origin_fd);
	close(destination_fd);
}
