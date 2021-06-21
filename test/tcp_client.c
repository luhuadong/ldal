#include <stdio.h>
#include "ldal_tcp.h"

/* The second parameter pass <server_ip:port> */
static struct ldal_tcp_device tcp0 = {
    "tcp0", "120.78.197.79:8888",
};

int main(int argc, char *argv[])
{
    int ret;
    char wbuf[80];
    char rbuf[80];
    struct ldal_device *device;

    printf("TCP Client Test Start\n");

    /* Register device */
    ret = ldal_device_register(&tcp0.device, tcp0.device_name, tcp0.file_name, LDAL_CLASS_TCP, (void *)&tcp0);
    if (ret != LDAL_EOK) {
        printf("Register tcp device failed\n");
    }

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("tcp0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init tcp device failed\n");
        return -1;
    }

#if 1
    /* You can configure server addr again */
    if (0 > connect_server_addr(device, "120.78.197.79", 8888)) {
        printf("connect failed\n");
    }
#endif

    for (int i = 0; i < 5; i++) {
        printf("Send to ...\n");
        snprintf(wbuf, sizeof(wbuf), "[%d] Hello, World", i);

        ret = write_device(device, wbuf, sizeof(wbuf));
        if (ret != LDAL_EOK) {
            printf("Write tcp device failed\n");
            goto __exit;
        }
        sleep(1);
        printf("Recv from ...\n");

        ret = read_device(device, rbuf, sizeof(rbuf));
        if (ret != LDAL_EOK) {
            printf("Read tcp device failed\n");
            goto __exit;
        }
        printf("> %s\n", rbuf);
        sleep(1);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop tcp device failed\n");
    }

    printf("TCP Client Test End\n");
    return 0;
}
