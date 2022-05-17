#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>

#define PORT "3490" // the pסrt users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

#define CLIENT_NUMBER 3 // how many clients can connect server parllel

#define MAXDATASIZE 1024 // max number of bytes we can get at once
// ******************** file ****************************
int fd;
struct flock lock;
void createFile(){
    fd = open("foo1.txt", O_WRONLY | O_CREAT);

    // printf("fd = %d/n", fd);

    if (fd == -1)
    {
        // print which type of error have in a code
        printf("Error Number % d\n", errno);

        // print program detail "Success or failure"
        perror("Program");
    }
    memset(&lock, 0, sizeof(lock));
}
    

//********************* stack ***************************
typedef struct myStack
{
    int top;
    char data[MAXDATASIZE];
}myStack, *pmyStack;

// int flag = 1;

void push(char *str, pmyStack s){
    lock.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock);
    //if you want to see the procces work in syncronize
    // if (flag)
    // {
    //     sleep(10);
    //     flag = 0;
    // }
    for (int i = 0 ; i < strlen(str); i++)
    {
        s->data[s->top + 1] = str[i];
        s->top++;
    }
    s->data[s->top + 1] = '\0';
    s->top++;
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
}
void pop(pmyStack s){
    lock.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock);
    s->top = s->top -1;
    while (s->data[s->top] != '\0')
    {
        s->top--;
    }
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
}
void printS(pmyStack s){
    for (int i = 0; i < 10; i++)
    {
        printf("S[%d] = %c \n", i , s->data[i]);
    }
    printf("top = %d\n", s->top);
}

void top(pmyStack s, int sockfd)
{
    lock.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock);
    char input[1024] = {0};
    strcat(input, "OUTPUT: ");
    int k = s->top - 1;
    while (s->data[k] != '\0')
    {
        k--;
    }
    // printf("%s", s->data[k]);
    
    for (int j = 8; s->data[k + 1] != '\0' ; j++)
    {
        input[j] = s->data[k+1];
        k++;
    }
    // for (int i = 0; i < 20; i++)
    // {
    //     printf("%c", input[i]);
    // }
    if (send(sockfd, input, strlen(input), 0) == -1)
    {
        perror("send");
    }
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
}
