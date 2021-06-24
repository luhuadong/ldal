/* **************************************************************************
 * 
 * device      file              type              ref   comment
 * ----------  ----------------  ----------------  ----  --------------------
 * serial0     /dev/ttyUSB0      Serial Device     1
 * client0     ppp0              TCP Socket        1     120.78.197.79:1601
 * client1     eth0              TCP Socket        1     113.31.117.114:8080
 * client2     eth1              UDP Socket        1     42.192.64.149:5566
 * rtc0        /dev/rtc0         RTC Device        2
 * 
*/

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
#define DEFAULT_PORT    1601
#define JSON_CMD        "{\"method\":\"listAllDevice\"}"

struct option longopts[] = {
    {"help",   no_argument,       NULL, 'h'},
    {"ipaddr", required_argument, NULL, 'a'},
    {"port",   required_argument, NULL, 'p'},
    {0, 0, 0, 0}
};

void show_usage(char *cmd)
{
    printf("List all devices registered in ldal framework.\n");
    printf("Usage: list_device [-a ipaddr] [-p port]\n", cmd);
    printf("OPTIONS\n");
    printf("  -a, --ipaddr  Specify server ip address, default localhost.\n");
    printf("  -p, --port    Specify server port, default %d.\n", DEFAULT_PORT);
    printf("  -h, --help    Display help and exit.\n\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int sockfd, c;
    socklen_t len;
    struct sockaddr_in serv_addr;
    uint16_t serv_port;
    char buf[MAXBUF];

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    while((c = getopt_long(argc, argv, "ha:p:", longopts, NULL)) != -1) {
        switch(c) {
        case 'a':
            serv_addr.sin_addr.s_addr = inet_addr(optarg);
            break;
        case 'p':
            serv_addr.sin_port = htons(atoi(optarg));
            break;
        case 'h':
            show_usage(argv[0]);
            break;
        default: break;
        }
    }

    printf("ip: %s, port: %d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(-EXIT_FAILURE);
    }

    int optval = 1;
    int optlen = sizeof(optval);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
        goto __exit;
    }

    int datalen;

    printf("Connected!\n");

    datalen = write(sockfd, JSON_CMD, strlen(JSON_CMD)+1);
    if (datalen < 0) {
        error("ERROR writing to socket");
        goto __exit;
    }
         
    bzero(buf, MAXBUF);
    datalen = read(sockfd, buf, sizeof(buf));
    if (datalen < 0) {
        error("ERROR reading from socket");
        goto __exit;
    }
    printf("%s\n", buf);

__exit:
    close(sockfd);
    return 0;
}
