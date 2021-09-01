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
    struct ldal_device device;
    int status;

    void *user_data;
};

int memory_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_MEMORY_H__ */