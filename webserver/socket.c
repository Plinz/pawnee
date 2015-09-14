#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


int socket_serveur;
int socket_client;
const char *message_bienvenue;
char message [50]= "";
int len = 0;

int creer_serveur(int port){

	struct sockaddr_in addr;

	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_serveur == -1){
		perror("socket_serveur");
		return -1;
	}



	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_serveur, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		perror("bind socket_serveur");
		return -1;
	}

	if (listen(socket_serveur, 10) == -1){
		perror("listen socket_serveur");
		return -1;
	}

	socket_client = accept (socket_serveur, NULL, NULL);
	if (socket_client == -1){
		perror ("accept");
	}

	message_bienvenue = "Bonjour, bienvenue sur mon serveur\n";
	write (socket_client, message_bienvenue, strlen(message_bienvenue));
	while (1){
		len = read(socket_client, message, 49);
		write(socket_client, message, strlen(message));
	}
}
