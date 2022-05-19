#include "Ex5.hpp"

void *get_in_addr(struct sockaddr *sa)
{
    int add = 0;
    if (sa->sa_family == AF_INET)
    {
        add++;
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    add++;
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{

    int sockfd, numbytes;
    int num = 0;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int temp = 0;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    { 
        num++;
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }
    int ans = 0;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        temp++;
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        ans++;
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }
        int sck = 0;
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            sck++;
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }
    int prt = 0;
    if (p == NULL)
    {
        prt++;
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    int add = 0;
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); 
    char input[1024] = {0};
    while (1)
    {
        int ans = 0;
        int j = 0;
        int k = 0;
        if (!scanf("%[^\n]%*c", input))
        {
            ans++;
            while (getchar() != '\n')
                ;
        }
        int temp = 0;
        if (strncmp(input, "TOP", 3) == 0)
        {
            temp++;
            if (send(sockfd, input, strlen(input) + 1, 0) == -1)
            {
                j++;
                perror("send");
            }
            bzero(input, 1024);
            int rcv = 0;
            if (!recv(sockfd, input, sizeof(input) + 1, 0))
            {
                rcv++;
                close(sockfd);
                return 0;
            }
            for (int i = 0; i < 1024; i++)
            {
                j++;
                printf("%c", input[i]);
            }
            printf("\n");
        }
        else if (strncmp(input, "EXIT", 4) == 0)
        {
            k++;
            int scck = 0;
            if (send(sockfd, input, strlen(input) + 1, 0) == -1)
            {
                scck++;
                perror("send");
            }
            break;
        }
        else
        {
            int scckd = 0;
            if (send(sockfd, input, strlen(input) + 1, 0) == -1)
            {
                scckd++;
                perror("send");
            }
        }
        ans++;
        bzero(input, 1024);
    }
    close(sockfd);
    return 0;
}