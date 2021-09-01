#include <stdio.h>
#include "ldal.h"

static char name[32];

static struct ldal_device_table device_table[] = {
    { "AI0", "/dev/aidev0",  LDAL_CLASS_ANALOG,  (void *)0 },
    { "AI1", "/dev/aidev1",  LDAL_CLASS_ANALOG,  (void *)1 },
    { "AI2", "/dev/aidev2",  LDAL_CLASS_ANALOG,  (void *)2 },
    { "AI3", "/dev/aidev3",  LDAL_CLASS_ANALOG,  (void *)3 },
    { "AI4", "/dev/aidev4",  LDAL_CLASS_ANALOG,  (void *)4 },
    { "AI5", "/dev/aidev5",  LDAL_CLASS_ANALOG,  (void *)5 },
    { "AI6", "/dev/aidev6",  LDAL_CLASS_ANALOG,  (void *)6 },
    { "AI7", "/dev/aidev7",  LDAL_CLASS_ANALOG,  (void *)7 },
    { "AI8", "/dev/aidev8",  LDAL_CLASS_ANALOG,  (void *)8 },
    { "AI9", "/dev/aidev9",  LDAL_CLASS_ANALOG,  (void *)9 },
    { "AI10", "/dev/aidev10", LDAL_CLASS_ANALOG, (void *)10 },
    { "AI11", "/dev/aidev11", LDAL_CLASS_ANALOG, (void *)11 },
    { "AI12", "/dev/aidev12", LDAL_CLASS_ANALOG, (void *)12 },
    { "AI13", "/dev/aidev13", LDAL_CLASS_ANALOG, (void *)13 },
    { "AI14", "/dev/aidev14", LDAL_CLASS_ANALOG, (void *)14 },
    { "AI15", "/dev/aidev15", LDAL_CLASS_ANALOG, (void *)15 },
};

int main(int argc, char *argv[])
{
    int ret, num = 0;
    float value = 0;
    struct ldal_device *device;

    printf("Analog Port Test Start\n");

    if (argc > 1) {
        num = atoi(argv[1]);
    }

    snprintf(name, 32, "AI%d", num);

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));
    
    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name(name);
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init analog device failed\n");
        return -1;
    }

    printf("Reading analog port ...\n");

    for (int i = 0; i < 5; i++) {
        
        ret = read_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK)
        {
            printf("Read analog device failed\n");
            goto __exit;
        }
        printf("> %f\n", value);
        sleep(1);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop analog device failed\n");
    }

    ldal_device_unregister(device);
    printf("Analog Port Test End\n");
    return 0;
}
