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

#define IP_ADDR_SIZE_MAX    16
#define CCID_SIZE_MAX       20
#define IMSI_SIZE_MAX       16  /* 15+1 */
#define IMEI_SIZE_MAX       16  /* 15+1 */
#define ME_NAME_MAX         16

struct me_info
{
    char model[ME_NAME_MAX];        /* Model identification */
    char vendor[ME_NAME_MAX];       /* Manufacturer identification */
    char ccid[CCID_SIZE_MAX];       /* Integrate circuit card identity */
    char imsi[IMSI_SIZE_MAX];       /* International Mobile Subscriber Identity */
    char imei[IMEI_SIZE_MAX];       /* International Mobile Equipment Identity */
    char ipaddr[IP_ADDR_SIZE_MAX];
    char dns1[IP_ADDR_SIZE_MAX];
    char dns2[IP_ADDR_SIZE_MAX];
    uint8_t sq;                     /* Signal quality 0-99 */
};

struct ldal_me_device
{
    char *device_name;
    char *file_name;
    at_status_t status;
    at_resp_status_t resp_status;

    /* the current received one line data buffer */
    char *recv_line_buf;
    /* The length of the currently received one line data */
    size_t recv_line_len;

    pthread_mutex_t lock;
    at_response_t resp;
    sem_t resp_notice;
    pthread_t parser;
    struct me_info info;  /* SIM card information */

    struct ldal_device device;
    void *user_data;
};

int me_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_ME_H__ */