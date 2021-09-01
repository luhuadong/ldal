#ifndef __LDAL_DEVICE_BACKLIGHT_H__
#define __LDAL_DEVICE_BACKLIGHT_H__

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

#define USR_BACKLIGHT_MIN    0
#define USR_BACKLIGHT_MAX    100
#define SYS_BACKLIGHT_MIN    20
#define SYS_BACKLIGHT_MAX    255

struct ldal_backlight_device
{
    struct ldal_device device;
    int status;
    int min;
    int max;
    
    void *user_data;
};

int backlight_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_BACKLIGHT_H__ */