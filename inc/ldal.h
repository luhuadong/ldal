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
#include "list.h"

#define LDAL_VERSION             "0.0.1"
#define LDAL_NAME_MAX            32
#define LDAL_FILENAME_MAX        64
#define NETDEV_NAME_MAX          16

#ifdef LDAL_GLOBALS
#define LDAL_EXT
#else
#define LDAL_EXT                 extern
#endif

#define debug                    1  /* enable this to printf */
#define DEBUG_PRINT(fmt, args...) \
    do { if(debug) \
    printf(fmt, ## args); \
    } while(0)

#define LOG_D(...)               printf(__VA_ARGS__);
#define LOG_E(...)               printf(__VA_ARGS__);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)          (sizeof(arr) / sizeof((arr)[0]))
#endif

#define SOCKET_CMD_BASE          0x1000
#define NETWORK_CMD_BASE         0x2000
/*
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
*/


#ifndef MN_LENGTH
#define MN_LENGTH                (25)
#endif

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
    LDAL_CLASS_BACKLIGHT,
    LDAL_CLASS_MAX,
} ldal_class_t;

typedef enum {
    INIT_STATE,          /* Begining state */
    ESTABLISHED_STATE,   /* The socket has created */
    UNCONNECTED_STATE,   /* The socket is not connected */
    HOST_LOOKUP_STATE,   /* The socket is performing a host name lookup */
    CONNECTING_STATE,    /* The socket has started establishing a connection */
    CONNECTED_STATE,     /* A connection is established */
    BOUND_STATE,         /* The socket is bound to an address and port */
    LISTENING_STATE,     /* For internal use only (for server) */
    CLOSING_STATE,       /* The socket is about to close (data may still be waiting to be written) */
    CLOSED_STATE,        /* The socket is closed */
} socket_state_t;

#define LDAL_CTRL_POWER_ON                0x01L
#define LDAL_CTRL_POWER_OFF               0x02L
#define LDAL_CTRL_POWER_RESET             0x03L

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

#define AUTO_REGISTER
#ifdef AUTO_REGISTER
#define INIT_CLASS_EXPORT(fn)  int fn (void) __attribute__ ((constructor));
#else
#define INIT_CLASS_EXPORT(fn)
#endif

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
    int (*check)(struct ldal_device *device);
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
    char filename[LDAL_FILENAME_MAX];            /* Device file name */
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

struct ldal_device_table
{
    char *device_name;
    char *file_name;
    ldal_class_t class_id;
    void *private_data;
};

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
int check_status(struct ldal_device *dev);

/* socket special functions  */
int bind_local_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port);
int connect_server_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port);

/* Find ldal device object */
struct ldal_device *ldal_device_get_by_name(const char *name);
struct ldal_device *ldal_device_get_by_type(const int type, const char *name);
struct ldal_device *ldal_device_get_object_by_name(const char *name);

/* Device objects initialize */
int ldal_device_create(struct ldal_device_table *table, const size_t size);

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
bool get_local_gateway(const char *ifname, char* gateway);
bool get_local_dns(const char *ifname, char* dns_addr);

bool set_local_mac(const char *ifname, const char *mac);
bool set_local_ip(const char *ifname, const char *ip);
bool set_local_netmask(const char *ifname, const char *netmask_addr);
bool set_local_gateway(const char *ifname, const char *gateway);
bool set_local_dns(const char *ifname, const char* dns_addr);

int ldal_get_ip_attr(const char *ifname, netdev_attr_t *attr);
int ldal_set_ip_attr(const char *ifname, const netdev_attr_t *attr);

typedef enum {
    LINK_UP = 0,
    LINK_DOWN
} link_status_t;

int set_netdev_status(const char *ifname, const link_status_t status);
link_status_t check_netdev_status(const char *ifname);

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
#include "ldal_backlight.h"

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_H__ */