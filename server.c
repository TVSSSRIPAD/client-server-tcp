#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "5000"
// the port users will be connecting to

#define BACKLOG 1 // the no of connections queue will hold
#define MAXMSGSIZE 150

// get IPv4  sockaddr  ;
void *getAddress(struct sockaddr *sa)
{
	return &(((struct sockaddr_in *)sa)->sin_addr);
}

int main(int argc,char *argv[])
{
	// 	Name : T.V.S.S.SRIPAD
	// Roll Number : 18CS01008
	
	if(argc != 2){
        fprintf(stderr,"Usage : %s dst \n",argv[0]);
        return 1;
    } 
    
	int sockfd, sock_conn_fd; // listen on sock_fd, new connection on sock_conn_fd
	struct addrinfo hints, *servinfo, *loopvar;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	char msg[MAXMSGSIZE];
	char rec_msg[MAXMSGSIZE];
	int rv, numbytes;

	memset(&hints, 0, sizeof hints);
	//using IPv4
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (loopvar = servinfo; loopvar != NULL; loopvar = loopvar->ai_next)
	{
		if ((sockfd = socket(loopvar->ai_family, loopvar->ai_socktype, loopvar->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, loopvar->ai_addr, loopvar->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (loopvar == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	printf("Server: waiting for connection...\n");

	sin_size = sizeof their_addr;

	// Accept connections
	sock_conn_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (sock_conn_fd == -1)
	{
		perror("accept");
	}

	inet_ntop(their_addr.ss_family, getAddress((struct sockaddr *)&their_addr), s, sizeof s);
	//Printing address of client
	printf("Server: got connection from %s\n", s);

	close(sockfd); // listener is not required any more
	printf("Enter your welcome msg: ");
	// Enter your welcome message:
	gets(msg);
	//Send welcome message
	if (send(sock_conn_fd, msg, strlen(msg), 0) == -1)
		perror("send");

	printf("Server : your welcome message is sent\n");

	if ((numbytes = recv(sock_conn_fd, rec_msg, MAXMSGSIZE - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	rec_msg[numbytes] = '\0';

	printf("Server: Welcome message received from Client\n");
	printf("Client says '%s' \n", rec_msg);

	while (1)
	{
		printf("Enter your msg:  ");
		gets(msg);
		//Send welcome message
		if (send(sock_conn_fd, msg, strlen(msg), 0) == -1)
			perror("send");

		printf("Server: Message sent\n");

		if (strcmp(msg, "good bye") == 0)
			break;

		if ((numbytes = recv(sock_conn_fd, rec_msg, MAXMSGSIZE - 1, 0)) == -1)
		{
			perror("recv");
			exit(1);
		}
		rec_msg[numbytes] = '\0';

		if (strcmp(rec_msg, "good bye") == 0)
			break;

		printf("Server: Message received\n");
		printf("Client says '%s' \n", rec_msg);
	}
	close(sock_conn_fd); // closing the connection

	return 0;
}
