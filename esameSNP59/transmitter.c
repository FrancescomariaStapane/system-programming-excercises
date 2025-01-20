#include "../include/apue.h"
#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#define DIM 1000000

#define PORT 9000
#define BUFSIZE 9000
int numbers[DIM];
void populate_array(){
	srand(10);
	for (int i=0; i<DIM; i++){
		numbers[i]=rand()%1000+1;
	}
}

void print_times(clock_t clk_since, struct tms* tms_since)
{	
	struct tms* tms_now=malloc(sizeof(struct tms));
	clock_t clk_now=times(tms_now);
	printf("clock time: %ld ms\n",(clk_now - clk_since)*10);
	printf("system time: %ld ms\n",(tms_now->tms_stime - tms_since->tms_stime)*10);
	printf("user time: %ld ms\n",(tms_now->tms_utime - tms_since->tms_utime)*10);
	free(tms_now);
}
int tcp_transfer(){
	struct tms* tms_start_tcp = malloc(sizeof(struct tms));
	clock_t clk_start_tcp = times(tms_start_tcp);
	int res = 0; //valore di ritorno delle Sockets API
	
	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	
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
	
	//messaggio predefinito da inviare al server
	char msg[BUFSIZE] = "";
	int counter = 0;
	ssize_t n = 0;
	time_t start_time = time(NULL);
	for (int i = 0; i < DIM; i++){
		snprintf(msg, BUFSIZE-1, "%d", numbers[i]);
		n = send(sockfd, msg, strlen(msg), 0);
		if (n == -1)
		{
			perror("send() error: ");
			close(sockfd);
			return -1;
		}
		//printf("Client queued %d bytes to server\n", (int) n);
		
		n = recv(sockfd, msg, BUFSIZE -1, 0);
		if (n == -1) {
			perror("recv() error: ");
			close(sockfd);
			return -1;
		} else if (n == 0) {
			printf("server closed connetion\n");
			break;
		}
		/*if (n > 0) {
			msg[n] = 0;
			if (strcmp(msg,"ok")!=0){
				printf("server reply: '%s'\n", msg);
			}
		}*/
		
	}//for
	
	//4-way teardown
	close(sockfd);
	printf("\n\ntcp:\n");
	print_times(clk_start_tcp, tms_start_tcp);

	return 0;

}

void sh_mem_transfer(){
	sleep(1);
	struct tms* tms_start_shmem = malloc(sizeof(struct tms));
	clock_t clk_start_shmem = times(tms_start_shmem);

	key_t shmem_key = ftok("./Makefile",1);
	int shmem_id = shmget(shmem_key, DIM*sizeof(int), 0);
	if (shmem_id == -1){
		shmem_id = shmget(shmem_key, DIM*sizeof(int),IPC_CREAT | IPC_EXCL | 0666);
	}

	key_t sem_key = ftok("./Makefile",2);//SystemV semaphore key
	int sem_id = semget(sem_key, 1, 0);//get semaphore
	
	//prepare operation
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	//sem_b.sem_op = -1; /* P() */
	//or
	sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = SEM_UNDO;
	
	int* sh_numbers = shmat(shmem_id,0,0);
	if (sh_numbers  == (void*)-1){
		err_sys("shmat error");
	}

	//put numbers in shared memory
	for (int i=0; i < DIM; i++){
		sh_numbers[i]=numbers[i];
	}
		
	//perform operation
	semop(sem_id, &sem_b, 1);
	printf("\n\nshared memory:\n");
	print_times(clk_start_shmem, tms_start_shmem);
}

void pipe_tranfer(){
	struct tms* tms_start_pipe = malloc(sizeof(struct tms));
	clock_t clk_start_pipe = times(tms_start_pipe);

	int pipe_fd[2];
	pipe(pipe_fd);
	int pid=fork();
	if (pid < 0)
		err_sys("fork error");

	if (pid == 0){
		
		close(pipe_fd[1]);
		printf("child:\n");
		for(int i=0; i< DIM; i++){
			int n;
			read(pipe_fd[0],&n,sizeof(int));
			//printf("\n%d: %d\n",i+1,n);
		}
	}
	close(pipe_fd[0]);
	for(int i=0; i< DIM; i++){
		int n;
		write(pipe_fd[1], &numbers[i], sizeof(int));
	}
	if ((pid = waitpid(pid, NULL, 0)) < 0)
		err_sys("waitpid error");
	printf("\n\npipe:\n");
	print_times(clk_start_pipe, tms_start_pipe);		
}
int main(){
	populate_array();
	tcp_transfer();
	sh_mem_transfer();
	pipe_tranfer();
	
}
