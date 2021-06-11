#ifndef __LDAL_H__
#define __LDAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include "ldal_config.h"
#include "list.h"

#define LDAL_CLASS_SERIAL               0x01U
#define LDAL_CLASS_GPIO                 0x02U
#define LDAL_CLASS_RTC                  0x03U
#define LDAL_CLASS_MAX                  0x04U

#define LDAL_CTRL_POWER_ON              0x01L
#define LDAL_CTRL_POWER_OFF             0x01L
#define LDAL_CTRL_POWER_RESET           0x01L

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

struct ldal_device;

/* Device operations */
struct ldal_device_ops
{
    int (*init)(struct ldal_device *device);
    int (*deinit)(struct ldal_device *device);
    int (*open)(struct ldal_device *device);
    int (*close)(struct ldal_device *device);
    int (*read)(struct ldal_device *device, char *buf, size_t len);
    int (*write)(struct ldal_device *device, char *buf, size_t len);
    int (*control)(struct ldal_device *device, int cmd, void *arg);
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

int startup_device(const char *dev_name);
int stop_device(const char *dev_name);
int read_device(const char *dev_name, char *buff, int len);
int write_device(const char *dev_name, char *buff, int len);
int control_device(const char *dev_name, int cmd, void *arg);
int config_device(const char *dev_name, int cmd, void *arg);
int read_device_ai_src_value(const char *dev_name, float *value);

struct ldal_device *ldal_device_get_by_name(const char *name);
//struct ldal_device *ldal_device_get_by_name(int type, const char *name);

/* Register device class object */
int ldal_device_class_register(struct ldal_device_class *class, uint16_t class_id);
/* Register device object */
int ldal_device_register(struct ldal_device *device, const char *devname, const char *filename, uint16_t class_id, void *user_data);


#ifdef __cplusplus
}
#endif

#endif /* __LDAL_H__ */