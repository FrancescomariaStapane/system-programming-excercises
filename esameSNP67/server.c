#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../include/apue.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 9000
#define BUFSIZE 9000
int main(){
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
				if (strcmp(buf, "close")==0){
					printf("closing server...\n");
					close(sockfd);
					exit(0);
				}
				setenv("A",buf,1);
				printf("A=%s\n",getenv("A"));
			}
		}//wend connected
		close(peerfd);
	}//wend quit
	close(sockfd);
	
return 0;	
}
