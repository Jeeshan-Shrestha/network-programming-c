#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define BUFFER_SIZE 255

void error(const char* msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char* argv[])
{

	struct sockaddr_in server_addr,client_addr;
	struct hostent* host_info;
	char buffer[BUFFER_SIZE];

	if (argc < 3)
		error("Please provide server ip and port number\n");

	int port_no = atoi(argv[2]);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
		error("error creating an endpoint\n");

	host_info = gethostbyname(argv[1]); // get the host by the server ip
	
	if (host_info == NULL)
		error("wrong ip\n");

	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *)host_info->h_addr_list[0],(char *)&server_addr.sin_addr.s_addr, host_info->h_length);
	server_addr.sin_port = htons(port_no);

	if (connect(sockfd,(struct sockaddr *)&server_addr, sizeof(server_addr))< 0)
		error("error connecting to server\n");

	printf("successfully connected to %s\n",host_info->h_name);

	while(1)
	{
		printf("client: ");
		bzero(buffer,BUFFER_SIZE);
		fgets(buffer,BUFFER_SIZE,stdin);
		int nbyte = write(sockfd,buffer,BUFFER_SIZE);
		if (nbyte < 0)
			error("Error writing to the server\n");

		bzero(buffer,BUFFER_SIZE);
		nbyte = read(sockfd,buffer,BUFFER_SIZE);
		if (nbyte < 0)
			error("Error reading from the server\n");

		printf("Server: %s",buffer);
		if (strncmp("bye", buffer, 3) == 0)
			break;
	}

	close(sockfd);
	return 0;
}
	
