#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "socket.h"
#include <sys/types.h>


int serv; 
int socket_client;
char message [1024]= "r";
const char *message_bienvenue;


int main ()
{
	serv = creer_serveur(8080);
	if (serv ==-1){
		perror("creer_serveur");
		return -1;
	}
	initialiser_signaux();
	while(1){
		socket_client = accept (serv, NULL, NULL);
		if (socket_client == -1){
			perror ("accept");
		}

		int pid = fork();
		if (pid == 0){
			message_bienvenue = "Bienvenue à toi jeune padawan, tu es désormais connecté au serveur Kashyyyk, en bordure Médiane. Ce serveur est développé par Antoine Duquennoy et Paul Gronier, deux grands maîtres Jedi, dans le but de permettre une communication client-serveur d'une efficacité rivalisant avec la Force. Pour cela, il utilise le protocole TCP (avec un taux élevé de médicloriens) afin d'atteindre une vitesse lumière de transfert de donnée contrairement au protocole UDP (avec un taux de médicloriens très bas et une vitesse comparable à celle de C3-PO).\n";
			write (socket_client, message_bienvenue, strlen(message_bienvenue));
			FILE * f;
			f = fdopen(socket_client, "w+");
			while(fgets(message, sizeof(message), f) != NULL){
				fprintf(f, "<Pawnee> %s", message);
			}
		}
		else{
			close(socket_client);
		}
	}
	return 0;
}



