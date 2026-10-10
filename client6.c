#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
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
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <ipv6-address|hostname> <port>\n", argv[0]);
		exit(1);
	}

	// gethostbyname() is IPv4-only, so use getaddrinfo() with AF_INET6
	struct addrinfo hints, *res, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (rc != 0) {
		fprintf(stderr, "wrong address: %s\n", gai_strerror(rc));
		exit(1);
	}

	int sockfd = -1;
	for (p = res; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd < 0)
			continue;
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
			break;
		close(sockfd);
		sockfd = -1;
	}

	if (sockfd < 0) {
		freeaddrinfo(res);
		error("error connecting to server");
	}

	char ip[INET6_ADDRSTRLEN];
	struct sockaddr_in6 *addr = (struct sockaddr_in6 *)p->ai_addr;
	inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
	printf("successfully connected to %s\n", ip);
	freeaddrinfo(res);

	char buffer[BUFFER_SIZE];
	while (1) {
		printf("client: ");
		fflush(stdout);
		memset(buffer, 0, BUFFER_SIZE);
		if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
			break;

		int nbyte = write(sockfd, buffer, strlen(buffer));
		if (nbyte < 0)
			error("Error writing to the server");
		if (strncmp("bye", buffer, 3) == 0)
			break;

		memset(buffer, 0, BUFFER_SIZE);
		nbyte = read(sockfd, buffer, BUFFER_SIZE - 1);
		if (nbyte < 0)
			error("Error reading from the server");
		if (nbyte == 0) {
			printf("Server disconnected\n");
			break;
		}

		printf("Server: %s", buffer);
		if (strncmp("bye", buffer, 3) == 0)
			break;
	}

	close(sockfd);
	return 0;
}
