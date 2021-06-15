/*
 * Copyright (c) 2020, RudyLo <luhuadong@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-12     RudyLo       the first version
 */

#ifndef __PMSXX_H__
#define __PMSXX_H__

#include <stdint.h>

#define PMSLIB_VERSION       "0.2.0"
#define PKG_USING_PMSXX_ENHANCED
#define PKG_USING_PMSXX_DEBUG_DUMP_RESP
#define PKG_USING_PMSXX_DEBUG_SHOW_RESP

/* Custom sensor control cmd types */
#define  RT_SENSOR_CTRL_PMS_STANDBY              (0x110)   /* Standby mode (no output) */
#define  RT_SENSOR_CTRL_PMS_NORMAL               (0x111)   /* Normal  mode */
#define  RT_SENSOR_CTRL_PMS_ACTIVE               (0x112)   /* Active output mode */
#define  RT_SENSOR_CTRL_PMS_PASSIVE              (0x113)   /* Passive output mode */
#define  RT_SENSOR_CTRL_PMS_DUMP                 (0x114)   /* Dump all response */

#define PMS_READ_WAIT_TIME                       (3000)

#define FRAME_LEN_MIN                            (32)
#define FRAME_LEN_MAX                            (40)

#define FRAME_START1                             (0x42)
#define FRAME_START2                             (0x4d)

typedef enum
{
    PMS_FRAME_HEAD,
    PMS_FRAME_HEAD_ACK,
    PMS_FRAME_LENGTH,
    PMS_FRAME_PAYLOAD,
    PMS_FRAME_END

} pms_frame_t;

typedef enum
{
    PMS_MODE_READ = 0,
    PMS_MODE_PASSIVE,
    PMS_MODE_ACTIVE,
    PMS_MODE_STANDBY,
    PMS_MODE_NORMAL

} pms_mode_t;

struct pms_cmd
{
    uint8_t  START1;
    uint8_t  START2;
    uint8_t  CMD;
    uint8_t  DATAH;
    uint8_t  DATAL;
    uint8_t  LRCH;
    uint8_t  LRCL;
};
typedef struct pms_cmd *pms_cmd_t;

struct pms_response
{
    uint8_t  start1;
    uint8_t  start2;
    uint16_t length;

    uint16_t PM1_0_CF1;
    uint16_t PM2_5_CF1;
    uint16_t PM10_0_CF1;
    uint16_t PM1_0_atm;
    uint16_t PM2_5_atm;
    uint16_t PM10_0_atm;
    uint16_t air_0_3um;
    uint16_t air_0_5um;
    uint16_t air_1_0um;
    uint16_t air_2_5um;
    uint16_t air_5_0um;
    uint16_t air_10_0um;

#ifdef PKG_USING_PMSXX_ENHANCED
    uint16_t hcho;
    uint16_t temp;
    uint16_t humi;
    uint16_t reserve;
#endif

    uint8_t  version;
    uint8_t  errorCode;
    uint16_t checksum;
};
typedef struct pms_response *pms_response_t;


uint16_t  pms_read(ldal_device_t *dev, void *buf, uint16_t size, int32_t time);
uint16_t  pms_wait(ldal_device_t *dev, void *buf, uint16_t size);
int       pms_set_mode(ldal_device_t *dev, pms_mode_t mode);
bool      pms_is_ready(ldal_device_t *dev);

void         pms_show_command(pms_cmd_t cmd);
void         pms_show_response(pms_response_t resp);
void         pms_dump(const char *buf, uint16_t size);

#endif /* __PMSXX_H__ */
