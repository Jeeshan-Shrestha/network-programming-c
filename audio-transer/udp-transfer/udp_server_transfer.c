#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

void error(const char* msg)
{
	perror(msg);
	exit(1);
}


int main(int argc, char* argv[])
{

	if (argc < 2)
		error("please provide the port number \n");

	int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	struct sockaddr_in server_addr, client_addr;

	int port_no = atoi(argv[1]);
	server_addr.sin_port = htons(port_no);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
		error("error while binding the address\n");

	printf("successfully created an endpoint at %d\n",port_no);

	FILE *fp = fopen("received.mp3","wb");

	if (fp == NULL)
		error("error opening file\n");

	char buffer[4096];

	socklen_t cli_len = sizeof(client_addr);
	ssize_t rec_bytes,write_bytes;
	while ((rec_bytes = recvfrom(sockfd, buffer, 
		 sizeof(buffer), 0,
		 (struct sockaddr*)&client_addr, &cli_len)) > 0 ){
		

		write_bytes = fwrite(buffer,1,sizeof(buffer),fp);

	}

	printf("successfully received the data\n");

	fclose(fp);
	close(sockfd);
	return 0;
}
