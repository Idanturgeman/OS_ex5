#include "Ex5.hpp"

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
            int k = 0;
            for (int i = 5, j = 0; i < strlen(input); i++, j++)
            {
                k++;
                str[j] = input[i];
            }
            numOfPush++;
            push(str, stack1);
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
        int byt = 0;
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
    int saved_errno = errno;
    int sg = 0;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

int main(void)
{
    createFile();
    int numOfFiles = 0;
    stack1 = (pmyStack)mmap(0, 2000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANON, -1, 0);
    stack1->data[0] = '\0';
    int ans = 0;
    stack1->top = 0;
    int sockfd, new_fd; 
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; 
    int temp = 0;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int no = 1;

    memset(&hints, 0, sizeof hints);
    int set = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 
    int ai = 0;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        ai++;
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    int srv = 0;
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

    freeaddrinfo(servinfo); 
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

    sa.sa_handler = sigchld_handler; 
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
    { 
        int ans = 0;
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
        int num = 0;
        printf("server: got connection from %s\n", s);

        if (!fork())
        { 
            int f = 0;
            i++;
            close(sockfd); 
            myThread(&new_fd);
            close(new_fd);
            exit(0);
        }
        close(new_fd); 
    }

    return 0;
}