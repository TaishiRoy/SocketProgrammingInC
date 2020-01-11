// TCP Client program 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#define PORT 5000 
#define MAXLINE 1024 
int main() 
{ 
	int sockfd; 
	char buffer[MAXLINE]; 
	struct sockaddr_in servaddr; 

	int n, len; 
	// Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		printf("socket creation failed"); 
		exit(0); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 

	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	if (connect(sockfd, (struct sockaddr*)&servaddr, 
							sizeof(servaddr)) < 0) { 
		printf("\n Error : Connect Failed \n"); 
	} 
    while(1)
    {
        printf("\nEnter Postfix Expression :");
        int count = 0;
	    memset(buffer, 0, sizeof(buffer)); 
        while((buffer[count++] = getchar()) != '\n'); ///to get input from terminal
	    // strcpy(buffer, "Hello Server"); 
	    write(sockfd, buffer, sizeof(buffer)); 
	    printf("\tReply from server: "); 
	    read(sockfd, buffer, sizeof(buffer)); 
	    puts(buffer); 
        if(strncmp(buffer,"exit",4)==0)
	    {
	    	printf("client pressed exit\n");
            close(sockfd);
	    	break;
	    }
    }
	close(sockfd); 
} 
