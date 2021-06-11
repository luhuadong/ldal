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

#include "ldal.h"

struct serial_port {
    int fd;
    char *dev_name;
    struct termios old_tio;
    struct termios new_tio;
};

struct ldal_serial_device
{
    char *device_name;
    char *file_name;
    int status;

    struct ldal_device device;
    void *user_data;

    /* custom ops */
    int (*open)(struct serial_port *port);
    int (*read)(struct serial_port *port,char *buf,int len);
    int (*write)(struct serial_port *port,char *buf,int len);
    int (*control)(struct serial_port *port,unsigned int cmd,unsigned long arg);
    int (*config)(struct serial_port *port,unsigned int cmd,unsigned long arg);
    int (*close)(struct serial_port *port);
    int (*show)(struct serial_port *port);
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

struct port_ops
{
    int (*open)(struct serial_port *port);
    int (*read)(struct serial_port *port,char *buf,int len);
    int (*write)(struct serial_port *port,char *buf,int len);
    int (*control)(struct serial_port *port,unsigned int cmd,unsigned long arg);
    int (*config)(struct serial_port *port,unsigned int cmd,unsigned long arg);
    int (*close)(struct serial_port *port);
    int (*show)(struct serial_port *port);
};

int serial_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_SERIAL_H__ */