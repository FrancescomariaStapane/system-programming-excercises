#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../include/apue.h"
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


#define DIM 1000000

#define PORT 9000
#define BUFSIZE 9000
int tcp_receive(){
int sockfd = 0; /* listening socket del server TCP */
	int peerfd = 0; /* connected socket del client TCP */
	int ret = 0; 	/* valore di ritorno delle Sockets API */
	
	//step 1: open listening socket per IPv4
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket() error: ");
		return -1;
	}
	
	//definizione di un indirizzo IPv4 su cui il server deve mettersi in ascolto
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family= AF_INET;//IPv4 family
	
	/*
	IPv4 wildcard address: qualsiasi IP
	Notare che l'indirizzo deve essere assegnato al campo s_addr in Network Byte Order
	*/
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/*
	TCP port su cui il server si metterà in ascolto, in Network Byte Order
	*/
	addr.sin_port = htons(PORT);

	/*
	Assegno al socket l'indirizzo IPv4 memorizzato in addr
	*/
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (ret == -1)
	{
		perror("bind() error: ");
		close(sockfd);
		return -1;
	}
	
	/*
	Passive Open obbligatoria per un server TCP: abilito il TCP layer ad
	accettare connessioni per questo socket
	*/
	ret = listen(sockfd, 1);
	if (ret == -1)
	{
		perror("listen() error: ");
		close(sockfd);
		return -1;
	}
	
	printf("\n\tServer listening on port %d\n", (int)PORT);
	
	/*
	Il server TCP memorizzerà in peer_addr l'indirizzo del client connesso
	*/
	struct sockaddr_in peer_addr;
	socklen_t len = sizeof(peer_addr);
	
	int quit = 0; //regola il loop infinito nel server
	int connected = 0; //regola la gestione della connessione col client
	while (!quit) 
	{
		/*
		chiamata bloccante:
		fino a quando non c'è una connessione completa (3WHS terminato) il server rimane bloccato in accept. 
		Al ritorno, peerfd è il socket connesso della connessione col client 
		- salvo errori.
		*/
		peerfd = accept(sockfd, (struct sockaddr *)&peer_addr, &len);
		if (peerfd == -1)
		{
			perror("accept() error: ");
			close(sockfd);
			return -1;
		}
	
		char clientaddr[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &(peer_addr.sin_addr), clientaddr, INET_ADDRSTRLEN);
		printf("\tAccepted a new TCP connection from %s:%d\n", clientaddr, ntohs(peer_addr.sin_port));
	
		char buf[BUFSIZE];
		ssize_t n = 0;
		connected = 1;
		int count=0;
		
		while (connected) 
		{
			printf("receiving new data:\n");
			n = recv(peerfd, buf, BUFSIZE-1, 0);
			if (n == -1)
			{
				perror("recv() error: ");
			}
			else if (n==0)//perchè n sia 0 DEVE esserci stata una close da parte del client, altrimenti la recv non ritorna, perchè è bloccante
			{
				printf("Peer closed connection\n");
				connected = 0;
			}
			else 
			{
				buf[n] ='\0';
				printf("Received message '%s'\n", buf);
				count++;
				n = send(peerfd, "ok", strlen(buf), 0);
				if (n == -1) {
					perror("send error:");
				}
				if (count==DIM){
					close(peerfd);
					close(sockfd);
					return 0;
				}	
				
			}
		}//wend connected
		close(peerfd);
	}//wend quit
	close(sockfd);
	return 0;
}
int sh_mem_receive(){
	key_t shmem_key = ftok("./Makefile",1);
	int shmem_id = shmget(shmem_key, DIM*sizeof(int),0);
	if (shmem_id == -1){
		shmem_id = shmget(shmem_key, DIM*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	}

	key_t sem_key = ftok("./Makefile",2);//SystemV semaphore key
	int sem_id = semget(sem_key, 1, IPC_CREAT | 0666);//get semaphore
							  //
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	};
	//init semaphore
	union semun sem_union;
	sem_union.val=0;
	int r = semctl(sem_id, 0, SETVAL, sem_union);
	if (r<0){
		err_sys("semaphor fcntl error");
	}
	//prepare operation
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1; /* P() */
	//or
	//sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = SEM_UNDO;
	
	//perform operation
	semop(sem_id, &sem_b, 1);

	//read numbers from shared memory
	int* sh_numbers = shmat(shmem_id,0,0);
	if (sh_numbers  == (void*)-1){
		err_sys("shmat error");
	}
	for (int i=0; i < DIM; i++){
		int n = sh_numbers[i];
		printf("%d: %d\n",i+1,n);	
	}
	shmdt(sh_numbers); //detach shared memory
	shmctl(shmem_id, IPC_RMID, NULL); //remove shared memory	
	semctl(sem_id, 0, IPC_RMID); //remove semaphore	
	return 0;
}
int main(){
	
	tcp_receive();
	sh_mem_receive();
}
