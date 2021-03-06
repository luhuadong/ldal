#include <stdio.h>
#include "ldal.h"

/* The second parameter pass <remote_ip:port> or <netdev> */
static struct ldal_device_table client_table[] = {
    { "udp0", "127.0.0.1:7575", LDAL_CLASS_UDP },
};

int main(int argc, char *argv[])
{
    int ret;
    char wbuf[80];
    char rbuf[80];
    struct ldal_device *device;

    printf("UDP Client Test Start\n");

    /* Register device */
    ldal_device_create(client_table, ARRAY_SIZE(client_table));

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

#if 0
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
            printf("Write udp device failed\n");
            goto __exit;
        }
        sleep(1);
        printf("Recv from ...\n");

        ret = read_device(device, rbuf, sizeof(rbuf));
        if (ret != LDAL_EOK) {
            printf("Read udp device failed\n");
            goto __exit;
        }
        printf("> %s\n", rbuf);
        sleep(1);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop udp device failed\n");
    }

    printf("UDP Client Test End\n");
    return 0;
}
