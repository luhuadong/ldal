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


#ifdef LDAL_GLOBALS
#define LDAL_EXT
#else
#define LDAL_EXT                        extern
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_H__ */