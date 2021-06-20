#ifndef __LDAL_DEVICE_SOCKET_H__
#define __LDAL_DEVICE_SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "ldal.h"

typedef enum {
    TCP_CLIENT,
    TCP_SERVER,
    LINK_TYPE_LEN,
}link_type;

static char linktype[LINK_TYPE_LEN+1][20]={
    "tcp client",
    "tcp server",
};

typedef enum LINK_STAT {
    LINK_INITIAL,
    LINK_CONNECT,
    LINK_CONNECT_ERR,
    LINK_DISCONNECT,
    LINK_DISCONNECT_ERR,
    LINK_END,
    LINK_STAT_LEN,
}LINK_STAT;

static char linkstat[LINK_STAT_LEN+1][20] = {
    "initial",
    "connect",
    "connect error",
    "disconnect",
    "disconnect error",
    "end"
};

struct ethernet {
    char *eth_name;
    pthread_mutex_t mutex_ether;
};

struct netdev_cfg {
    char eth_name[6];
    union {
        char ipaddr[16];
        char gateway[16];
        char netmask[16];
        char dns[16];
    } eth_configs;
};

struct modbusTcp_cfg {
    int sockfd;
    struct sockaddr_in client;
};

struct eth_link{
        char *name;
        //char * eth_name;
        struct ethernet *ethernet;
        pthread_mutex_t mutex_ether;
        const link_type ltype;
        int fd;
        void *priv;
        int pos;
        struct ldal_device dev;
        int (*config)(struct eth_link *eth_link,unsigned int cmd,unsigned long arg);
        struct sockaddr_in local;
        int stat_local;
        struct sockaddr_in server;
        int stat_server;
        LINK_STAT stat;
        unsigned long timeout;/*read timeout*/
        int keepalive;
        int keepidle;
        int keepinterval;
        int keepcount;
        int signal_strength;//0~100%
};

struct eth_dev{
        struct eth_link *link ;
        struct device_ops *eth_ops;
        int link_n;
};

typedef enum link_config_cmd{
        LINK_COMM_START,
        LINK_COMM_STOP
}link_cfg_cmd;

typedef enum {
    PLATFORM_COUNTRY = 0,
    PLATFORM_PROVINCE,
    PLATFORM_CITY,
    PLATFORM_AREA,
    PLATFORM_COMPANY,
    PLATFORM_OPERATOR,
} platform_t;

struct server_cfg{
    char ipaddr[100];
    unsigned int port;
    char password[25];
    int connect_style;//  eth0=1   eth2=2   ppp0=3
//    char netdev_name[10];//"eth0"  "eth1"  "ppp0"
    int platform_type;//0:country 1:province 2:city 3:area 4:company 5:operator
    int alarm_period;
    //char data_report_tm[2];//[0]: hour  , [1]:minute
    //int rtdata_report_interval;
    int report_tm[2];
    int send_period;
    int hearbeat_period;
    int time_out;
    int resend_count;
    char MN[25];
};

#define DEV_NET0 "eth0"
#define DEV_NET1 "eth1"
#define DEV_NET2 "ppp0"

typedef enum{
    NETDEV_DOWN,
    NETDEV_UP,
    NETDEV_NONEXIST,
}NETDEV_DETECT;

/* LDAL device */

struct ldal_socket_device
{
    char *device_name;
    char *file_name;
    int status;

    struct ldal_device device;
    void *user_data;
};

int socket_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_SOCKET_H__ */