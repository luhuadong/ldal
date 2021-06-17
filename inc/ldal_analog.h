#ifndef __LDAL_DEVICE_ANALOG_H__
#define __LDAL_DEVICE_ANALOG_H__

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


#define AI_SET_CURR_VOL_VALMAX_VALMIN 0x101
#define AI_SET_CORRECT_VOL_CUR        0x102
#define AI_GET_CORRECT_VOL_CUR        0x103


struct ldal_analog_device
{
    char *device_name;
    char *file_name;
    int status;

    struct ldal_device device;
    void *user_data;
};

int analog_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_ANALOG_H__ */