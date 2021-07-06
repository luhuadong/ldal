#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>

#define MAXBUF          (8*1024)
#define DEFAULT_PORT    7575

struct option longopts[] = {
    {"help",   no_argument,       NULL, 'h'},
    {"ipaddr", required_argument, NULL, 'a'},
    {"port",   required_argument, NULL, 'p'},
    {0, 0, 0, 0}
};

void show_usage(char *cmd)
{
    printf("This is an UDP echo server\n");
    printf("Usage: %s [-a ipaddr] [-p port]\n", cmd);
    printf("OPTIONS\n");
    printf("  -a, --ipaddr  Specify IP address, default INADDR_ANY.\n");
    printf("  -p, --port    Specify IP port, default %d.\n", DEFAULT_PORT);
    printf("  -h, --help    Display help and exit.\n\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int sockfd, c;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    uint16_t myport;
    char buf[MAXBUF];

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    while((c = getopt_long(argc, argv, "ha:p:", longopts, NULL)) != -1) {
        switch(c) {
        case 'a':
            my_addr.sin_addr.s_addr = inet_addr(optarg);
            break;
        case 'p':
            my_addr.sin_port = htons(atoi(optarg));
            break;
        case 'h':
            show_usage(argv[0]);
            break;
        default: break;
        }
    }

    printf("ip: %s, port: %d\n", inet_ntoa(my_addr.sin_addr), ntohs(my_addr.sin_port));
    
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(-EXIT_FAILURE);
    }

    int optval = 1;
    int optlen = sizeof(optval);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);

    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("listen");
        exit(-EXIT_FAILURE);
    }

    int datalen;
    len = sizeof(their_addr);

    printf("Establish...\n");

    while(1)
    {
        bzero(buf, MAXBUF);
        datalen = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&their_addr, &len);
        if(datalen > 0) {
            printf("recv from %s:%d > %s\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), buf);
            datalen = sendto(sockfd, buf, strlen(buf)+1, 0, (struct sockaddr *)&their_addr, len);
            printf("send to ...\n");
        }
    }

    close(sockfd);
    return 0;
}
