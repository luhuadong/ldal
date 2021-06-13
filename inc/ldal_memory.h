#ifndef __LDAL_DEVICE_MEMORY_H__
#define __LDAL_DEVICE_MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "ldal.h"

struct ldal_memory_device
{
    char *device_name;
    char *file_name;
    int status;

    struct ldal_device device;
    void *user_data;

    /* custom ops */
    int (*open)(void);
    int (*close)(void);
    int (*read)(char *buf, int len);
    int (*write)(char *buf, int len);
    int (*control)(int cmd, void *arg);
    int (*show)(void);
};

int memory_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_SERIAL_H__ */