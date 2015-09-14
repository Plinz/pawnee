#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int socket_serveur;
int socket_client;
const char *message_bienvenue;

int creer_serveur(int port){

	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_serveur == -1){
		perror("socket_serveur");
		return -1;
	}

	struct sockaddr_in addrIPV4;

	addrIPV4.sin_family = AF_INET;
	addrIPV4.sin_port = htons(port);
	addrIPV4.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_serveur, (struct sockaddr *)&addrIPV4, sizeof(addrIPV4)) == -1){
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

}
