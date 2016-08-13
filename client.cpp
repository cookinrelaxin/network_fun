#include <iostream>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cerrno>

extern "C" {
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
}

#define PORT "80"
#define MAXDATASIZE 100

void *get_in_addr(sockaddr *sa) {
	if (sa->sa_family == AF_INET)
		return &(((sockaddr_in*)sa)->sin_addr);
	return &(((sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {

	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	 	fprintf(stderr, "usage: client hostname\n");
		exit(1);
	}

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	for (p = servinfo; p != nullptr; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (!p) {
		fprintf(stderr, "client: failed to connect\n");
		exit(2);
	}

	inet_ntop(p->ai_family, get_in_addr((sockaddr *)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1,0)) == -1) {
		perror("recv");
		exit(1);
	}
	
	buf[numbytes] = '\0';

	printf("client: received '%s'\n", buf);

	close(sockfd);

	return 0;
}
