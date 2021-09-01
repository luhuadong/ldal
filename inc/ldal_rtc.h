#ifndef __LDAL_DEVICE_RTC_H__
#define __LDAL_DEVICE_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "ldal_private.h"
#include "ldal.h"

#define LDAL_RTC_TIME_SIZE    7

struct ldal_rtc_time {
    int sec;
    int min;
    int hour;
    int mday;
    int mon;
    int year;
    int wday;
};

struct ldal_rtc_device
{
    struct ldal_device device;
    int status;

    void *user_data;
};

int rtc_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_RTC_H__ */