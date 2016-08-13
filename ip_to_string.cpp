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

	sockaddr_in sa;
	sockaddr_in6 sa6;
	inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr));
	inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr));

	char ip4[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
	std::cout<<"The IPv4 address is "<<ip4<<std::endl;

	char ip6[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);
	std::cout<<"The IPv6 address is "<<ip6<<std::endl;

	return 0;
}


