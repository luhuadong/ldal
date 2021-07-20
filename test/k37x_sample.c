#include <stdio.h>
#include "ldal.h"

/* The second parameter pass <remote_ip:port> or <netdev> */
static struct ldal_udp_device udp0 = {
    "udp_server", "0.0.0.0",
};

static struct ldal_rtc_device rtc0 = {
    "rtc0", "/dev/rtc0",
};

static struct ldal_device_table device_table[] = {
    { "AI0", "/dev/aidev0", LDAL_CLASS_ANALOG, 0 },
    { "AI1", "/dev/aidev1", LDAL_CLASS_ANALOG, 1 },
    { "AI2", "/dev/aidev2", LDAL_CLASS_ANALOG, 2 },
    { "AI3", "/dev/aidev3", LDAL_CLASS_ANALOG, 3 },
    { "AI4", "/dev/aidev4", LDAL_CLASS_ANALOG, 4 },
    { "AI5", "/dev/aidev5", LDAL_CLASS_ANALOG, 5 },
    { "AI6", "/dev/aidev6", LDAL_CLASS_ANALOG, 6 },
    { "AI7", "/dev/aidev7", LDAL_CLASS_ANALOG, 7 },
    { "AI8", "/dev/aidev8", LDAL_CLASS_ANALOG, 8 },
    { "AI9", "/dev/aidev9", LDAL_CLASS_ANALOG, 9 },
    { "AI10", "/dev/aidev10", LDAL_CLASS_ANALOG, 10 },
    { "AI11", "/dev/aidev11", LDAL_CLASS_ANALOG, 11 },
    { "AI12", "/dev/aidev12", LDAL_CLASS_ANALOG, 12 },
    { "AI13", "/dev/aidev13", LDAL_CLASS_ANALOG, 13 },
    { "AI14", "/dev/aidev14", LDAL_CLASS_ANALOG, 14 },
    { "AI15", "/dev/aidev15", LDAL_CLASS_ANALOG, 15 },
    
    { "DI0", "/dev/didev0", LDAL_CLASS_DIGITAL },
    { "DI1", "/dev/didev1", LDAL_CLASS_DIGITAL },
    { "DI2", "/dev/didev2", LDAL_CLASS_DIGITAL },
    { "DI3", "/dev/didev3", LDAL_CLASS_DIGITAL },
    { "DI4", "/dev/didev4", LDAL_CLASS_DIGITAL },
    { "DI5", "/dev/didev5", LDAL_CLASS_DIGITAL },
    { "DI6", "/dev/didev6", LDAL_CLASS_DIGITAL },
    { "DI7", "/dev/didev7", LDAL_CLASS_DIGITAL },

    { "DO0", "/dev/dodev0", LDAL_CLASS_DIGITAL },
    { "DO1", "/dev/dodev1", LDAL_CLASS_DIGITAL },
    { "DO2", "/dev/dodev2", LDAL_CLASS_DIGITAL },
    { "DO3", "/dev/dodev3", LDAL_CLASS_DIGITAL },
    { "DO4", "/dev/dodev4", LDAL_CLASS_DIGITAL },
    { "DO5", "/dev/dodev5", LDAL_CLASS_DIGITAL },
    { "DO6", "/dev/dodev6", LDAL_CLASS_DIGITAL },
    { "DO7", "/dev/dodev7", LDAL_CLASS_DIGITAL },

    {"ibutton", "/dev/k37adev_ibutton", LDAL_CLASS_MISC },
    {"battery", "/dev/k37xdev_battery", LDAL_CLASS_MISC },
    {"door1",   "/dev/k37xdev_door1",   LDAL_CLASS_MISC },
    {"door2",   "/dev/k37xdev_door2",   LDAL_CLASS_MISC },
    {"locker1", "/dev/k37xdev_locker1", LDAL_CLASS_MISC },
    {"locker2", "/dev/k37xdev_locker2", LDAL_CLASS_MISC },

    {"uart_en", "/dev/uart-power", LDAL_CLASS_GPIO },
};

int main(int argc, char *argv[])
{
    int ret;
    char rbuf[80];
    struct ldal_device *device;
    struct sockaddr_in raddr;

    printf("K37X sample -- register %lu devices\n", ARRAY_SIZE(device_table));

    ldal_device_create(&device_table, ARRAY_SIZE(device_table));

    /* Register device */
    ret = ldal_device_register(&udp0.device, udp0.device_name, udp0.file_name, LDAL_CLASS_UDP, (void *)&udp0);
    if (ret != LDAL_EOK) {
        printf("Register udp device failed\n");
    }

    ret = ldal_device_register(&rtc0.device, rtc0.device_name, rtc0.file_name, LDAL_CLASS_RTC, (void *) &rtc0);
    if (ret != LDAL_EOK) {
        printf("Register rtc device failed\n");
    }

    ldal_show_device_list();
    establish_serve();

    /* Get device handler */
    device = ldal_device_get_by_name("udp_server");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init udp device failed\n");
        return -1;
    }

    control_device(device, SOCKET_SET_REUSEADDR, 0);
    control_device(device, SOCKET_SET_ECHO_FLAG, 1);  /* echo server */

    /* Bind ip addr and port */
    if (0 > bind_local_addr(device, NULL, 8080)) {
        printf("bind failed\n");
    }

    while (1) {
        puts("waiting data ......");

        ret = read_device(device, rbuf, sizeof(rbuf));
        if (ret < 0) {
            printf("recvfrom failed\n");
            break;
        }
        control_device(device, SOCKET_GET_RECVADDR, &raddr);
        printf("ip: %s, port: %d\n", inet_ntoa(raddr.sin_addr), ntohs(raddr.sin_port));
        printf("the data : %s\n", rbuf);

        ret = write_device(device, rbuf, sizeof(rbuf));
        if(ret < 0) {
            perror("send failed");
        } else {
            printf("send success.\n");
        }
        bzero(rbuf, 80);
    }

    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop udp device failed\n");
    }

    printf("UDP Server Test End\n");
    return 0;
}
