#include <stdio.h>
#include "ldal_memory.h"

#define BUF_SIZE   40

void *read_thread(void)
{
    int i;
    char buf[BUF_SIZE] = {0};
    struct ldal_device *device;
    
    device = ldal_device_get_by_name("mem0");
    if (device == NULL) {
        printf("Can't get device\n");
    }

    for (i=0; i<15; i++) {
        usleep(300 * 1000);
        
        read_device(device, buf, BUF_SIZE);
        /*
        for (int i=0; i<BUF_SIZE; i++) {
            printf("%0x ", buf[i]);
        }
        printf("\n");
        */
        printf("Read: %s\n", buf);
    }

    return NULL;
}

void *write_thread(void *args)
{
    int i;
    struct ldal_device *device;

    pthread_t tid = pthread_self();
    
    device = ldal_device_get_by_name("mem0");
    if (device == NULL) {
        printf("Can't get device\n");
    }

    for (i=0; i<5; i++) {
        char str[BUF_SIZE] = {0};
        sprintf(str, "[%d] Thread #%lu : Hello, World!", i, tid%100);
        write_device(device, str, strlen(str));
        sleep(1);
    }

    return NULL;
}

static struct ldal_memory_device mem0 =
{
    "mem0",
    "/dev/null",
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    pthread_t read_tid, write1_tid, write2_tid;

    printf("Memory Test Start\n");

    ret = memory_device_class_register();
    if (ret != LDAL_EOK) {
        printf("Register memory class failed\n");
    }

    ret = ldal_device_register(&mem0.device, mem0.device_name, mem0.file_name, LDAL_CLASS_MEMORY, &mem0);
    if (ret != LDAL_EOK) {
        printf("Register memory device failed\n");
    }

    /* Get device handler */
    device = ldal_device_get_by_name("mem0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s\n", device->name);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init memory device failed\n");
        return -1;
    }

    /* Create thread */

    ret = pthread_create(&write1_tid, NULL, (void *)write_thread, NULL);
    if (ret) {
        printf("Create write thread failed\n");
        goto __exit;
    }

    usleep(500 * 1000);

    ret = pthread_create(&write2_tid, NULL, (void *)write_thread, NULL);
    if (ret) {
        printf("Create write thread failed\n");
        goto __exit;
    }

    ret = pthread_create(&read_tid, NULL, (void *)read_thread, NULL);
    if (ret) {
        printf("Create read thread failed\n");
        goto __exit;
    }

    pthread_join(read_tid, NULL);
    
__exit:
    stop_device(device);

    printf("Memory Test End\n");
    return 0;
}
