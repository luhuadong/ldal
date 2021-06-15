/*
 * Copyright (c) 2020, RudyLo <luhuadong@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-04     luhuadong    the first version
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "ldal_serial.h"
#include "pmsxx.h"

#define PMSXX_UART_NAME     "uart3"

/* cat_pmsxx_passive */
static void cat_pmsxx_passive(void)
{
    uint16_t  loop = 10;
    struct pms_response resp;
    pms_device_t sensor = pms_create(PMSXX_UART_NAME);
    if (!sensor)
    {
        printf("(PMS) Init failed\n");
        return;
    }

    while (!pms_is_ready(sensor))
    {
        sleep(1);
    }

    pms_set_mode(sensor, PMS_MODE_NORMAL);
    pms_set_mode(sensor, PMS_MODE_PASSIVE);

    while (loop--)
    {
        printf("\n[%d] Request...\n", loop);
        pms_read(sensor, &resp, sizeof(resp), rt_tick_from_millisecond(3000));
        pms_show_response(&resp);
        sleep(3);
    }
    
    printf("(PMS) Finished!\n");
    pms_delete(sensor);
}

/* cat_pmsxx_active */
static void cat_pmsxx_active(void)
{
    struct pms_response resp;
    pms_device_t sensor = pms_create(PMSXX_UART_NAME);
    if (!sensor)
    {
        printf("(PMS) Init failed\n");
        return;
    }

    while (!pms_is_ready(sensor))
    {
        sleep(1);
    }

    pms_set_mode(sensor, PMS_MODE_NORMAL);
    pms_set_mode(sensor, PMS_MODE_ACTIVE);

    while (1)
    {
        pms_wait(sensor, &resp, sizeof(resp));
        pms_show_response(&resp);
    }
    
    printf("(PMS) Finished!\n");
    pms_delete(sensor);
}
