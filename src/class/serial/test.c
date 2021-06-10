#include <stdio.h>
#include "dev_serial.h"

static struct serial_device serial0 =
{
    "serial0",
    "/dev/ttyUSB0",

    A9G_SAMPLE_POWER_PIN,
    A9G_SAMPLE_STATUS_PIN,
    A9G_SAMPLE_RECV_BUFF_LEN,
};

int main(int argc, char *argv[])
{
    char *filename;
    
    if (argc == 2) {
        filename = argv[1];
    }
    else {
        filename = "/dev/ttyUSB0";
    }

    ldal_device_class_register(struct ldal_device_class *class, uint16_t class_id);
    ldal_device_register()
    
}



static int a9g_device_register(void)
{
    struct at_device_a9g *a9g = &a9g0;

    return at_device_register(&(a9g->device),
                              a9g->device_name,
                              a9g->client_name,
                              AT_DEVICE_CLASS_A9G,
                              (void *) a9g);
}