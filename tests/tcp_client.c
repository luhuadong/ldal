#include <stdio.h>
#include "ldal.h"

/* The second parameter pass <server_ip:port> */
static struct ldal_device_table device_table[] = {
    { "tcp0", "127.0.0.1:8888", LDAL_CLASS_TCP },
};

int main(int argc, char *argv[])
{
    int ret;
    char wbuf[80];
    char rbuf[80];
    struct ldal_device *device;

    printf("TCP Client Test Start\n");

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));

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
    if (0 > connect_server_addr(device, "127.0.0.1", 8888)) {
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

        ret = read_device(device, rbuf, sizeof(rbuf));
        if (ret != LDAL_EOK) {
            printf("Read tcp device failed\n");
            goto __exit;
        }
        printf("Recv > %s\n", rbuf);
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
