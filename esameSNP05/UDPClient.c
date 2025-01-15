#include "Header.h"
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include "../include/apue.h"
#include <semaphore.h>
#include <fcntl.h>
#define PORT 9000
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static void * threadFunc(void *arg)
{
    	int code = *((int *) arg);
	if (!(code == 0 || code == 1)){
		err_sys("illegal thread code, must be 0 or 1");
	}
	int res = 0; //valore di ritorno delle APIs

	/*
	socket: servirà per la comunicazione col server
	*/
	int sockfd = 0;
	
	/*
	open socket di tipo datagram
	*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		err_sys("socket error (thread %d): ",code );
	}
	
	/*
	indirizzo IPv4 del server, senza hostname resolution
	*/
	struct sockaddr_in server;
	socklen_t len = sizeof(server);
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address Family IPv4
	
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		
	/*
	Specifico la well-known port
	*/
	
	server.sin_port = htons(PORT + code);
	
	//setto l'indirizzo well-known del socket
	
		
	ssize_t n = 0;
	char buffer[BUFSIZE];
	
	char address[INET_ADDRSTRLEN] = "";
	
	int quit = 0;
	
	while (!quit)
	{
		sem_t* semaphore = sem_open("snp05semaphore",0);
		n = sem_trywait(semaphore);
		if (n<0 && errno == EAGAIN)
			continue;
		printf("Sending message... (thread %d):\n ",code);
		strcpy(buffer,"ciao");
		n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server, len);
		if (n == -1)
		{
			err_sys("sendto() error (thread %d): ",code);
			continue;
// 			close(sockfd);
// 			return FAILURE;
		}
		n = recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&server, &len);
		if (n == -1)
		{
			err_sys("recvfrom() error (thread %d): ",code);
			continue;
// 			close(sockfd);
// 			return FAILURE;
		}
	
		buffer[n] = '\0';
		printf("\tRicevuta risposta:\n\t'%s'\n\tda: %s:%d\n(thread %d)\n", buffer, \
		inet_ntop(AF_INET, &(server.sin_addr), address, INET_ADDRSTRLEN), \
		ntohs(server.sin_port),code );
		sem_post(semaphore);

	}//wend
		
	//qui non ci arrivo mai...	
	close(sockfd);

        
    	return NULL;
}



int main(){
	sem_open("snp05semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1 );
	pthread_t t1, t0;
	int th1=1, th0=0;
	int s;
	s = pthread_create(&t1, NULL, threadFunc, &th1);
    	if (s != 0)
		err_sys("thred &d creation error\n", 1);
        	
    	s = pthread_create(&t0, NULL, threadFunc, &th0);
    	if (s != 0)
		err_sys("thred %d creation error\n", 0);
	pthread_join(t1, NULL);
	pthread_join(t0, NULL);
		
		
	return 0;
}


