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
#include "ldal.h"

#define SOCKET_SET_REUSEADDR     (SOCKET_CMD_BASE + 0x01)
#define SOCKET_BINDTODEVICE      (SOCKET_CMD_BASE + 0x02)
#define SOCKET_SET_NETMASK       (SOCKET_CMD_BASE + 0x03)
#define SOCKET_GET_RECVADDR      (SOCKET_CMD_BASE + 0x04)
#define SOCKET_SET_KEEPALIVE     (SOCKET_CMD_BASE + 0x05)
#define SOCKET_SET_RECVTIMEO     (SOCKET_CMD_BASE + 0x06)  /* set recv timeout (unit: ms) */
#define SOCKET_SET_SENDTIMEO     (SOCKET_CMD_BASE + 0x07)  /* set send timeout (unit: ms) */
#define SOCKET_SET_ETHDEV        (SOCKET_CMD_BASE + 0x08)
#define SOCKET_SET_ECHO_FLAG     (SOCKET_CMD_BASE + 0x09)

#define SOCKET_BINDTOCONNECT     0x200
#define SOCKET_UNBINDTOCONNECT   0x201
#define SOCKET_BIND              0x202
//#define SOCKET_BINDTODEVICE      0x203
#define SOCKET_ETH0_STATE        0x204
#define SOCKET_ETH1_STATE        0x205
#define SOCKET_PPP0_STATE        0x206
#define SOCKET_CONNECT           0x207
#define SOCKET_DISCONNECT        0x208
#define LINK_READ_TIMEOUT        0x209
#define SOCKET_CHECK_LINK        0x20a

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

/*
struct netdev_cfg {
    char eth_name[6];
    union {
        char ipaddr[16];
        char gateway[16];
        char netmask[16];
        char dns[16];
    } eth_configs;
};
*/

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
    PLATFORM_COUNTRY  = 0,       /* 国家平台 */
    PLATFORM_PROVINCE = 1,       /* 省平台 */
    PLATFORM_CITY     = 2,       /* 市平台 */
    PLATFORM_AREA     = 3,       /* 区平台 */
    PLATFORM_COMPANY  = 4,       /* 企业平台 */
    PLATFORM_OPERATOR = 5,       /* 运营商平台 */
} platform_t;                    /* 上报平台类型 */

struct server_cfg
{
    char ipaddr[100];            /* 平台 IP */
    unsigned int port;           /* 中心端口 */
    char password[25];           /* 访问密码 */
    int connect_style;//  eth0=1   eth2=2   ppp0=3
//    char netdev_name[10];//"eth0"  "eth1"  "ppp0"
    platform_t platform_type;    /* 平台类型 */
    int alarm_period;
    //char data_report_tm[2];//[0]: hour  , [1]:minute
    //int rtdata_report_interval;
    int report_tm[2];
    int send_period;             /* 实时发送周期 */
    int hearbeat_period;         /* 心跳周期 */
    int time_out;                /* 超时时间 */
    int resend_count;            /* 重发次数 */
    char MN[MN_LENGTH];          /* MN 号码：联网许可身份设备唯一标识 */
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
    char *device_name;
    char *file_name;
    int status;

    struct ldal_device device;

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