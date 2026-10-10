#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#define BUFFER_SIZE 255

void error(const char* msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{

	struct sockaddr_in server_addr, client_addr;
	if (argc < 2)
		error("please provide the port number\n");

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("error creating new socket\n");

	bzero((char *)&server_addr, sizeof(server_addr));
	int port_no = atoi(argv[1]);
	server_addr.sin_port = port_no;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
		error("error assigning address to the socket\n");

	printf("successfully created the endpoint at port: %d\n",port_no);

	listen(sockfd,5);

	socklen_t clilen = sizeof(client_addr);
	int newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clilen);
	if (newsockfd < 0)
		error("error accepting the request from client");

	printf("A client has connected to the server \n");
	printf("A websocket session has begun\n");

	char buffer[BUFFER_SIZE];
	int nbyte;
	while (1){

		bzero(buffer,BUFFER_SIZE);
		nbyte = read(newsockfd,buffer, BUFFER_SIZE);
		if (nbyte < 0)
			error("error reading from the client \n");

		printf("Client: %s",buffer);
		bzero(buffer,BUFFER_SIZE);
		printf("Server: ");
		fgets(buffer,BUFFER_SIZE,stdin);
		nbyte = write(newsockfd,buffer,BUFFER_SIZE);
		if (strncmp(buffer,"bye",3) == 0)
			break;

	}

	close(sockfd);
	close(newsockfd);

	return 0;
}
