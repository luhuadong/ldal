#include <stdio.h>
#include "ldal.h"

static struct ldal_device_table client_table[] = {
    { "tcp0", "192.168.31.45:1024", LDAL_CLASS_TCP },
    { "tcp1", "192.168.31.45:1024", LDAL_CLASS_TCP },
};

int main(int argc, char *argv[])
{
    int ret;
    char wbuf[80];
    char rbuf[80];
    struct ldal_device *device;

    printf("TCP Client Test Start\n");

    /* Register device */
    ldal_device_create(&client_table, ARRAY_SIZE(client_table));

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("tcp0");
    if (device == NULL) {
        printf("Can't get device\n");
        return -1;
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init tcp device failed\n");
        return -1;
    }

#if 1
    /* You can configure server addr again */
    if (0 > connect_server_addr(device, "192.168.31.45", 1024)) {
        printf("connect failed\n");
    }
#endif

    for (int i = 0; i < 5; i++) {
        printf("Send to ...\n");
        snprintf(wbuf, sizeof(wbuf), "[%d] Hello, World!\n", i);

        ret = write_device(device, wbuf, strlen(wbuf));
        if (ret < 0) {
            printf("Write tcp device failed\n");
            goto __exit;
        }
        sleep(1);

        ret = read_device(device, rbuf, sizeof(rbuf));
        if (ret < 0) {
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
