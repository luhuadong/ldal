#include <stdio.h>
#include "dev_serial.h"

static struct ldal_serial_device serial0 =
{
    "serial0",
    "/dev/ttyUSB0",
};

int main(int argc, char *argv[])
{
    serial_device_class_register();
    ldal_device_register(&serial0.device, serial0.device_name, serial0.file_name, LDAL_CLASS_SERIAL, (void *) &serial0);

    
    
}
