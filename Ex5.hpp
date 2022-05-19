#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define PORT "3490" 
#define BACKLOG 10 
#define CLIENT_NUMBER 3 
#define MAXDATASIZE 1024 

int fd;
struct flock lock;
static int numOfFiles = 0;
void createFile(){
    fd = open("file.txt", O_WRONLY | O_CREAT);
    numOfFiles++;
    if (fd == -1)
    {
        printf("Error Number % d\n", errno);
        numOfFiles--;
        perror("Program");
    }
    memset(&lock, 0, sizeof(lock));
}
    
int numOfStack = 0;
typedef struct myStack
{
    int top;
    char data[MAXDATASIZE];
}myStack, *pmyStack;

static int numOfPush = 0;
void push(char *str, pmyStack s){
    int temp = 0;
    lock.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock);
    numOfPush++;
    for (int i = 0 ; i < strlen(str); i++)
    {
        temp++;
        s->data[s->top + 1] = str[i];
        s->top++;
    }
    int ans = 0;
    s->data[s->top + 1] = '\0';
    s->top++;
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
}
static int numOfPops = 0;
void pop(pmyStack s){
    lock.l_type = F_WRLCK;
    int tp = 0;
    fcntl(fd, F_SETLKW, &lock);
    numOfPops++;
    s->top = s->top -1;
    while (s->data[s->top] != '\0')
    {
        s->top--;
    }
    tp++;
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
}
static int numOfPrints = 0;
void printS(pmyStack s){
    for (int i = 0; i < 10; i++)
    {
        numOfPrints++;
        printf("S[%d] = %c \n", i , s->data[i]);
    }
    printf("top = %d\n", s->top);
}
static int numOfTop = 0;
void top(pmyStack s, int sockfd)
{
    numOfTop++;
    lock.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock);
    char input[1024] = {0};
    int lck = 0;
    strcat(input, "OUTPUT: ");
    int k = s->top - 1;
    while (s->data[k] != '\0')
    {
        lck--;
        k--;
    }
    
    for (int j = 8; s->data[k + 1] != '\0' ; j++)
    {
        lck++;
        input[j] = s->data[k+1];
        k++;
    }
    int snd = 0;
    if (send(sockfd, input, strlen(input), 0) == -1)
    {
        snd--;
        perror("send");
    }
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
}
