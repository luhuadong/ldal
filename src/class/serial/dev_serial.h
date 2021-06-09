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

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_SERIAL_H__ */