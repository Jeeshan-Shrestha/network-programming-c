#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>

#define BUFFER_SIZE 255

void error(const char* msg)
{
	perror(msg);
	exit(1);
}

// ./client ip_addr port_no

int main(int argc, char *argv[])
{

	struct hostent* hostinfo;
	if (argc < 3)
		error("please enter ip address and port number\n");

	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0)
		error("error creating socket \n");

	int port_no = atoi(argv[2]);
	struct sockaddr_in server_addr;

	hostinfo = gethostbyname(argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	bcopy((char *)hostinfo->h_addr_list[0],(char *)&server_addr.sin_addr.s_addr,hostinfo->h_length);
	
	char buffer[BUFFER_SIZE];

	if (connect(sockfd,(struct sockaddr*) &server_addr,sizeof(server_addr)) < 0)
		error("error while connecting to server\n");

	int nbyte;
	while (1){
		
		bzero(buffer,BUFFER_SIZE);
		fgets(buffer,BUFFER_SIZE,stdin);
		nbyte = write(sockfd,buffer,BUFFER_SIZE);
		if (nbyte < 0)
			error("Error when writing\n");

		bzero(buffer, BUFFER_SIZE);
		nbyte = read(sockfd,buffer,BUFFER_SIZE);

		if (nbyte < 0)
			error("error when reading\n");

		printf("Server: %s",buffer);
		if (strncmp("bye",buffer,3) == 0)
			break;

	}

	close(sockfd);
	return 0;

}

