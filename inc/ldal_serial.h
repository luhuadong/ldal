#ifndef __LDAL_DEVICE_SERIAL_H__
#define __LDAL_DEVICE_SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>    /*标准输入输出定义*/
#include <stdlib.h>   /*标准函数库定义*/
#include <unistd.h>   /*Unix 标准函数定义*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>    /*文件控制定义*/
#include <termios.h>  /*POSIX 终端控制定义*/
#include <errno.h>    /*错误号定义*/
#include <string.h>
#include <assert.h>
#include <pthread.h> /* POSIX Threads */

#include "ldal.h"

#define SERIAL_SET_TIMEOUT   0x101
#define SERIAL_SET_OPTIONS   0x102
#define SERIAL_GET_OPTIONS   0x103

typedef enum {
    PAR_NONE = 0,
    PAR_EVEN = 1,
    PAR_ODD = 2,
} ser_parity_t;

typedef enum {
    BITS_1P0 = 1,
    BITS_1P5 = 0,
    BITS_2P0 = 2,
} ser_stopbits_t;

typedef enum {
    FC_OFF = 0,
    FC_ON = 1,
} ser_flowctrl_t;

struct flow_ctrl {
    unsigned char dtr_dsr;
    unsigned char rts_cts;
    unsigned char xon_xoff;
};

struct port_option {
    unsigned int baudrate;     /* Baudrate:  */
    unsigned char data_bits;   /* ByteSize: 4, 5, 6, 7, 8 */
    ser_parity_t parity;       /* Parity: None, Odd, Even, Mark, Space */
    ser_stopbits_t stop_bits;  /* StopBits: 1, 1.5, 2  */
    ser_flowctrl_t flowctrl;   /* Flow Control: SW, HW, No Ctrl Flow */
    struct flow_ctrl fcopt;
	char MN[25];
	int addr;
};

struct serial_port {
    int fd;
    char *dev_name;
    struct termios old_tio;
    struct termios new_tio;
};

struct ldal_serial_device
{
    struct ldal_device device;
    int status;
    uint32_t timeout;
    struct port_option opt;

    void *user_data;
};

#if 0
struct port_option{
    unsigned int baudrate;
    unsigned char data_bits;
    unsigned char parity;
#define PAR_NONE  0
#define PAR_EVEN 1
#define PAR_ODD 2
    unsigned char stop_bits;
#define BITS_1P0 1
#define BITS_1P5 0
#define BITS_2P0 2
    unsigned char flowctrl;
#define FC_ON 1
#define FC_OFF 0
    struct flow_ctrl fcopt;
	char MN[25];
	int addr;
};
#endif

int serial_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_SERIAL_H__ */
