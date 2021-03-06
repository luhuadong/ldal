#ifndef __LDAL_DEVICE_FILE_H__
#define __LDAL_DEVICE_FILE_H__

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

#define USING_LIBC_FILE

struct ldal_file_device
{
    struct ldal_device device;
    int status;

    void *user_data;
};

int file_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_FILE_H__ */