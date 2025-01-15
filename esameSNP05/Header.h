/**
@addtogroup Group4 
@{
*/
/**
@file 	Header.h
@author Catiuscia Melle

@brief 	Definizione di un header comune al client e server dell'esempio.
*/

#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>


#define FAILURE 3 		/**< definizione del valore di errore di ritorno del processo 
						 in caso di errori delle Socket APIs */
#define PORT 9000
#define COUNT 10 		/**< Dimensione della sequenza di messaggi inviati dal client */

#define BUFSIZE 256 	/**< Dimensione del buffer applicativo */


#endif /* __HEADER_H__ */

/** @} */
