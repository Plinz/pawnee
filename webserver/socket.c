#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>


enum http_method {
	HTTP_GET,
	HTTP_UNSUPPORTED,
};

typedef struct {
	enum	http_method method;
	int	major_version;
	int	minor_version;
	char	*url;
} http_request;

int socket_serveur;


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

	int optval = 1;
	if (setsockopt(socket_serveur, SOL_SOCKET , SO_REUSEADDR , &optval , sizeof(int)) == -1){
	perror("Can not set SO_REUSEADDR option");
	}

	if (bind(socket_serveur, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		perror("bind socket_serveur");
		return -1;
	}

	if (listen(socket_serveur, 10) == -1){
		perror("listen socket_serveur");

		return -1;
	}
	return socket_serveur;

}

char * fgets_or_exit(char *buffer , int size , FILE *stream){
	char *buf;	
	if((buf = fgets(buffer, size, stream)) == NULL) {
		exit(1);
	}
	return buf;
}

int parse_http_request(const char *request_line , http_request *request){
	char* met;
	char* url;
	char* http_version;
	char* str = strdup(request_line);
	if((met = strtok (str," ")) == NULL) return 0;
	if((url = strtok (NULL, " ")) == NULL) return 0;
	if((http_version = strtok (NULL, " ")) == NULL) return 0;
	if(strcmp(met,"GET") == 0)
		request->method = HTTP_GET;
	else
		request->method = HTTP_UNSUPPORTED;
	if ((strcmp(http_version,"HTTP/1.0")==1) && (strcmp(http_version,"HTTP/1.1")==1)) return 0;
	request->url = url;
	return 1;
}

void skip_headers(FILE *client){
	char buf[1024];
	while(buf[0] != '\r' && buf[1] != '\n') fgets_or_exit(buf, sizeof(buf), client);
}

void send_status(FILE *client , int code , const char *reason_phrase){
	char status[256];
	sprintf(status, "HTTP/1.1 %d %s\r\n", code, reason_phrase);
	fprintf(client, status);
}

void send_response(FILE *client , int code , const  char *reason_phrase , const  char *message_body){
	send_status(client, code, reason_phrase);
	if(message_body != NULL) {
		char content_length[256];
		sprintf(content_length, "Content-Length: %zu\r\n", strlen(message_body));
		fprintf(client, "Content-Type: text/html\r\n");
		fprintf(client, content_length);
		fprintf(client, "\r\n");
		fprintf(client, message_body);
	}
	fprintf(client, "\r\n");
}

void traitement_signal(int sig)
{
	printf("Signal %d reçu\n", sig);
	int status;
	waitpid(-1, &status, WUNTRACED);
}

void initialiser_signaux(void)
{
	struct sigaction sa;
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD , &sa, NULL) == -1)
	{
		perror("sigaction(SIGCHLD)");
	}
}

char * rewrite_url ( char *url ){
	return strtok(url,"?");
}

int check_and_open ( const char *url , const char * document_root ){
  struct stat buf;
  int fd;
  char *tmpURL="";
  char *tmpDOC="";

  tmpURL=strdup(url);
  tmpDOC=strdup(document_root);
  strcat(tmpDOC,rewrite_url(tmpURL));
  
  if((stat(tmpDOC,&buf)!=-1) && ((fd = open(tmpDOC,O_RDONLY))!=-1) && (S_ISREG(buf.st_mode)==0)){
	return fd;
  }
  else{
	return -1;
  }
}

int get_file_size(int fd){
	struct stat buf;
	fstat(fd, &buf);
	return (buf.st_blocks*512);
}

int copy(int in, int out){
	char buf [1024];
	int n;
	while((n = read(in, buf, 1024)) > 0){
		write(out,buf,n);
	}
	return 0;
}