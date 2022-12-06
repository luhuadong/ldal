#include <string.h>
#include "ldal_serial.h"

#define MAX_STR_LEN 256

static void set_serial_options(struct termios *options, struct port_option *opt)
{
    int baudrate = opt->baudrate;
    int byteSize = opt->data_bits;
    int parity = opt->parity;
    int stopBits = opt->stop_bits;
    int flowctrl = opt->flowctrl;

    cfsetispeed(options, baudrate);
    cfsetospeed(options, baudrate);

    switch(byteSize) {
       case 8  :
            options->c_cflag &= ~CSIZE;
            options->c_cflag |= CS8;
            break;
       case 7 :
            options->c_cflag &= ~CSIZE;
            options->c_cflag |= CS7;
            break;
       case 6 :
            options->c_cflag &= ~CSIZE;
            options->c_cflag |= CS6;
            break;         
       case 5 :
            options->c_cflag &= ~CSIZE;
            options->c_cflag |= CS5;
            break;
        default : 
            break;
    }

    switch(parity) {
        case PAR_NONE :
            options->c_cflag &= ~PARENB;
            break;
        case PAR_ODD :
            options->c_cflag |= PARENB;
            options->c_cflag |= PARODD;
            break;
        case PAR_EVEN :
            options->c_cflag |= PARENB;
            options->c_cflag &= ~PARODD;
            break;
        default : break;
    }

    switch(stopBits) {
        case BITS_1P0 : options->c_cflag &= ~CSTOPB; break;
        case BITS_1P5 : printf("no stop bits 1.5\n"); break;
        case BITS_2P0 : options->c_cflag |= CSTOPB; break;         
        default : break;
    }

    if (flowctrl) {
        options->c_cflag  |= CRTSCTS;  /* start hardware flow control */
    } else {
        options->c_cflag &= ~CRTSCTS;  /* close hardware flow control*/
    }
}

static void get_serial_options(struct termios *options, struct port_option *opt)
{
    int *baudrate = &opt->baudrate;
    int *byteSize = &opt->data_bits;
    int *parity   = &opt->parity;
    int *stopBits = &opt->stop_bits;
    int *flowctrl = &opt->flowctrl;

    *baudrate = cfgetispeed(options);

    switch(options->c_cflag & CSIZE) {
        case CS5: *byteSize = 5; break;
        case CS6: *byteSize = 6; break;
        case CS7: *byteSize = 7; break;
        case CS8: *byteSize = 8; break;
        default: break;
    }

    if(options->c_cflag & PARENB ) {
        if(options->c_cflag & PARODD)
            *parity = PAR_ODD;
        else
            *parity = PAR_EVEN;
    } else {
        *parity = PAR_NONE;
    }

    *stopBits = options->c_cflag & CSTOPB ? BITS_2P0 : BITS_1P0;

    *flowctrl = options->c_cflag & CRTSCTS ?  1 : 0;
}

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
static int set_serial_attribs(int fd, int speed, int parity, int waitTime)
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
}

static int serial_default_config(int fd)
{
    struct termios old_tio = {0};
    struct termios new_tio = {0};
    tcgetattr(fd, &old_tio);
    
    /* Default baudrate 115200, usually B9600, B115200 */
    new_tio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    new_tio.c_iflag = 0; // IGNPAR | ICRNL
    new_tio.c_oflag = 0;
    new_tio.c_lflag = 0; // ICANON
    new_tio.c_cc[VTIME] = 0;
    new_tio.c_cc[VMIN] = 1;
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &new_tio);

    /* Non block while read data */
    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}

static int serial_open(struct ldal_device *device)
{
    assert(device);

    device->fd = open(device->filename, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == device->fd) {
        perror("Can't Open Serial Port");
        return -LDAL_ERROR;
    }

    // Determine whether the state of the serial port is blocked or not.
    if (fcntl(device->fd, F_SETFL, 0) < 0) {
        perror("fcntl failed!");
        return -LDAL_ERROR;
    } else {
        printf("fcntl = %d\n", fcntl(device->fd, F_SETFL, 0));
    }

    //test Is it a terminal device?
    if (0 == isatty(STDIN_FILENO)) {
        printf("standard input is not a terminal device\n");
        return -LDAL_ERROR;
    } else {
        serial_default_config(device->fd);
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

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param dev the pointer of device driver structure
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
static int serial_control(struct ldal_device *dev, int cmd, void *arg)
{
    assert(dev);

    int ret = 0;
    struct termios options;
    struct port_option *opt;
    struct ldal_serial_device *serial = (struct ldal_serial_device *)dev->user_data;

    switch(cmd) {
    case SERIAL_GET_OPTIONS: 
    {
        opt = (struct port_option *) arg;
        if(!opt) {
            return -LDAL_EINVAL;
        }
        
        if(tcgetattr(dev->fd, &options) < 0) {
            printf("error,tcgetattr:\n");
            return -LDAL_ERROR;
        }
        get_serial_options(&options, opt);
    } break;

    case SERIAL_SET_OPTIONS: 
    {
        opt = (struct port_option *) arg;
        if(!opt) {
            return -LDAL_EINVAL;
        }
        
        if( tcgetattr(dev->fd, &options) < 0) {
            printf("error,tcgetattr:\n");
            return -LDAL_ERROR;
        }
        
        set_serial_options(&options, opt);

        tcflush(dev->fd, TCIOFLUSH);
        if(tcsetattr(dev->fd, TCSANOW,  &options) < 0) {//serial0 set options error. because of serial0 fd=-1;
            printf("error tcsetattr:\n");
            return -LDAL_ERROR;
        }
        serial->opt = *opt;
        //memcpy(&serial->opt, opt, sizeof(struct port_option));
    } break;

    case SERIAL_READ_TIMEOUT : 
    {
        serial->timeout = (int)arg;
    } break;

    default: 
        ret = -LDAL_EINVAL;
        break;
    }
    return LDAL_EOK;
}

static int serial_init(struct ldal_device *dev)
{
    assert(dev);

    struct ldal_serial_device *serial = (struct ldal_serial_device *)dev->user_data;
    serial->timeout = 0;

    return LDAL_EOK;
}

const struct ldal_device_ops serial_device_ops = 
{
    .init  = serial_init,
    .open = serial_open,
    .close = serial_close,
    .read = serial_read,
    .write = serial_write,
    .control = serial_control,
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
