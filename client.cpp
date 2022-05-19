#include "Ex5.hpp"


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); 
    char input[1024] = {0};
    while (1)
    {
        if (!scanf("%[^\n]%*c", input))
        {
            while (getchar() != '\n')
                ;
        }
        if (strncmp(input, "TOP", 3) == 0)
        {
            if (send(sockfd, input, strlen(input) + 1, 0) == -1)
            {
                perror("send");
            }
            bzero(input, 1024);
            if (!recv(sockfd, input, sizeof(input) + 1, 0))
            {
                close(sockfd);
                return 0;
            }
            for (int i = 0; i < 1024; i++)
            {
                printf("%c", input[i]);
            }
            printf("\n");
        }
        else if (strncmp(input, "EXIT", 4) == 0)
        {
            if (send(sockfd, input, strlen(input) + 1, 0) == -1)
            {
                perror("send");
            }
            break;
        }
        else
        {
            if (send(sockfd, input, strlen(input) + 1, 0) == -1)
            {
                perror("send");
            }
        }
        bzero(input, 1024);
    }
    close(sockfd);
    return 0;
}