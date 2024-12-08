//Francescomaria Stapane, 2024/11/26

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

int main(){
	char buf[256];
	fgets(buf, 256, stdin);
	int size= readlink("/proc/self/fd/0",buf, 256);
	if(size<0)
		exit(-1);
	printf("path of stdin: %s\n",buf);
}
