#include "ldal_serial.h"

#define MAX_STR_LEN 256

/*
 * The values for speed are 
 * B115200, B230400, B9600, B19200, B38400, B57600, B1200, B2400, B4800, etc
 * 
 *  The values for parity are 0 (meaning no parity), 
 * PARENB|PARODD (enable parity and use odd), 
 * PARENB (enable parity and use even), 
 * PARENB|PARODD|CMSPAR (mark parity), 
 * and PARENB|CMSPAR (space parity).
 * */

/*
 * if waitTime  < 0, it is blockmode
 *  waitTime in unit of 100 millisec : 20 -> 2 seconds 
 */
#if 0
static int setInterfaceAttribs(int fd, int speed, int parity, int waitTime)
{
    int isBlockingMode = 0;
    struct termios tty;

    if (waitTime < 0 || waitTime > 255)
        isBlockingMode = 1;

    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) /* save current serial port settings */
    {
        printf("__LINE__ = %d, error %s\n", __LINE__, strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;                                 // disable break processing
    tty.c_lflag = 0;                                        // no signaling chars, no echo,
                                                            // no canonical processing
    tty.c_oflag = 0;                                        // no remapping, no delays
    tty.c_cc[VMIN] = (1 == isBlockingMode) ? 1 : 0;         // read doesn't block
    tty.c_cc[VTIME] = (1 == isBlockingMode) ? 0 : waitTime; // in unit of 100 milli-sec for set timeout value

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("__LINE__ = %d, error %s\n", __LINE__, strerror(errno));
        return -1;
    }
    return 0;
} /* setInterfaceAttribs */
#endif

static int serial_config(int fd)
{
    struct termios old_tio = {0};
    struct termios new_tio = {0};
    tcgetattr(fd, &old_tio);
    // 设置波特率为115200 B115200
    new_tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    new_tio.c_iflag = 0; // IGNPAR | ICRNL
    new_tio.c_oflag = 0;
    new_tio.c_lflag = 0; // ICANON
    new_tio.c_cc[VTIME] = 0;
    new_tio.c_cc[VMIN] = 1;
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &new_tio);

    // 设置为非阻塞模式，这个在读串口的时候会用到
    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}

static int serial_open(struct ldal_device *device)
{
    assert(device);

    device->fd = open(device->filename, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == device->fd)
    {
        perror("Can't Open Serial Port");
        return -LDAL_ERROR;
    }

    // Determine whether the state of the serial port is blocked or not.
    if (fcntl(device->fd, F_SETFL, 0) < 0)
    {
        printf("fcntl failed!\n");
        return -LDAL_ERROR;
    }
    else
    {
        printf("fcntl = %d\n", fcntl(device->fd, F_SETFL, 0));
    }

    //test Is it a terminal device?
    if (0 == isatty(STDIN_FILENO))
    {
        printf("standard input is not a terminal device\n");
        return -LDAL_ERROR;
    }
    else
    {
        serial_config(device->fd);
        printf("isatty success!\n");
    }

    return LDAL_EOK;
}

static int serial_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int serial_read(struct ldal_device *device, void *buf, size_t len)
{
    assert(device);
    int ret = read(device->fd, buf, len);
    return ret;
}

static int serial_write(struct ldal_device *device, const void *buf, size_t len)
{
    assert(device);
    int ret = write(device->fd, buf, len);
    return ret;
}

static int serial_control(int fd)
{
    return LDAL_EOK;
}

const struct ldal_device_ops serial_device_ops = 
{
    .open = serial_open,
    .close = serial_close,
    .read = serial_read,
    .write = serial_write,
};

/**
 * This function register serial class device
 *
 * @param void none
 *
 * @return On success, returns 0; on error, it returns an error number
 */
int serial_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for serial device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_SERIAL;
    class->device_ops = &serial_device_ops;

    printf("Register serial device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_SERIAL);
}
INIT_CLASS_EXPORT(serial_device_class_register);
