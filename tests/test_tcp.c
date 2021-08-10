#include <stdio.h>
#include "ldal.h"

#define BUF_SIZE   512

static struct ldal_device_table client_table[] = {
    { "tcp0", "127.0.0.1:1107", LDAL_CLASS_TCP },
};

void *reader_thread(void *args)
{
    int i;
    uint16_t cnt = 0;
    char buf[BUF_SIZE] = {0};
    struct ldal_device *device = (struct ldal_device *)args;

    while (1) {
        memset(buf, 0, sizeof(buf));
        read_device(device, buf, BUF_SIZE);
        printf("[%d] Recv: %s\n", cnt++, buf);
        sleep(1);
    }

    pthread_exit(NULL);
}

void *writer_thread(void *args)
{
    uint16_t cnt = 0;
    struct ldal_device *device = (struct ldal_device *)args;
    char buf[BUF_SIZE] = {0};

    while (1) {
        snprintf(buf, sizeof(buf), "[%u] Hello, World!\n", cnt++);
        printf("Send: %s\n", buf);
        write_device(device, buf, strlen(buf));
        sleep(1);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    pthread_t r_tid, w_tid;

    printf("TCP Client Test Start\n");

    /* Register device */
    ldal_device_create(&client_table, ARRAY_SIZE(client_table));

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
        goto __exit;
    }

#if 1
    printf("Set timeout\n");
    /* Set timeout */
    ret = control_device(device, SOCKET_SET_RECVTIMEO, 1000);
    if (ret != LDAL_EOK) {
        printf("Config socket recv timeout failed\n");
        goto __exit;
    }
#endif

    /* You can configure server addr again */
    if (CONNECTED_STATE != check_status(device)) {
        ret = connect_server_addr(device, "192.168.31.45", 1024);
        if (ret < 0) {
            printf("connect failed\n");
        } else {
            printf("reconnect success\n");
        }
    }

    ldal_show_device_list();

    /* Create thread */
    pthread_create(&w_tid, NULL, writer_thread, (void *)device);
    pthread_create(&r_tid, NULL, reader_thread, (void *)device);

    pthread_join(w_tid, NULL);
    pthread_join(r_tid, NULL);

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop tcp device failed\n");
    }

    printf("TCP Client Test End\n");
    return 0;
}
