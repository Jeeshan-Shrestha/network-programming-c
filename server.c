#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 255

void error(const char* msg)
{
	perror(msg);
	exit(1);
}

/*
 * GENERAL FLOW OF THE PROGRAM
 * create a socket() 
 * assign address to the socket using bind()
 * also note the bind takes struct sockaddr but we will use sockaddr_in (for ipv4) and cast it later
 * we will implement accept() for accepting the incoming request from the client
 * we will also read() and write() 
 * and finally close() to close the session
 * */

int main(int argc, char *argv[])
{
	// we would want 2 argument at the least 
	// first is program name itself and the second is port number 
	// that we will use later on to communicate
	if (argc < 2){
		printf("Port number not provided! ");
		exit(1);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM,0);
	// if sockfd is less then zero then we got an error
	if (sockfd < 0) 
		error("error opening socket!\n");

	//this structure defines intertet socket address
	struct sockaddr_in server_addr, cli_addr;

	// sets the memory reserved by them to values of zeroes 
	bzero((char *)&server_addr, sizeof(server_addr));
	int port_no = atoi(argv[1]); // converts string into int
	


	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // listen to any request from any address be in lo, wifi, ethernet
	server_addr.sin_port = htons(port_no);// to make sure it uses network byte order which is big endian
	
	// need to cast sockaddr_in to sockaddr because sockaddr_in specifies internet specific 
	// structure while sockaddr is general structure
	// example think of sockaddr as a parent of sockaddr_in and sockaddr_in6 
	if (bind(sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0)
		error("failed to bind address to the socket\n");
	
	printf("successfully created an endpoint at %d\n",port_no);

	//listen to any calls 
	listen(sockfd, 5); // second parameter is the maximum no. of concurrent clients
	
	socklen_t cli_len = sizeof(cli_addr);

	int newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &cli_len);

	if (newsockfd < 0)
		error("couldnt accept the incoming request");

	printf("A client has successfully connected\n");
	char buffer[BUFFER_SIZE];
	while(1){

		bzero(buffer,BUFFER_SIZE);
		int nbyte = read(newsockfd,buffer,BUFFER_SIZE);
		if (nbyte < 0)
			error("error while reading");

		printf("Client: %s",buffer);
		bzero(buffer, BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);
		if (strncmp("bye", buffer, 3) == 0)
			break;

		nbyte = write(newsockfd,buffer,BUFFER_SIZE);
		if (nbyte < 0 )
			error("Error while writing\n");


	}

	close(newsockfd);
	close(sockfd);

	return 0;

}

