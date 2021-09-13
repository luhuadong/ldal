#ifndef __LDAL_DEVICE_MISC_H__
#define __LDAL_DEVICE_MISC_H__

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
#include "ldal_private.h"
#include "ldal.h"

#define GET_K37A_POWER_SUPPLY        _IOW('H',2,int)
#define GET_K37A_BATTERY_CHARGE      _IOW('H',3,int)

struct ldal_misc_device
{
    struct ldal_device device;
    int status;

    void *user_data;
};

int misc_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_MISC_H__ */
