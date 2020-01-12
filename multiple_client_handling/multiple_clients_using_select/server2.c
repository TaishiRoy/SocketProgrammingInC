//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux

// if math lib is not linked use gcc server2.c -lm -o s to compile

#include <stdio.h> 
#include<math.h>
#include <netinet/in.h> 
#include <stdbool.h>
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <signal.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <ctype.h>
#include <errno.h> 	
#define FALSE 0 
#define PORT 8888 
#define MAX 1024
#define MAXLINE 1024 

char str[MAX], stack[MAX];
int top = -1;
 
// Stack type 
struct Stack 
{ 
    int top; 
    unsigned capacity; 
    double* array; 
}; 
  
// Stack Operations 
struct Stack* createStack( unsigned capacity ) 
{ 
    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack)); 
  
    if (!stack) return NULL; 
  
    stack->top = -1; 
    stack->capacity = capacity; 
    stack->array = (double*) malloc(stack->capacity * sizeof(double)); 
  
    if (!stack->array) return NULL; 
  
    return stack; 
} 
  
int isEmpty(struct Stack* stack) 
{ 
    return stack->top == -1 ; 
} /*
int top()
{

}*/  
double peek(struct Stack* stack) 
{ 
    return stack->array[stack->top]; 
} 
  
double pop(struct Stack* stack) 
{ 
    if (!isEmpty(stack)) 
        return stack->array[stack->top--] ; 
    return '$'; 
} 
  
void push(struct Stack* stack,double op) 
{ 
    stack->array[++stack->top] = op; 
} 

// checks if a postfix expression is valid
bool isValid(char* exp)
{
    int counter = 0, i;

    for (i = 0; exp[i+1]; ++i)
    {
        if(counter < 0)   return false;
        if(exp[i]==' ')continue; 
        else if(exp[i]=='.')continue;
        else if (isdigit(exp[i]) && (exp[i+1]==' ') )
            ++counter;
        else if (isdigit(exp[i]) && (exp[i+1]!=' ') )
            continue;
        else
        {
            --counter;
            --counter;
            if(counter < 0) return false;
            ++counter;
        }
    }
    if(counter == 1)    return true;
}

  
  
// The main function that returns value  
// of a given postfix expression 
double evaluatePostfix(char* exp) 
{ 
    // Create a stack of capacity equal to expression size 
    struct Stack* stack = createStack(strlen(exp)); 
    int i; 
    int decCounter = 0;
    double num = 0;
  
    // See if stack was created successfully 
    if (!stack) return -1; 
  
    // Scan all characters one by one 
    for (i = 0; exp[i+1]; ++i) 
    { 
        //if the character is blank space then continue 
        if(exp[i]==' ')
        {
            num = 0;
            decCounter = 0;
            continue;
        } 
        else if(exp[i]=='.') decCounter = 1;
          
        // If the scanned character is an  
        // operand (number here),extract the full number 
        // Push it to the stack. 
        else if (isdigit(exp[i])) 
        { 
            //extract full number 
            while(isdigit(exp[i]))  
            { 

                if(decCounter == 0)
                {
                    num=num*10 + (int)(exp[i]-'0'); 
                    i++; 
                }
                else if(decCounter >= 1)
                {
                    num=num + ((int)(exp[i]-'0'))*1.0/pow(10,decCounter);
                    ++decCounter;
                    i++;
                }

                
            } 
            i--; 
              
            //push the element in the stack 

            if(exp[i+1]!='.')
            {
                push(stack,num); 
            }
        } 
          
        // If the scanned character is an operator, pop two 
        // elements from stack apply the operator 
        else
        { 
            double val1 = pop(stack); 
            double val2 = pop(stack); 
            switch (exp[i]) 
            { 
            case '+': push(stack, val2 + val1); break; 
            case '-': push(stack, val2 - val1); break; 
            case '*': push(stack, val2 * val1); break; 
            case '/': push(stack, val2/val1); break; 
              
            } 
        } 

    } 
    return pop(stack); 
} 

int max(int x, int y) 
{ 
	if (x > y) 
		return x; 
	else
		return y; 
} 	
int main(int argc , char *argv[]) 
{ 
	int opt = 1; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 
		
	char *buffer; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds,fds; 
		
	//a message 
	char *message;
	message=(char*)malloc(1024*sizeof(char)); 
	buffer=(char*)malloc(1024*sizeof(char));
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
		
	//create a master socket 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
		
	//bind the socket to localhost port 8888 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	message="welcome";	
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
	FD_ZERO(&fds); 
	
		//add master socket to set 
	FD_SET(master_socket, &fds); 
	max_sd = master_socket;	
	while(1) 
	{ 
		//clear the socket set 
		readfds=fds;
		 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		//for(i=0;i<=max_sd;i++){
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
		
			//send new connection greeting message 
			if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
			{ 
				perror("send"); 
			} 
				
			puts("Welcome message sent successfully"); 
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 	
					break; 
				} 
			} 
		 
		}	
		//else its some IO operation on some other socket 
		for (i = 0; i <= max_sd; i++) 
		{ 
			sd = client_socket[i]; 
			bzero(buffer,MAX);	
			if (FD_ISSET( sd , &readfds)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message 
				if ((valread = read( sd , buffer, 1024)) <= 0) 
				{ 
					//Somebody disconnected , get his details and print 
					getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n[Press Ctrl+C to close server]\n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
						
					//Close the socket and mark as 0 in list for reuse 
					close( sd ); 
					client_socket[i] = 0; 
				} 
					
				//Echo back the message that came in 
				else
				{ 
					//set the string terminating NULL byte on the end 
					//of the data read 
					buffer[valread] = '\0'; 
					//write(sd , buffer , strlen(buffer)); 
					if(strncmp(buffer,"exit",4)==0)
	                {
	                	printf("client pressed exit [Press Ctrl+C to close server]");
					    FD_CLR(sd,&readfds);
	                	break;
	                }
                    if(!isValid(buffer))
                    {
                    	printf("\tINVALID INPUT !!\n");
					    bzero(buffer,MAX);
                    	sprintf(buffer, "INVALID INPUT !!");
					    if((valread=write(sd, (const char*)buffer, 1024))<=0)	
					    {
					    	printf("write error:");						
					    }
                    }
                    else
                    {
					    printf("Postfix Expression :%s",buffer);
                    	double res = evaluatePostfix(buffer);
                    	bzero(buffer,MAX);
                    	sprintf(buffer, "%lf", res);
					    printf("\tPostfix expression evaluates to : %s\n\n", buffer);
					    if((valread=write(sd,buffer, 1024))<=0)	
					    {
					    	printf("write error:");						
					    }
					
                    }
	                		 				
				} 
			} 
		} 
	} 
		
	return 0; 
} 
