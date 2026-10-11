#include <ctype.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

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

	struct hostent* host_info; 
	if (argc < 4){
		printf("please provide ip and port number and file name");
		exit(1);
	}

	int sockfd = socket(AF_INET6, SOCK_STREAM,0);
	if (sockfd < 0) 
		error("error opening socket!\n");

	//this structure defines internet socket address
	struct sockaddr_in6 server_addr;

	// sets the memory reserved by them to values of zeroes 
	bzero((char *)&server_addr, sizeof(server_addr));
	int port_no = atoi(argv[2]); // converts string into int
	
	host_info = gethostbyname2(argv[1],AF_INET6);
	
	if (host_info == NULL)
		error("couldnt get the host from the ip\n");
	server_addr.sin6_family = AF_INET6;
	bcopy((char *)host_info->h_addr_list[0],(char *)&server_addr.sin6_addr,host_info->h_length);
	server_addr.sin6_port = htons(port_no);// to make sure it uses network byte order which is big endian

	if (connect(sockfd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		error("error while connecting to server\n");

	printf("successfully connected to the server\n");
	char buffer[BUFFER_SIZE];
	FILE *fp = fopen(argv[3],"r");
	int words = 0;
	int c;
	while ( (c = getc(fp)) != EOF){

		// this is to actually move the file pointer front even though we are not really 
		// caring about the data we are storing as of rn
		fscanf(fp,"%s", buffer);
		if (isspace(c) || c == '\t')
			words++;

	}

	write(sockfd,&words,sizeof(int)); //sending the number of words to the file pointed by file descriptor 
	//through the socket

	rewind(fp); // bring back the file pointer to the top
	
	int ch;
	while (ch != EOF){
		fscanf(fp,"%s",buffer);
		write(sockfd,buffer,BUFFER_SIZE);
		ch = getc(fp);

	}

	printf("the file has been successfully sent");

	close(sockfd);

	return 0;

}

