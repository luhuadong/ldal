#ifndef __LDAL_DEVICE_DIGITAL_H__
#define __LDAL_DEVICE_DIGITAL_H__

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
#include "ldal.h"


struct ldal_digital_device
{
    struct ldal_device device;
    int status;

    void *user_data;
};

int digital_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_DIGITAL_H__ */