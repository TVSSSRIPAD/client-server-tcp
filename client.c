#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "5000" // the port client will be connecting to

#define MAXMSGSIZE 150 // max number of bytes we can get at once

// get IPv4  sockadd
void *getAddress(struct sockaddr *sa)
{
	return &(((struct sockaddr_in *)sa)->sin_addr);
}

int main(int argc, char *argv[])
{
	// 	Name : T.V.S.S.SRIPAD
	// Roll Number : 18CS01008
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *loopvar;
	int rv;
	char s[INET6_ADDRSTRLEN], msg[MAXMSGSIZE], rec_msg[MAXMSGSIZE];

	memset(&hints, 0, sizeof hints);
	//using IPv4
	hints.ai_family = AF_INET;

	hints.ai_socktype = SOCK_STREAM;

	//getAddressInfo
	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for (loopvar = servinfo; loopvar != NULL; loopvar = loopvar->ai_next)
	{
		if ((sockfd = socket(loopvar->ai_family, loopvar->ai_socktype, loopvar->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, loopvar->ai_addr, loopvar->ai_addrlen) == -1)
		{
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (loopvar == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(loopvar->ai_family, getAddress((struct sockaddr *)loopvar->ai_addr), s, sizeof s);
	printf("Client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, rec_msg, MAXMSGSIZE - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}

	rec_msg[numbytes] = '\0';
	printf("Client: welcome message received from server \n");
	printf("Server says '%s'\n", rec_msg);

	printf("Enter your welcome msg: ");
	// Enter your welcome message:
	gets(msg);
	//Send welcome message
	if (send(sockfd, msg, strlen(msg), 0) == -1)
		perror("send");
	printf("Client : your welcome message is sent.\n");

	while (1)
	{

		if ((numbytes = recv(sockfd, rec_msg, MAXMSGSIZE - 1, 0)) == -1)
		{
			perror("recv");
			exit(1);
		}

		rec_msg[numbytes] = '\0';

		printf("Client: Message received\n");
		printf("Server says '%s' \n", rec_msg);

		if (strcmp(rec_msg, "good bye") == 0)
			break;

		printf("Enter your msg: ");
		gets(msg);
		//Send welcome message
		if (send(sockfd, msg, strlen(msg), 0) == -1)
			perror("send");

		printf("Client: Message sent\n");

		if (strcmp(msg, "good bye") == 0)
			break;
	}
	close(sockfd); // closing the connection

	return 0;
}
