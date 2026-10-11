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
* here i will be implementing hamming code that i studied in 
* data communication , will just deal with hamming code of (7,3) for now
* basically the flow is 
* server (data) -> hamming error correction -> client receives data + parity bits for error correction  
* client verifies the data using even parity hamming correction
*
*/

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
	
	/* hamming code implementation starts here wohoooo*/

	int data[7]; //for 4 bits of data and 3 bits of parity bits 
	printf("Enter your 4 bits of data\n");

	/* reserving 0 1 and 3 for parity bits*/
	printf("bit 1: \n");
	scanf("%d",&data[2]); 
	printf("bit 2: \n");
	scanf("%d",&data[4]); 
	printf("bit 3: \n");
	scanf("%d",&data[5]); 
	printf("bit 4: \n");
	scanf("%d",&data[6]); 
	
	data[0] = data[2] ^ data[4] ^ data[6]; // D0 = D2 XOR D4 XOR D6 
	data[1] = data[2] ^ data[5] ^ data[6]; // D1 = D2 XOR D5 XOR D6
	data[3] = data[4] ^ data[5] ^ data[6]; // D3 = D4 XOR D5 XOR D6

	write(newsockfd, data, sizeof(data));

	close(newsockfd);

	close(sockfd);

	return 0;

}

