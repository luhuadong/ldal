#ifndef __LDAL_H__
#define __LDAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "ldal_config.h"
#include "list.h"

#ifdef LDAL_GLOBALS
#define LDAL_EXT
#else
#define LDAL_EXT           extern
#endif

#define debug              1  /* enable this to printf */
#define DEBUG_PRINT(fmt, args...) \
    do { if(debug) \
    printf(fmt, ## args); \
    } while(0)

#define LOG_D(...)         printf(__VA_ARGS__);
#define LOG_E(...)         printf(__VA_ARGS__);

#define SOCKET_CMD_BASE          0x1000
#define SOCKET_SET_REUSEADDR     (SOCKET_CMD_BASE + 0x01)
#define SOCKET_BINDTODEVICE      (SOCKET_CMD_BASE + 0x02)
#define SOCKET_SET_NETMASK       (SOCKET_CMD_BASE + 0x03)
#define SOCKET_GET_RECVADDR      (SOCKET_CMD_BASE + 0x04)
#define SOCKET_SET_ECHO_FLAG     (SOCKET_CMD_BASE + 0x05)

#define NETWORK_CMD_BASE         0x2000
#define NETWORK_CFG_SERVER       (NETWORK_CMD_BASE + 0x01)
#define NETWORK_CFG_KEEPALIVE    (NETWORK_CMD_BASE + 0x02)
#define NETWORK_CFG_ETHDEV       (NETWORK_CMD_BASE + 0x03)
#define NETWORK_CFG_IPADDR       (NETWORK_CMD_BASE + 0x04)
#define NETWORK_CFG_NETMASK      (NETWORK_CMD_BASE + 0x05)
#define NETWORK_CFG_GATEWAY      (NETWORK_CMD_BASE + 0x06)
#define NETWORK_DEL_GATEWAY      (NETWORK_CMD_BASE + 0x07)
#define NETWORK_CFG_DNS          (NETWORK_CMD_BASE + 0x08)
#define NETWORK_GET_ETH0_MAC     (NETWORK_CMD_BASE + 0x09)
#define NETWORK_GET_ETH1_MAC     (NETWORK_CMD_BASE + 0x0a)
#define NETWORK_CFG_ETH0_MAC     (NETWORK_CMD_BASE + 0x0b)
#define NETWORK_CFG_ETH1_MAC     (NETWORK_CMD_BASE + 0x0c)

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
    LDAL_CLASS_MEMORY = 0,
    LDAL_CLASS_FILE,
    LDAL_CLASS_SERIAL,
    LDAL_CLASS_GPIO,
    LDAL_CLASS_DIGITAL,
    LDAL_CLASS_ANALOG,
    LDAL_CLASS_RTC,
    LDAL_CLASS_UDP,
    LDAL_CLASS_TCP,
    LDAL_CLASS_ME,
    LDAL_CLASS_MISC,
    LDAL_CLASS_MAX,
} ldal_class_t;

#define LDAL_CTRL_POWER_ON              0x01L
#define LDAL_CTRL_POWER_OFF             0x02L
#define LDAL_CTRL_POWER_RESET           0x03L

/* LDAL error code definitions */
#define LDAL_EOK                          0               /* There is no error */
#define LDAL_ERROR                        1               /* A generic error happens */
#define LDAL_ETIMEOUT                     2               /* Timed out */
#define LDAL_EFULL                        3               /* The resource is full */
#define LDAL_EEMPTY                       4               /* The resource is empty */
#define LDAL_ENOMEM                       5               /* No memory */
#define LDAL_ENOSYS                       6               /* No system */
#define LDAL_EBUSY                        7               /* Busy */
#define LDAL_EIO                          8               /* IO error */
#define LDAL_EINTR                        9               /* Interrupted system call */
#define LDAL_EINVAL                       10              /* Invalid argument */

#define INIT_CLASS_EXPORT(fn)  int fn (void) __attribute__ ((constructor));

struct ldal_device;
LDAL_EXT char *class_label[LDAL_CLASS_MAX];

/* Device operations */
struct ldal_device_ops
{
    int (*init)(struct ldal_device *device);
    int (*deinit)(struct ldal_device *device);
    int (*open)(struct ldal_device *device);
    int (*close)(struct ldal_device *device);
    int (*read)(struct ldal_device *device, void *buf, const size_t len);
    int (*write)(struct ldal_device *device, const void *buf, const size_t len);
    int (*control)(struct ldal_device *device, int cmd, void *arg);
    //int (*config)(struct ldal_device *device, int cmd, void *arg);
    int (*bind)(struct ldal_device *dev, const char *ipaddr, const uint16_t port);
    int (*connect)(struct ldal_device *dev, const char *ipaddr, const uint16_t port);
};

struct ldal_device_class
{
    uint16_t class_id;                           /* Device class ID */
    uint16_t dev_count;                          /* Count of devices */
    const struct ldal_device_ops *device_ops;    /* Device operaiotns */
    struct list_head list;                       /* Device class list */
};

struct ldal_device
{
    int fd;                                      /* File descriptor */
    char name[LDAL_NAME_MAX];                    /* Device name */
    char filename[LDAL_NAME_MAX];                /* Device file name */
    bool is_init;                                /* Device initialization completed */
    uint16_t max_dev_num;                        /* The maximum number of devices */
    uint16_t ref;                                /* The count of owned by threads */
    pthread_mutex_t mutex;
    pthread_mutex_t read_mutex;
    pthread_mutex_t write_mutex;
    struct ldal_device_class *class;             /* Device class object */
    struct list_head list;                       /* Device list */

    void *user_data;                             /* User-specific data */
};
typedef struct ldal_device ldal_device_t;        /* Type for ldal device. */

#define NAME_SIZE    8
#define MAC_SIZE     18
#define IP_SIZE      16
#define MASK_SIZE    24
#define GATEWAY_SIZE 24

struct netdev_attr
{
    char ipaddr[IP_SIZE];
    char netmask[IP_SIZE];
    char gateway[IP_SIZE];
    char dns[IP_SIZE];
};
typedef struct netdev_attr netdev_attr_t;


/* Generic device operations */
int startup_device(struct ldal_device *dev);
int stop_device(struct ldal_device *dev);
int read_device(struct ldal_device *dev, void *buff, const size_t len);
int write_device(struct ldal_device *dev, const void *buff, const size_t len);
int control_device(struct ldal_device *dev, int cmd, void *arg);
int config_device(struct ldal_device *dev, int cmd, void *arg);

/* socket special functions  */
int bind_local_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port);
int connect_server_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port);

/* Find ldal device object */
struct ldal_device *ldal_device_get_by_name(const char *name);
struct ldal_device *ldal_device_get_by_type(const int type, const char *name);
struct ldal_device *ldal_device_get_object_by_name(const char *name);

/* Register and unregister device or class object */
int ldal_device_register(struct ldal_device *dev, const char *devname, const char *filename, ldal_class_t class_id, void *user_data);
int ldal_device_unregister(struct ldal_device *dev);
int ldal_device_class_register(struct ldal_device_class *class, ldal_class_t class_id);

/* Others */
void ldal_show_device_list(void);

/* Monitor serve */
int establish_serve(void);

/* Netdev */
bool get_local_mac(const char *ifname, char *mac);
bool get_local_ip(const char *ifname, char *ip);
bool get_local_netmask(const char *ifname, char* netmask_addr);
bool get_local_gateway(char* gateway);
bool get_local_dns(char* dns_addr);

bool set_local_mac(const char *ifname, const char *mac);
bool set_local_ip(const char *ifname, const char *ip);
bool set_local_netmask(const char *ifname, const char *netmask_addr);
bool set_local_gateway(const char *gateway);
bool set_local_dns(const char* dns_addr);

int ldal_get_ip_attr(const char *ifname, netdev_attr_t *attr);
int ldal_set_ip_attr(const char *ifname, const netdev_attr_t *attr);

#include "ldal_memory.h"
#include "ldal_file.h"
#include "ldal_serial.h"
#include "ldal_gpio.h"
#include "ldal_digital.h"
#include "ldal_analog.h"
#include "ldal_rtc.h"
#include "ldal_udp.h"
#include "ldal_tcp.h"
#include "ldal_me.h"
#include "ldal_misc.h"

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_H__ */