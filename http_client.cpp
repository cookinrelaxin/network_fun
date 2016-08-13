#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <string>

extern "C" {
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
}

void usage() {
	fprintf(stderr, "USAGE: htmlget host [page]\n\
\thost: the website hostname. ex: coding.debuntu.org\n\
\tpage: the page to retrieve. ex: index.html, default: /\n");
}

class IP_Address {
	std::string host;
	std::string service;
	std::string printable;
	int file_descriptor;

	void *get_in_addr(sockaddr *sa) {
		if (sa->sa_family == AF_INET) {
			return &(((sockaddr_in*)sa)->sin_addr);
		}
		return &(((sockaddr_in6*)sa)->sin6_addr);
	}

	public:
		IP_Address(std::string hostname) {
			int rv;

			addrinfo *hints = new addrinfo {0,       // int ai_flags
										AF_UNSPEC,   // int ai_family
										SOCK_STREAM, // int ai_socktype
										0,           // int ai_protocol
										0,           // socklen_t ai_addrlen
										0,           // char *ai_canonname
										0,           // sockaddr *ai_addr
										0};          // addrinfo *ai_next


			addrinfo *servinfo, *p;
			if ((rv = getaddrinfo(hostname.c_str(), "http", hints, &servinfo)) != 0) {
			 	fprintf(stderr, "oops! getaddrinfo: %s\n", gai_strerror(rv));
				exit(1);
			}

			for (p = servinfo; p != nullptr; p = p->ai_next) {

				if ((file_descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
					perror("socket");
					continue;
				}
				if (connect(file_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
					perror("connect");
					close(file_descriptor);
					continue;
				}
				break;
			}

			if (p == nullptr) {
				fprintf(stderr, "failed to connect\n");
				exit(2);
			}

			char s[INET6_ADDRSTRLEN];
			inet_ntop(p->ai_family, get_in_addr((sockaddr*)p->ai_addr), s, sizeof s);
			printable = s;

			char host_c[1024];
			char service_c[20];

			getnameinfo(p->ai_addr, sizeof (p->ai_addr), host_c, sizeof host_c, service_c, sizeof service_c, 0);
			host = host_c;
			service = service_c;

			freeaddrinfo(servinfo);
			delete hints;

		}

		std::string get_printable() {
			return printable;
		}

		std::string get_host() {
			return host;
		}

		std::string get_service() {
			return service;
		}

		int get_file_descriptor() {
			return file_descriptor;
		}

};

class GETQuery {
	public:
		std::string q;
		GETQuery(std::string host, std::string page) {
			q = "GET /";
			q += page;
			q += " HTTP/1.0";
			q += "\n";
			q += "Host: ";
			q += host;
			q += "\n";
			q += "User-Agent: ";
			q += "HTMLGET 1.0";
			q += "\n";
			q += "\n";
		}
		
};
 
int main(int argc, char **argv) {

  	if (argc == 1) {
  		usage();
  	  	exit(2);
  	}  

  	std::string host(argv[1]);

	IP_Address ip(host);

  	std::string page;

  	if (argc > 2){
		page = argv[2];
  	}
	else {
		page = "/";
  	}

	char *get = (char*)(GETQuery(host, page).q.c_str());
  	fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
 
  	int sent = 0, tmpres;
  	while (sent < strlen(get)) {
		tmpres = send(ip.get_file_descriptor(), get+sent, strlen(get)-sent, 0);
		if(tmpres == -1){
			perror("Can't send query");
  	    	exit(1);
  	  	}
  	  	sent += tmpres;
  	}

	std::string response;
  	char buf[250];
  	while ((tmpres = recv(ip.get_file_descriptor(), buf, sizeof(buf), 0)) > 0) {
		for (int i(0); i<tmpres; ++i)
			response.push_back(buf[i]);
  	}
  	if (tmpres < 0) {
		perror("Error receiving data");
  	}
	fprintf(stdout, response.c_str());
  	close(ip.get_file_descriptor());
  	return 0;
}
