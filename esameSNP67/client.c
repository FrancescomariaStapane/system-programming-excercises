#include "../include/utils.h"
#include <errno.h>
#include <pthread.h>
#include "../include/apue.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 9000
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
int sockfd;

static void * thread_f_env_modifier(void *arg)
{
    	int code = *((int *) arg);
	int s;	
	for(;;){
		int interval = rand()%3+1;
		printf("waiting %d seconds\n", interval);
		sleep(interval);	
		int int_value = rand()%1000;
		char str_value[10];
		snprintf(str_value, 10, "%d", int_value);
		s = pthread_mutex_lock(&mtx);
		if (s != 0)
			err_sys("error with mutex lock");	
		s = setenv("A",str_value,1);
		printf("A=%s\n", getenv("A"));
		s = pthread_mutex_unlock(&mtx);
		if (s != 0)
			err_sys("error with mutex unlock"); 
	}
	return NULL;
}

static void * thread_tcp_client(void *arg){
	char a[256];
	//struct timespec* sleep_time;
	//sleep_time->tv_sec=0;
	//sleep_time->tv_nsec=100000;
	for(;;){
		//nanosleep(sleep_time, NULL);
		char* new_a=getenv("A");
		if (new_a==NULL || strcmp(a, new_a)== 0)
			continue;
		strcpy(a,new_a);
		int n = send(sockfd, a, strlen(a), 0);
		if (n == -1)
		{
			err_sys("coulnd't send env value message to server");
		}			
	}
}

static void handler(int signo){
	//manda notifica al server tcp
	char* msg="close";	
	int n = send(sockfd, msg, strlen(msg), 0);
	if (n == -1)
	{
		err_sys("coulnd't send close message to server");
	}
	printf("\nClosing client and server...\n");
	exit(0);
}

int connect_to_server(){
	int res = 0; //valore di ritorno delle Sockets API

	sockfd = 0; //connection socket: servirà per la comunicazione col server

	struct sockaddr_in server; //IPv4 server address, senza hostname resolution
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo

	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4

	/*
	Specifico la porta del server da contattare, verso cui avviare il 3WHS
	*/
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


	//connessione al server
	/* open socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket error: ");
		return -1;
	}

	//avvio il 3WHS
	res = connect(sockfd, (struct sockaddr *)&server, len);
	if (res != 0)
	{
		perror("connect() error: ");
		close(sockfd);
		return -1;
	}
	return 0;
}
int main(){
	int s;
	s= connect_to_server();
	if (s<0)
		err_sys("error connecting to server\n");
	for (int i=1; i <= NSIG; i++){
		signal(i, handler);
	}
	srand(10);
	pthread_t t1, t2, t3;

	int th1=1, th2=2, th3=3;
	s = pthread_create(&t1, NULL, thread_tcp_client, &th1);
    	if (s != 0)
		err_sys("thred creation error");
        	
    	s = pthread_create(&t2, NULL, thread_f_env_modifier, &th2);
    	if (s != 0)
		err_sys("thred creation error");
	s = pthread_create(&t3, NULL, thread_f_env_modifier, &th3);
    	if (s != 0)
		err_sys("thred creation error");
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	
}


