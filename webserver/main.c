
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "socket.h"
#include <sys/types.h>
#include <stdio.h>

int serv; 
int socket_client;
char message [1024]= "r";
const char *message_bienvenue = "Bienvenue à toi jeune padawan, tu es désormais connecté au serveur Kashyyyk, en bordure Médiane. Ce serveur est développé par Antoine Duquennoy et Paul Gronier, deux grands maîtres Jedi, dans le but de permettre une communication client-serveur d'une efficacité rivalisant avec la Force. Pour cela, il utilise le protocole TCP (avec un taux élevé de médicloriens) afin d'atteindre une vitesse lumière de transfert de donnée contrairement au protocole UDP (avec un taux de médicloriens très bas et une vitesse comparable à celle de C3-PO).\n";
char* methode;
char* ressource;
char* split3;
char* splitm;
char* splitM;
char tok [1024];
const char *e400 = "HTTP/1.1 400 Bad Request\r\nConnection: Close\r\nContent-Length: 17 \r\n\n400 Bad request\r";
const char *e200 = "HTTP/1.1 200 OK\r\nContent-Length: ";
FILE * f;

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
			f = fdopen(socket_client, "w+");
			fgets(message,sizeof(message),f);
			while(fgets(tok,sizeof(tok),f)!=NULL && tok[0] != '\r' && tok[0]!= '\n' );
				methode = strtok(message, " ");
				strtok(NULL, " ");
				ressource = strtok(NULL, " ");
				split3 = strtok(NULL, " ");
				if ((strcmp(methode,"GET")==0) && (ressource != NULL) && (split3 == NULL) && ((strstr(ressource,"HTTP/1.0")==0) || (strstr(ressource,"HTTP/1.1")==0))){
					fprintf(f,"<pawnne> %s %s : %d \n %s\n",message, e200,(int)strlen(message_bienvenue),message_bienvenue);
					fflush(f);
	     			fclose(f);
				}
				else {
	      			fprintf(f,"%s",e400);
					fflush(f);
	     			fclose(f);
	    		}
		}
		else{
			close(socket_client);
		}
	}
	return 0;
}



