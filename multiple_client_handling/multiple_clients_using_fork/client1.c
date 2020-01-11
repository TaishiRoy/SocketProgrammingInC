#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include<unistd.h>
#include <arpa/inet.h>
#define MAX 255 
#define PORT 5001
int main()
{
	int socket_fd,count; 
	char buff[MAX];  
	struct sockaddr_in server_addr; 
	socket_fd = socket(AF_INET, SOCK_STREAM, 0); // for creating socket and  verification of socket 
	if (socket_fd == -1) 
	{ 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&server_addr, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET;  ///assign ipv4 family
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //assign ip address
	server_addr.sin_port = htons(PORT); //assign port no
		
	if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)  // connect the client socket to server socket 
	{ 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 
	for(;;){
	bzero(buff, sizeof(buff)); 
	printf("Please enter the message to the server:  "); 
	count = 0; 
	while((buff[count++] = getchar()) != '\n'); ///to get input from terminal
	write(socket_fd, buff, sizeof(buff)); //to send the message from client to server
	
	bzero(buff, sizeof(buff));  //to resize the buffer
	read(socket_fd, buff, sizeof(buff)); ///to read message from server
	printf("Server replied: : %s\n", buff);  ///to print server's message
	if(strncmp(buff,"exit",4)==0){
		printf("Client Exit...\n"); 
        close(socket_fd);
		break;
	}	
	 
	}
	close(socket_fd); //  to close the socket 
}
