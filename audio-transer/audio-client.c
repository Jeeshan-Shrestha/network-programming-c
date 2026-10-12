#include <ctype.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define BUFFER_SIZE 4096 

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

	int sockfd = socket(AF_INET, SOCK_STREAM,0);
	if (sockfd < 0) 
		error("error opening socket!\n");

	//this structure defines internet socket address
	struct sockaddr_in server_addr;

	// sets the memory reserved by them to values of zeroes 
	bzero((char *)&server_addr, sizeof(server_addr));
	int port_no = atoi(argv[2]); // converts string into int
	
	host_info = gethostbyname(argv[1]);

	server_addr.sin_family = AF_INET;
	bcopy((char *)host_info->h_addr_list[0],(char *)&server_addr.sin_addr.s_addr,host_info->h_length);
	server_addr.sin_port = htons(port_no);// to make sure it uses network byte order which is big endian

	if (connect(sockfd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		error("error while connecting to server\n");

	printf("successfully connected to the server\n");
	char buffer[BUFFER_SIZE];
	FILE *fp = fopen(argv[3],"rb");
	
	while (fp == NULL)
		error("Error opening the file\n");

	ssize_t bytes_read = 0;
	while ((bytes_read = fread(buffer,1,sizeof(buffer),fp)) > 0){
		
		ssize_t bytes_wrote = write(sockfd,buffer,sizeof(buffer));
		if (bytes_read != bytes_wrote){
			fclose(fp);
			error("error writing\n");
		}
			
	}	

	if (fclose(fp) != 0)
		error("everything is fucked\n");

	printf("the file has been successfully sent");

	close(sockfd);

	return 0;

}


