#include <stdio.h> 
#include<stdbool.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include<unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#define MAX 255 
#define PORT 5001
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
 
int main()
{
	int socket_fd, connection_fd, length,count,n,read_check,temp;
	char line[256];
	int i = 0;
	FILE *fptr; 
	char c; 
	char buff[MAX];
	char *buffer2;
	buffer2=(char *)malloc((MAX+1)*sizeof(char));
	memset(buffer2,0,MAX);
	struct sockaddr_in server_addr, client; 
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);  // for creating socket and  verification of socket 
	if (socket_fd == -1) 
	{ 
		printf("socket creation failed\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created\n"); 
	bzero(&server_addr, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET;  ///assign ipv4 family
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //assign ip address
	server_addr.sin_port = htons(PORT); ///assign port
	if ((bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) != 0) //// to Bind  created socket to given IP 
	{ 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n");  
    	/* */
	
	if ((listen(socket_fd, 5)) != 0) // lsiten and verification of server 
	{ 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n");
	int count1=0;	
	for(;;){
	length = sizeof(client); 	
	connection_fd = accept(socket_fd, (struct sockaddr*)&client, &length); // to accept data from client 
	count1++;
	if (connection_fd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client socket no %d...\n",count1); 
	
	if(fork()==0){
	close(socket_fd);
        for(;;){ 	
	bzero(buff, MAX);
	read_check = read(connection_fd, buff, sizeof(buff)); // to copy the message that is read from client
	if(read_check==-1) 
	{
		printf("READ ERROR");
		exit(1);
	}
	printf("\nClient socket %d sent message: = %s",count1,buff);
    if(strncmp(buff,"exit",4)==0)
	{
		printf("client press exit:\n");
		close(connection_fd);	
        close(socket_fd);
		break;
	}
    if(!isValid(buff))
    {
        printf("\tINVALID INPUT !!\n");
        sprintf(buff, "Invalid Input");
    }
    else
    {
        int res = evaluatePostfix(buff);
        printf("\tPostfix expression evaluates to : %d\n", res);
        sprintf(buff, "%d", res);	
    }
	write(connection_fd,buff,sizeof(buff));
	
	}
	}
	//close(socket_fd);

}}
