#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 4096 

void error(const char* msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	// we would want 2 argument at the least 
	// first is program name itself and the second is port number 
	// that we will use later on to communicate
	if (argc < 2){
		printf("Port number not provided! ");
		exit(1);
	}

	int sockfd = socket(AF_INET6, SOCK_STREAM,0);
	// if sockfd is less then zero then we got an error
	if (sockfd < 0) 
		error("error opening socket!\n");

	//this structure defines intertet socket address
	struct sockaddr_in6 server_addr, cli_addr;

	// sets the memory reserved by them to values of zeroes 
	bzero((char *)&server_addr, sizeof(server_addr));
	int port_no = atoi(argv[1]); // converts string into int
	


	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_addr = in6addr_any; // listen to any request from any address be in lo, wifi, ethernet
	server_addr.sin6_port = htons(port_no);// to make sure it uses network byte order which is big endian
	
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
	
	//working in binary mode for audio files :)
	FILE *audio = fopen("received.mp3","wb");

	if (audio == NULL)
		error("Error opening file\n");

	// let us use ssize_t instead of int for sized values 
	ssize_t n = 0;
	ssize_t bytes_received = 0;
	/*main logic lies right here*/
	/*
	 * continue the loop until all the bytes from the client has been successfully received,
	 * we will get chunk of data in the buffer, and we will write that data onto the file 
	 */
	while ((bytes_received = read(newsockfd, buffer, sizeof(buffer))) > 0) {
		size_t bytes_written = fwrite(buffer, 1,
						(size_t)bytes_received, audio);

		/*basically if the bytes u read is not equal to bytes u write then u are cooked son 
		 * so just give up*/
		if (bytes_written != (size_t)bytes_received) {
			fclose(audio);
			error("Error writing audio file");
		}
	}
		if (bytes_received < 0) {
			fclose(audio);
			error("Error receiving audio");
		}

	if (fclose(audio) != 0)
		error("Error closing audio file");

	printf("Audio received successfully!\n");

	close(newsockfd);
	close(sockfd);

	return 0;

}

