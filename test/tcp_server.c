#include <stdio.h>
#include "ldal_udp.h"

/* The second parameter pass <remote_ip:port> or <netdev> */
static struct ldal_udp_device udp0 = {
    "udp0", "0.0.0.0",
};

int main(int argc, char *argv[])
{
    int ret;
    char rbuf[80];
    struct ldal_device *device;
    struct sockaddr_in raddr;

    printf("UDP Server Test Start\n");

    /* Register device */
    ret = ldal_device_register(&udp0.device, udp0.device_name, udp0.file_name, LDAL_CLASS_UDP, (void *)&udp0);
    if (ret != LDAL_EOK) {
        printf("Register udp device failed\n");
    }

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("udp0");
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
    if (0 > bind_local_addr(device, NULL, 1601)) {
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
