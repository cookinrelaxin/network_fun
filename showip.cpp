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

int main(int argc, char *argv[]) {

	addrinfo hints, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	if (argc != 2) {
		fprintf(stderr, "usage: showip hostname\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(argv[1], nullptr, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		exit(2);
	}

	printf("IP addresses for %s: \n\n", argv[1]);

	for (p = res; p != nullptr; p = p->ai_next) {
		void *addr;
		std::string ipver;

		if (p->ai_family == AF_INET) {
			sockaddr_in *ipv4 = (sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else {
			sockaddr_in6 *ipv6 = (sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("  %s: %s\n", ipver.c_str(), ipstr);
	}

	freeaddrinfo(res);

	return 0;
}
