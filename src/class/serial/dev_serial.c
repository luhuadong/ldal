#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <termios.h> /* termio.h for serial IO api */

#include <pthread.h> /* POSIX Threads */

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

static int setInterfaceAttribs(int fd, int speed, int parity, int waitTime)
{
    int isBlockingMode;
    struct termios tty;

    isBlockingMode = 0;
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

int open(char *dev_name)
{
    assert(dev_name);

    int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);

    int fd;
    fd = open(dev_name, O_RDWR);
    if (-1 == fd) {
        perror("Can not open serial device");
    }
}

int close(char *dev_name)
{}

int read(char *dev_name)
{}

int write(char *dev_name)
{}

int open(char *dev_name)
{}
