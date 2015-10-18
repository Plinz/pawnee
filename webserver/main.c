#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "socket.h"
#include <sys/types.h>
#include <stdio.h>

int serv; 
int socket_client;
char message [1024]= "";
const char *message_bienvenue = "Bienvenue à toi jeune padawan, tu es désormais connecté au serveur Kashyyyk, en bordure Médiane. Ce serveur est développé par Antoine Duquennoy et Paul Gronier, deux grands maîtres Jedi, dans le but de permettre une communication client-serveur d'une efficacité rivalisant avec la Force. Pour cela, il utilise le protocole TCP (avec un taux élevé de médicloriens) afin d'atteindre une vitesse lumière de transfert de donnée contrairement au protocole UDP (avec un taux de médicloriens très bas et une vitesse comparable à celle de C3-PO).\n";
char* methode;
char * url;
char* ressource;
char* version;
char buf [1024];
const char *e404 = "404 NOT FOUND\r\n";
const char *e400 = "HTTP/1.1 400 Bad Request\r\nConnection: Close\r\nContent-Length: 17 \r\n\n400 Bad request\r";
const char *e200 = "HTTP/1.1 200 OK\r\nContent-Length: ";
FILE * f;
http_request * request;
int req;

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
			fgets_or_exit(message,sizeof(message),f);
			req = parse_http_request(message, request);
			skip_headers(f);

			if (!req){
				send_response(f, 400, "Bad Request", "400 : Bad Request");
			}
			else if(request->method == HTTP_UNSUPPORTED){
				send_response(f, 405, "Method Not Allowed", "405 : Method Not Allowed");
			}
			else if(strcmp(request->url, "/") == 0){
				send_response(f, 200, "OK", message_bienvenue);
			}
			else{
				send_response(f, 404, "Not Found", "404 : Not Found");
			}
			fclose(f);			
		}
		else{
			close(socket_client);
		}
	}
	return 0;
}



