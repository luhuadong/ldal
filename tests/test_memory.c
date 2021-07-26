#include <stdio.h>
#include "ldal.h"

#define BUF_SIZE      10
#define READER_NUM    1
#define WRITER_NUM    2

void *reader_thread(void *args)
{
    int i;
    char buf[BUF_SIZE] = {0};
    long tid = (long)args;
    struct ldal_device *device;
    
    device = ldal_device_get_by_name("mem0");
    if (device == NULL) {
        printf("Can't get device\n");
    }

    for (i=0; i<10; i++) {
        read_device(device, buf, BUF_SIZE);
        printf("[%d] Reader[%lu] : %s\n", i, tid, buf);
        sleep(1);
    }

    pthread_exit(NULL);
}

void *writer_thread(void *args)
{
    int i;
    struct ldal_device *device;
    char *str = (char *)args;
    
    device = ldal_device_get_by_name("mem0");
    if (device == NULL) {
        printf("Can't get device\n");
    }

    for (i=0; i<5; i++) {
        printf("Write %s\n", str);
        write_device(device, str, strlen(str));
        sleep(1);
    }

    pthread_exit(NULL);
}

static struct ldal_memory_device mem0 = { "mem0", "/dev/null", };

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    pthread_t r_tid[READER_NUM];
    pthread_t w_tid1, w_tid2;

    printf("Memory Test Start\n");

    ret = ldal_device_register(&mem0.device, mem0.device_name, mem0.file_name, LDAL_CLASS_MEMORY, &mem0);
    if (ret != LDAL_EOK) {
        printf("Register memory device failed\n");
    }
    
    ldal_show_device_list();

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
    pthread_create(&w_tid1, NULL, writer_thread, "0123456789");
    pthread_create(&w_tid2, NULL, writer_thread, "abcdefghij");
    
    sleep(1);

    for (long i = 0; i < READER_NUM; ++i) {
        ret = pthread_create(&r_tid[i], NULL, reader_thread, (void *)i);
        if (ret) {
            printf("Create read thread failed\n");
            goto __exit;
        }
    }

    pthread_join(w_tid1, NULL);
    pthread_join(w_tid2, NULL);

    for (long i = 0; i < READER_NUM; ++i) {
        pthread_join(r_tid[i], NULL);
    }
    
__exit:
    stop_device(device);

    printf("Memory Test End\n");
    return 0;
}
