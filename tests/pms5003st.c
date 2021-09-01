/*
 * Copyright (c) 2020, RudyLo <luhuadong@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-04     luhuadong    the first version
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include "ldal.h"
#include "pmsxx.h"

#define PMS_THREAD_STACK_SIZE          1024
#define PMS_THREAD_PRIORITY            (RT_THREAD_PRIORITY_MAX/2)

#define ntohs(x) ((((x)&0x00ffUL) << 8) | (((x)&0xff00UL) >> 8))

static struct ldal_device_table device_table[] = {
    { "pms", "/dev/ttyUSB0", LDAL_CLASS_SERIAL },
};

struct port_option opt = {
    .baudrate  = B9600,
    .data_bits = 8,
    .parity    = PAR_NONE,
    .stop_bits = BITS_1P0,
    .flowctrl  = 0,
};

static struct pms_cmd preset_commands[] = {
    {0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71}, /* Read in passive mode */
    {0x42, 0x4d, 0xe1, 0x00, 0x00, 0x01, 0x70}, /* Change to passive mode */
    {0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71}, /* Change to active  mode */
    {0x42, 0x4d, 0xe4, 0x00, 0x00, 0x01, 0x73}, /* Change to standby mode */
    {0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74}  /* Change to normal  mode */
};

void pms_show_command(pms_cmd_t cmd)
{
    assert(cmd);

    printf("\n+-----------------------------------------------------+\n");
    printf("| HEAD1 | HEAD2 |  CMD  | DATAH | DATAL | LRCH | LRCL |\n");
    printf(" ----------------------------------------------------- \n");
    printf("|   %02x  |   %02x  |   %02x  |   %02x  |   %02x  |  %02x  |  %02x  |\n", 
            cmd->START1, cmd->START2, cmd->CMD, cmd->DATAH, cmd->DATAL, cmd->LRCH, cmd->LRCL);
    printf("+-----------------------------------------------------+\n");
}

void pms_show_response(pms_response_t resp)
{
    assert(resp);

#ifdef PKG_USING_PMSXX_BASIC
    printf("\nResponse => len: %d bytes, version: %02X, Error: %02X\n", resp->length+4, resp->version, resp->errorCode);
    printf("+-----------------------------------------------------+\n");
    printf("|  CF=1  | PM1.0 = %-4d | PM2.5 = %-4d | PM10  = %-4d |\n", resp->PM1_0_CF1, resp->PM2_5_CF1, resp->PM10_0_CF1);
    printf("|  atm.  | PM1.0 = %-4d | PM2.5 = %-4d | PM10  = %-4d |\n", resp->PM1_0_atm, resp->PM2_5_atm, resp->PM10_0_atm);
    printf("|        | 0.3um = %-4d | 0.5um = %-4d | 1.0um = %-4d |\n", resp->air_0_3um, resp->air_0_5um, resp->air_1_0um);
    printf("|        | 2.5um = %-4d | 5.0um = %-4d | 10um  = %-4d |\n", resp->air_2_5um, resp->air_5_0um, resp->air_10_0um);
    printf("+-----------------------------------------------------+\n");
#endif
#ifdef PKG_USING_PMSXX_ENHANCED
    printf("\nResponse => len: %d bytes, version: %02X, Error: %02X\n", resp->length+4, resp->version, resp->errorCode);
    printf("+-----------------------------------------------------+\n");
    printf("|  CF=1  | PM1.0 = %-4d | PM2.5 = %-4d | PM10  = %-4d |\n", resp->PM1_0_CF1, resp->PM2_5_CF1, resp->PM10_0_CF1);
    printf("|  atm.  | PM1.0 = %-4d | PM2.5 = %-4d | PM10  = %-4d |\n", resp->PM1_0_atm, resp->PM2_5_atm, resp->PM10_0_atm);
    printf("|        | 0.3um = %-4d | 0.5um = %-4d | 1.0um = %-4d |\n", resp->air_0_3um, resp->air_0_5um, resp->air_1_0um);
    printf("|        | 2.5um = %-4d | 5.0um = %-4d | 10um  = %-4d |\n", resp->air_2_5um, resp->air_5_0um, resp->air_10_0um);
    printf("| extra  | hcho  = %-4d | temp  = %-4d | humi  = %-4d |\n", resp->hcho, resp->temp, resp->humi);
    printf("+-----------------------------------------------------+\n");
#endif
}

void pms_dump(const uint8_t *buf, uint16_t size)
{
#ifdef PKG_USING_PMSXX_DEBUG_SHOW_RULER
#ifdef PKG_USING_PMSXX_BASIC
    printf("\n_______________________________________________________________________________________________\n");
    printf("01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32\n");
    printf("-----------------------------------------------------------------------------------------------\n");
#endif
#ifdef PKG_USING_PMSXX_ENHANCED
    printf("\n_______________________________________________________________________________________________________________________\n");
    printf("01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40\n");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
#endif
#endif
    for (uint16_t i = 0; i < size; i++)
    {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

/**
 * Receive callback function
 */
#if 0
static int pms_uart_input(ldal_device_t dev, size_t size)
{
    assert(dev);
    pms_device_t pms = (pms_device_t)dev->user_data;

#ifdef PKG_USING_PMSXX_UART_DMA
    if (pms) rt_mb_send(pms->rx_mb, size);
#else
    if (pms) rt_sem_release(pms->rx_sem);
#endif

    return RT_EOK;
}
#endif

static int pms_check_frame(ldal_device_t *dev, const uint8_t *buf, uint16_t size)
{
    assert(dev);

    if (size < FRAME_LEN_MIN || size > FRAME_LEN_MAX)
        return -LDAL_ERROR;

    uint16_t sum = 0, i;
    pms_response_t resp = (struct pms_response *)malloc(sizeof(struct pms_response));

#ifdef PKG_USING_PMSXX_DEBUG_DUMP_RESP
    pms_dump(buf, size);
#endif

    memcpy(resp, buf, size);

    resp->length     = ntohs(resp->length);
    resp->PM1_0_CF1  = ntohs(resp->PM1_0_CF1);
    resp->PM2_5_CF1  = ntohs(resp->PM2_5_CF1);
    resp->PM10_0_CF1 = ntohs(resp->PM10_0_CF1);
    resp->PM1_0_atm  = ntohs(resp->PM1_0_atm);
    resp->PM2_5_atm  = ntohs(resp->PM2_5_atm);
    resp->PM10_0_atm = ntohs(resp->PM10_0_atm);
    resp->air_0_3um  = ntohs(resp->air_0_3um);
    resp->air_0_5um  = ntohs(resp->air_0_5um);
    resp->air_1_0um  = ntohs(resp->air_1_0um);
    resp->air_2_5um  = ntohs(resp->air_2_5um);
    resp->air_5_0um  = ntohs(resp->air_5_0um);
    resp->air_10_0um = ntohs(resp->air_10_0um);
#ifdef PKG_USING_PMSXX_ENHANCED
    resp->hcho       = ntohs(resp->hcho);
    resp->temp       = ntohs(resp->temp);
    resp->humi       = ntohs(resp->humi);
#endif
    resp->checksum   = ntohs(resp->checksum);

    for (i = 0; i < (size - 2); i++)
        sum += buf[i];

    if (sum != resp->checksum)
    {
        printf("Checksum incorrect (%04x != %04x)\n", sum, resp->checksum);
        free(resp);
        return -LDAL_ERROR;
    }

#ifdef PKG_USING_PMSXX_DEBUG_SHOW_RESP
    pms_show_response(resp);
#endif
    free(resp);
    return LDAL_EOK;
}

#ifdef PKG_USING_PMSXX_UART_DMA
static void pms_recv_thread_entry(void *parameter)
{
    pms_device_t dev = (pms_device_t)parameter;
    
    char ch;
    pms_frame_t state = PMS_FRAME_END;
    rt_uint16_t idx = 0;
    rt_uint16_t frame_len = 0;
    rt_uint16_t frame_idx = 0;
    rt_ubase_t  size;
    rt_uint32_t len;
    rt_uint8_t  buf[RT_SERIAL_RB_BUFSZ];
    
    while (1)
    {
        if (RT_EOK != rt_mb_recv(dev->rx_mb, &size, RT_WAITING_FOREVER))
        {
            continue;
        }

        len = rt_device_read(dev->serial, 0, buf+idx, size);
        LOG_D("[recv thread] Receive %d bytes, read %d bytes", size, len);

        for (rt_uint32_t i=0; i<len; i++)
        {
            ch = buf[idx];
            switch (state)
            {
            case PMS_FRAME_END:
                if (ch == FRAME_START1)
                {
                    idx = 1;
                    state = PMS_FRAME_HEAD;
                }
                break;
            case PMS_FRAME_HEAD:
                if (ch == FRAME_START2)
                {
                    idx++;
                    state = PMS_FRAME_HEAD_ACK;
                }
                break;
            case PMS_FRAME_HEAD_ACK:
            {
                idx++;
                state = PMS_FRAME_LENGTH;
            }
            break;
            case PMS_FRAME_LENGTH:
            {
                idx++;
                frame_len = buf[idx - 2] << 8 | buf[idx - 1];
                frame_idx = 0;

                if (frame_len <= FRAME_LEN_MAX - 4)
                    state = PMS_FRAME_PAYLOAD;
                else
                    state = PMS_FRAME_END;
            }
            break;
            case PMS_FRAME_PAYLOAD:
            {
                idx++;
                frame_idx++;

                if (frame_idx >= frame_len)
                {
                    state = PMS_FRAME_END;
                    idx = 0;
                    if (RT_EOK == pms_check_frame(dev, buf, frame_len + 4))
                    {
                        rt_sem_release(dev->ack);
                    }
                }
            }
            break;
            default:
            {
                idx = 0;
                state = PMS_FRAME_END;
            }
            break;
            }
        }
    }
}
#else
static void *pms_recv_thread_entry(void *parameter)
{
    ldal_device_t *dev = (ldal_device_t *)parameter;

    char ch;
    pms_frame_t state = PMS_FRAME_END;
    uint16_t idx = 0;
    uint16_t frame_len = 0;
    uint16_t frame_idx = 0;
    uint8_t  buf[FRAME_LEN_MAX] = {0};

    printf("Run reciver thread\n");

    while (1)
    {
        while (read_device(dev, &ch, 1) != 1)
        {
            usleep(1000);
        }

        switch (state)
        {
        case PMS_FRAME_END:
            if (ch == FRAME_START1)
            {
                idx = 0;
                buf[idx++] = ch;
                state = PMS_FRAME_HEAD;
            }
            break;
        case PMS_FRAME_HEAD:
            if (ch == FRAME_START2)
            {
                buf[idx++] = ch;
                state = PMS_FRAME_HEAD_ACK;
            }
            break;
        case PMS_FRAME_HEAD_ACK:
        {
            buf[idx++] = ch;
            state = PMS_FRAME_LENGTH;
        }
        break;
        case PMS_FRAME_LENGTH:
        {
            buf[idx++] = ch;
            frame_len = buf[idx - 2] << 8 | buf[idx - 1];
            frame_idx = 0;

            if (frame_len <= FRAME_LEN_MAX - 4)
                state = PMS_FRAME_PAYLOAD;
            else
                state = PMS_FRAME_END;
        }
        break;
        case PMS_FRAME_PAYLOAD:
        {
            buf[idx++] = ch;
            frame_idx++;

            if (frame_idx >= frame_len)
            {
                state = PMS_FRAME_END;
                idx = 0;
                if (LDAL_EOK == pms_check_frame(dev, buf, frame_len + 4))
                {
                    printf("## ^_^ ##\n");
                }
            }
        }
        break;
        default:
            idx = 0;
            break;
        }
    }
    return 0;
}
#endif

int pms_set_mode(ldal_device_t *dev, pms_mode_t mode)
{
    assert(dev);

    write_device(dev, &preset_commands[mode], sizeof(struct pms_cmd));

#ifdef PKG_USING_PMSXX_DEBUG_SHOW_CMD
    pms_show_command(&preset_commands[mode]);
#endif
    sleep(2);

    return LDAL_EOK;
}
/*
uint16_t pms_read(ldal_device_t *dev, void *buf, uint16_t size, int32_t time)
{
    assert(dev);

    rt_sem_control(dev->ack, RT_IPC_CMD_RESET, NULL);

    write_device(dev, &preset_commands[PMS_MODE_READ], sizeof(struct pms_cmd));

    if (RT_EOK != rt_sem_take(dev->ack, time))
        return 0;

    rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    rt_memcpy(buf, &dev->resp, size);
    rt_mutex_release(dev->lock);

    return size;
}
*/

/*
uint16_t pms_wait(ldal_device_t dev, void *buf, uint16_t size)
{
    assert(dev);

    rt_sem_take(dev->ack, RT_WAITING_FOREVER);

    rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    rt_memcpy(buf, &dev->resp, size);
    rt_mutex_release(dev->lock);

    return size;
}
*/

/*
bool pms_is_ready(ldal_device_t *dev)
{
    return dev->version == 0x00 ? false : true;
}
*/

static void sensor_init_entry(void *parameter)
{
    ldal_device_t *dev = (ldal_device_t *)parameter;

    pms_set_mode(dev, PMS_MODE_NORMAL);
    pms_set_mode(dev, PMS_MODE_ACTIVE);
}

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    void *status;
    pthread_t tid;
    pthread_attr_t attr;

    printf("Sensor Test Start\n");

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("pms");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s\n", device->name);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init serial device failed\n");
        return -1;
    }
    
    /* Configurate serial parameter for sensor */
    ret = control_device(device, SERIAL_SET_OPTIONS, &opt);
    if (ret != LDAL_EOK) {
        printf("Config serial options failed\n");
        return -1;
    }

    /* Set timeout */
    ret = control_device(device, SERIAL_SET_TIMEOUT, (void *)3000); /* 3s */
    if (ret != LDAL_EOK) {
        printf("Config serial timeout failed\n");
        return -1;
    }

    /* Reciver thread */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&tid, &attr, pms_recv_thread_entry, (void *)device);
    pthread_attr_destroy(&attr);

    printf("Initialize sensor\n");
    sensor_init_entry(device);

    pthread_join(tid, &status);
    
    ret = stop_device(device);
    if (ret != LDAL_EOK) {
        printf("Stop serial device failed\n");
    }

    printf("Sensor Test End\n");
    return 0;
}
