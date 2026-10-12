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

int main(int argc, char *argv[])
{

	struct hostent* host_info; 
	if (argc < 4){
		printf("please provide ip and port number and file name");
		exit(1);
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
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

	char buffer[BUFFER_SIZE];
	FILE *fp = fopen(argv[3],"r");

	ssize_t read_bytes,sent_bytes;
	while ((read_bytes = fread(buffer,1,sizeof(buffer),fp)) > 0){

		sent_bytes = sendto(sockfd,buffer,sizeof(buffer),
		      0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		
	}

	printf("successfully sent the data\n");
	fclose(fp);


	close(sockfd);

	return 0;

}


