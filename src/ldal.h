#ifndef __LDAL_H__
#define __LDAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ldal_config.h"

#define LDAL_CLASS_SERIAL       0x01U
#define LDAL_CLASS_GPIO         0x02U
#define LDAL_CLASS_RTC          0x03U

#define LDAL_CTRL_POWER_ON      0x01L
#define LDAL_CTRL_POWER_OFF     0x01L
#define LDAL_CTRL_POWER_RESET   0x01L

struct ldal_device
{
    char *name[LDAL_DEVICE_NAME_MAX];
    int fd;
};

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_H__ */