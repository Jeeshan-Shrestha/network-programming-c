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

	int data[7];

	read(sockfd,data,sizeof(data));
	for (int i = 0 ; i < 7 ; i++)
		printf("%d ",data[i]);

	printf("\n");
	/*check if data bits are all correct or not*/

	int p,p1,p2,p4;
	
	
	p1 = (data[0] ^ data[2] ^ data[4] ^ data[6]) == 0 ? 0 : 1; // D0 = D2 XOR D4 XOR D6 
	p2 = (data[1] ^ data[2] ^ data[5] ^ data[6]) == 0 ? 0 : 1; // D1 = D2 XOR D5 XOR D6
	p4 = (data[3] ^ data[4] ^ data[5] ^ data[6]) == 0 ? 0 : 1; // D3 = D4 XOR D5 XOR D6

	p = (p4 << 2) | (p2 << 1) | (p1 << 0);
	printf("parity checker : %d\n",p);

	if (p == 0)
		printf("no error in transmission \n");
	else{
		printf("theres error at position: %d\n",p);
		data[p - 1] ^= 1; // flip the bit
		
		//corrected bits
		for (int i = 0 ; i < 7 ; i++)
			printf("%d ",data[i]);

		printf("\n");
	}

	close(sockfd);
	return 0;
}
	
