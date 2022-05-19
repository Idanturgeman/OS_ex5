/*
** server.c -- a stream socket server demo
*/

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











pmyStack stack1;
static int numOfThread = 0;
void *myThread(void *arg)
{
    char input[1024] = {0};
    int new_fd = *(int *)arg;
    numOfThread++;
    int numbytes;
    while (strcmp(input, "EXIT"))
    {
        static int numOfPush = 0;
        static int numOfPop = 0;
        static int numOfTop = 0;
        char str[1024];
        bzero(str, 1024);
        if (strncmp(input, "PUSH", 4) == 0)
        {
            for (int i = 5, j = 0; i < strlen(input); i++, j++)
            {
                str[j] = input[i];
            }
            numOfPush++;
            push(str, stack1);
            // printS(stack1);
        }
        else if (strncmp(input, "POP", 3) == 0)
        {
            pop(stack1);
            numOfPop++;
        }
        else if (strncmp(input, "TOP", 3) == 0)
        {
            top(stack1, new_fd);
            numOfTop++;
            // printS(stack1);
        }
        bzero(input, 1024);
        int temp = 0;
        if ((numbytes = recv(new_fd, input, MAXDATASIZE - 1, 0)) == -1)
        {
            temp++;
            perror("recv");
            exit(1);
        }
        int ans = 0;
        if (!numbytes)
        {
            ans++;
            printf("client disconnect\n");
            close(new_fd);
            close (fd);
            return NULL;
        }

        input[numbytes] = '\0';
        printf("server received: ");
        for (int i = 0; i < strlen(input); i++)
        {
            int k = 0;
            printf("%c", input[i]);
        }
        printf("\n");
    }
    printf("good bye\n");
    close(new_fd);
    close (fd);
    return NULL;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    int ad = 0;
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    int sg = 0;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

int main(void)
{
    createFile();
    stack1 = (pmyStack)mmap(0, 2000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANON, -1, 0);
    stack1->data[0] = '\0';
    stack1->top = 0;
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    // pthread_mutex_init(&mutex,NULL);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    int ai = 0;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        int sck = 0;
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            sck++;
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            sck++;
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            sck++;
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure
    int fd = 0;
    if (p == NULL)
    {
        fd++;
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        fd++;
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    int saf = 0;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        saf++;
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    pthread_t tid[CLIENT_NUMBER];
    int i = 0;
    int k = 0;
    while (1)
    { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            k++;
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork())
        { // this is the child process
            i++;
            close(sockfd); // child doesn't need the listener
            myThread(&new_fd);
            close(new_fd);
            exit(0);
        }
        close(new_fd); // parent doesn't need this
    }

    return 0;
}