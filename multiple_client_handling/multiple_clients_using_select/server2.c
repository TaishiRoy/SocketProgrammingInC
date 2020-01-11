// Server program 
#include <arpa/inet.h> 
#include<stdbool.h>
#include <ctype.h>
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#define MAX 255 
char str[MAX], stack[MAX];
#define PORT 5000 
#define MAXLINE 1024 

char str[MAX], stack[MAX];
int top = -1;
 
// Stack type 
struct Stack 
{ 
    int top; 
    unsigned capacity; 
    int* array; 
}; 
  
// Stack Operations 
struct Stack* createStack( unsigned capacity ) 
{ 
    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack)); 
  
    if (!stack) return NULL; 
  
    stack->top = -1; 
    stack->capacity = capacity; 
    stack->array = (int*) malloc(stack->capacity * sizeof(int)); 
  
    if (!stack->array) return NULL; 
  
    return stack; 
} 
  
int isEmpty(struct Stack* stack) 
{ 
    return stack->top == -1 ; 
} 
  
int peek(struct Stack* stack) 
{ 
    return stack->array[stack->top]; 
} 
  
int pop(struct Stack* stack) 
{ 
    if (!isEmpty(stack)) 
        return stack->array[stack->top--] ; 
    return '$'; 
} 
  
void push(struct Stack* stack,int op) 
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
int evaluatePostfix(char* exp) 
{ 
    // Create a stack of capacity equal to expression size 
    struct Stack* stack = createStack(strlen(exp)); 
    int i; 
  
    // See if stack was created successfully 
    if (!stack) return -1; 
  
    // Scan all characters one by one 
    for (i = 0; exp[i+1]; ++i) 
    { 
        //if the character is blank space then continue 
        if(exp[i]==' ')continue; 
          
        // If the scanned character is an  
        // operand (number here),extract the full number 
        // Push it to the stack. 
        else if (isdigit(exp[i])) 
        { 
            int num=0; 
              
            //extract full number 
            while(isdigit(exp[i]))  
            { 
            num=num*10 + (int)(exp[i]-'0'); 
                i++; 
            } 
            i--; 
              
            //push the element in the stack 
            push(stack,num); 
        } 
          
        // If the scanned character is an operator, pop two 
        // elements from stack apply the operator 
        else
        { 
            int val1 = pop(stack); 
            int val2 = pop(stack); 
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
int main() 
{ 
	int listenfd, connfd, udpfd, nready, maxfdp1; 
	char buffer[MAXLINE]; 
	pid_t childpid; 
	fd_set rset; 
	ssize_t n; 
	socklen_t len; 
	const int on = 1; 
	struct sockaddr_in cliaddr, servaddr; 
	char* message = "Insert Answer"; 
	void sig_chld(int); 

	/* create listening TCP socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0); 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// binding server addr structure to listenfd 
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	listen(listenfd, 10); 
    printf("Server listening..\n");

	/* create UDP socket */
	udpfd = socket(AF_INET, SOCK_DGRAM, 0); 
	// binding server addr structure to udp sockfd 
	bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

	// clear the descriptor set 
	FD_ZERO(&rset); 

	// get maxfd 
	maxfdp1 = max(listenfd, udpfd) + 1; 
	for (;;) { 

		// set listenfd and udpfd in readset 
		FD_SET(listenfd, &rset); 
		FD_SET(udpfd, &rset); 

		// select the ready descriptor 
		nready = select(maxfdp1, &rset, NULL, NULL, NULL); 
        printf("\nConnection established with client..\n");
		// if tcp socket is readable then handle 
		// it by accepting the connection 
		if (FD_ISSET(listenfd, &rset)) { 
			len = sizeof(cliaddr); 
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len); 
			if ((childpid = fork()) == 0) { 
				close(listenfd); 
                while(1)
                {
				    bzero(buffer, sizeof(buffer)); 
				    printf("Message From TCP client: "); 
				    read(connfd, buffer, sizeof(buffer)); 
				    puts(buffer); 
                    if(strncmp(buffer,"exit",4)==0)
	                {
	                	printf("client pressed exit [Press Ctrl+C to close server]");
	                	break;
	                }
                    if(!isValid(buffer))
                    {
                        printf("\tINVALID INPUT !!\n");
                        sprintf(buffer, "Invalid Input");
                    }
                    else
                    {
                        int res = evaluatePostfix(buffer);
                        printf("\tPostfix expression evaluates to : %d\n", res);
                        sprintf(buffer, "%d", res);	
                    }
	                write(connfd, (const char*)buffer, sizeof(buffer));  
                }
                close(connfd); 
				exit(0);
			} 
			close(connfd); 
		} 
	} 
} 
