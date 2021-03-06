#ifndef __LDAL_DEVICE_TCP_H__
#define __LDAL_DEVICE_TCP_H__

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
#include "ldal_private.h"
#include "ldal.h"

/* #define CONFIG_SOCKET_SET_NONBLOCK */
#define CONFIG_SOCKET_SET_REUSE
#define CONFIG_SOCKET_SET_TIMEOUT

#ifdef CONFIG_SOCKET_SET_TIMEOUT
#define SOCKET_DEFAULT_TIMEOUT   3000 /* ms */
#endif

/* seconds : frequency of sending keep alive packets after first error keepalive respone */
#define CONFIG_TCP_KEEPINTVL     20
/* seconds : if no packets transmittion of Tx Rx in TCP after such seconds,send keep alive packets */
#define CONFIG_TCP_KEEPIDLE      120
/* total counts of probe connecttion */
#define CONFIG_TCP_KEEPCNT       2

#define SOCKET_SET_REUSEADDR     (SOCKET_CMD_BASE + 0x01)
#define SOCKET_SET_NETDEV        (SOCKET_CMD_BASE + 0x02)
#define SOCKET_SET_KEEPALIVE     (SOCKET_CMD_BASE + 0x03)
#define SOCKET_SET_RECVTIMEO     (SOCKET_CMD_BASE + 0x04)  /* set recv timeout (unit: ms) */
#define SOCKET_SET_SENDTIMEO     (SOCKET_CMD_BASE + 0x05)  /* set send timeout (unit: ms) */
#define SOCKET_SET_ECHO_FLAG     (SOCKET_CMD_BASE + 0x06)
#define SOCKET_GET_RECVADDR      (SOCKET_CMD_BASE + 0x07)
#define SOCKET_SET_NONBLOCK      (SOCKET_CMD_BASE + 0x08)

typedef enum {
    TCP_CLIENT,
    TCP_SERVER,
    LINK_TYPE_LEN,
} link_type;

typedef enum LINK_STAT {
    LINK_INITIAL,
    LINK_CONNECT,
    LINK_CONNECT_ERR,
    LINK_DISCONNECT,
    LINK_DISCONNECT_ERR,
    LINK_END,
    LINK_STAT_LEN,
} LINK_STAT;

struct ethernet {
    char *eth_name;
    pthread_mutex_t mutex_ether;
};

struct modbusTcp_cfg {
    int sockfd;
    struct sockaddr_in client;
};

struct eth_link {
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

struct eth_dev
{
    struct eth_link *link ;
    struct device_ops *eth_ops;
    int link_n;
};

typedef enum link_config_cmd {
    LINK_COMM_START,
    LINK_COMM_STOP
} link_cfg_cmd;


typedef enum {
    PLATFORM_COUNTRY  = 0,       /* ???????????? */
    PLATFORM_PROVINCE = 1,       /* ????????? */
    PLATFORM_CITY     = 2,       /* ????????? */
    PLATFORM_AREA     = 3,       /* ????????? */
    PLATFORM_COMPANY  = 4,       /* ???????????? */
    PLATFORM_OPERATOR = 5,       /* ??????????????? */
} platform_t;                    /* ?????????????????? */

struct server_cfg
{
    char ipaddr[100];            /* ?????? IP */
    unsigned int port;           /* ???????????? */
    char password[25];           /* ???????????? */
    int connect_style;//  eth0=1   eth2=2   ppp0=3
//    char netdev_name[10];//"eth0"  "eth1"  "ppp0"
    platform_t platform_type;    /* ???????????? */
    int alarm_period;
    //char data_report_tm[2];//[0]: hour  , [1]:minute
    //int rtdata_report_interval;
    int report_tm[2];
    int send_period;             /* ?????????????????? */
    int hearbeat_period;         /* ???????????? */
    int time_out;                /* ???????????? */
    int resend_count;            /* ???????????? */
    char MN[MN_LENGTH];          /* MN ????????????????????????????????????????????? */
};

#define DEV_NET0 "eth0"
#define DEV_NET1 "eth1"
#define DEV_NET2 "ppp0"

typedef enum {
    NETDEV_UP = 0,
    NETDEV_DOWN,
    NETDEV_NONEXIST,
} NETDEV_DETECT;

/* LDAL device */

struct ldal_tcp_device
{
    struct ldal_device device;
    int status;

    struct sockaddr_in local;     /* local addr */
    struct sockaddr_in server;    /* server addr */

    uint32_t recv_timeout;
    uint32_t send_timeout;
    int keepalive;
    int keepidle;
    int keepinterval;
    int keepcount;
    int signal_strength;          /* 0~100% */

    void *user_data;
};

int tcp_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_TCP_SOCKET_H__ */