#include <stdio.h>
#include "dev_serial.h"

int main(int argc, char *argv[])
{
    char *filename;
    
    if (argc == 2) {
        filename = argv[1];
    }
    else {
        filename = "/dev/ttyUSB0";
    }
    
}