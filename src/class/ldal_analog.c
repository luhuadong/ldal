#include <stdio.h>
#include "ldal_analog.h"

struct ldal_analog_calibration
{
    /* data */
    float current_a;
    float current_b;
    float voltage_a;
    float voltage_b;
};

/*
signalType: 0 -> voltage, 1 -> current

1 0.178327 -43.927979   aifile0
1 0.180870 -46.151855   aifile1
1 0.177788 -44.506348   aifile10
1 0.179158 -44.849121   aifile11
1 0.178657 -45.319092   aifile12
1 0.180050 -44.832520   aifile13
1 0.179673 -44.618896   aifile14
1 0.178285 -45.640869   aifile15
1 0.180549 -46.461182   aifile2
1 0.180029 -45.967529   aifile3
1 0.181192 -46.022949   aifile4
1 0.178966 -44.980225   aifile5
1 0.180223 -45.296143   aifile6
1 0.178660 -44.695068   aifile7
1 0.178508 -44.091309   aifile8
1 0.179469 -43.431396   aifile9
1 0.717927 -166.989258  aifile_current0
1 0.728120 -175.913086  aifile_current1
1 0.716230 -176.764648  aifile_current10
1 0.723066 -171.366211  aifile_current11
1 0.721058 -175.144531  aifile_current12
1 0.724743 -169.734375  aifile_current13
1 0.723537 -171.768555  aifile_current14
1 0.718030 -173.620117  aifile_current15
1 0.726903 -178.672852  aifile_current2
1 0.725111 -177.216797  aifile_current3
1 0.729129 -178.636719  aifile_current4
1 0.719476 -172.386719  aifile_current5
1 0.727220 -174.095703  aifile_current6
1 0.720669 -171.807617  aifile_current7
1 0.719424 -171.943359  aifile_current8
1 0.722074 -167.954102  aifile_current9

*/


static int analog_open(struct ldal_device *device)
{
    assert(device);

    device->fd = open(device->filename, O_RDWR);
    if (device->fd < 0)
    {
        perror("Can't open analog port");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int analog_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int analog_read(struct ldal_device *device, void *buf, size_t len)
{
    assert(device);
    assert(buf);

    int ret;
    ret = read(device->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int analog_write(struct ldal_device *device, const void *buf, size_t len)
{
    assert(device);
    assert(buf);

    int ret;
    ret = write(device->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

const struct ldal_device_ops analog_device_ops = 
{
    .open  = analog_open,
    .close = analog_close,
    .read  = analog_read,
    .write = analog_write,
};

int analog_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for analog device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_ANALOG;
    class->device_ops = &analog_device_ops;

    return ldal_device_class_register(class, LDAL_CLASS_ANALOG);
}
