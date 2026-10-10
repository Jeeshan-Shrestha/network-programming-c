#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 255

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	// AF_INET6 instead of AF_INET
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("error opening socket");

	// allow quick restart of the server on the same port
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// 0 = dual-stack (accepts IPv6 AND IPv4 clients via v4-mapped addresses)
	// 1 = IPv6 only
	int v6only = 0;
	if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only)) < 0)
		error("setsockopt IPV6_V6ONLY failed");

	struct sockaddr_in6 server_addr, cli_addr;
	memset(&server_addr, 0, sizeof(server_addr));

	int port_no = atoi(argv[1]);

	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_addr = in6addr_any;     // :: (any interface), replaces INADDR_ANY
	server_addr.sin6_port = htons(port_no);

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		error("failed to bind address to the socket");

	printf("successfully created an endpoint at %d\n", port_no);

	if (listen(sockfd, 5) < 0)
		error("listen failed");

	socklen_t cli_len = sizeof(cli_addr);
	int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
	if (newsockfd < 0)
		error("couldn't accept the incoming request");

	char cli_ip[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &cli_addr.sin6_addr, cli_ip, sizeof(cli_ip));
	printf("A client has successfully connected from %s (port %d)\n",
	       cli_ip, ntohs(cli_addr.sin6_port));

	char buffer[BUFFER_SIZE];
	while (1) {
		memset(buffer, 0, BUFFER_SIZE);
		int nbyte = read(newsockfd, buffer, BUFFER_SIZE - 1);
		if (nbyte < 0)
			error("error while reading");
		if (nbyte == 0) {
			printf("Client disconnected\n");
			break;
		}

		printf("Client: %s", buffer);
		if (strncmp("bye", buffer, 3) == 0)
			break;

		memset(buffer, 0, BUFFER_SIZE);
		if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
			break;

		nbyte = write(newsockfd, buffer, strlen(buffer));
		if (nbyte < 0)
			error("error while writing");

		if (strncmp("bye", buffer, 3) == 0)
			break;
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}
