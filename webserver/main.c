#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "socket.h"

int serv; 
int socket_client;
char message [1024]= "";
const char *message_bienvenue;

int main ()
{
	serv = creer_serveur(8080);
	if (serv ==-1){
		perror("creer_serveur");
		return -1;
	}

	while(1){
		socket_client = accept (serv, NULL, NULL);
		if (socket_client == -1){
			perror ("accept");
		}

		int pid = fork();
		if (pid == 0){
			message_bienvenue = "Bonjour, bienvenue sur mon serveur\n";
			write (socket_client, message_bienvenue, strlen(message_bienvenue));
			while (1){
				read(socket_client, message, 1023);
				write(socket_client, message, strlen(message));
			}
		}
		else{
			close(socket_client);
		}
	}
	return 0;
}
