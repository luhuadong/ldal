#ifndef __LDAL_DEVICE_ME_H__
#define __LDAL_DEVICE_ME_H__

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
#include <semaphore.h>
#include "ldal.h"
#include "at.h"


struct ldal_me_device
{
    char *device_name;
    char *file_name;
    at_status_t status;
    at_resp_status_t resp_status;
    uint8_t sq;                /* Signal quality 0-99 */
    pthread_mutex_t lock;
    at_response_t resp;
    sem_t resp_notice;

    struct ldal_device device;
    void *user_data;
};

int me_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_ME_H__ */