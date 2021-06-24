#include <stdio.h>
#include "ldal_me.h"

#define ME_EC20_ECHO_OFF        "ATE0\r\n"
#define ME_EC20_SET_NETWORK     "AT+CREG=2\r\n"
#define ME_EC20_GET_LAC_CID     "AT+CREG?\r\n"
#define ME_EC20_GET_MODLE_INFO  "ATI\r\n"
#define ME_GET_LAC_CID          "AT+CMGS?\r\n"
#define ME_EC20_GNSS_UP         "AT+QGPS=1\r\n"
#define ME_EC20_GNSS_DOWN       "AT+QGPSEND\r\n"
#define ME_GET_CENTER_NUM       "AT+CSCA?\r\n"
#define ME_GET_VERSION          "AT+CGMI\r\n"
#define ME_GET_NETWORK          "AT+CGREG?\r\n"

//msg cmd
#define ME_SET_PDU_MODE         "AT+CMGF=0\r\n"
#define ME_SEND_PDU_MSGS        "AT+CMGS="
//interface define
#define ME_EC20_MODOM_DEV_USB   "/dev/ttyUSB3"
#define ME_UMA2_MODOM_DEV_USB   "/dev/ttyUSB5"
#define ME_EC20_MODOM_DEV_UART  "/dev/ttyO1"

#define ME_EC25_DM_PORT         "/dev/ttyUSB0"
#define ME_EC25_NMEA_PORT       "/dev/ttyUSB1"
#define ME_EC25_AT_PORT         "/dev/ttyUSB2"
#define ME_EC25_PPP_PORT        "/dev/ttyUSB3"

static int me_ec25_send_messages(struct ldal_device *dev, char *message_pdu, int len_udp)
{
    assert(dev);

    char buffer[300] = {0};
    struct ldal_me_device *me = (struct ldal_me_device *)dev->user_data;

    write(dev->fd, ME_SET_PDU_MODE, strlen(ME_SET_PDU_MODE));
    sleep(2);
    memset(buffer, 0, sizeof(buffer));
    read(dev->fd,buffer,sizeof(buffer));
    if(strstr(buffer, "OK")) {
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%s%d\r\n", ME_SEND_PDU_MSGS, len_udp);
        write(dev->fd, buffer, strlen(buffer));
        sleep(1);
        memset(buffer, 0, sizeof(buffer));
        read(dev->fd, buffer, sizeof(buffer));
        write(dev->fd, message_pdu, strlen(message_pdu));
        sleep(4);
        memset(buffer, 0, sizeof(buffer));
        read(dev->fd, buffer, sizeof(buffer));
    } else {
        printf("EC25 send message failed.\n");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int me_open(struct ldal_device *dev)
{
    assert(dev);

    dev->fd = open(dev->filename, O_RDWR);
    if (dev->fd < 0)
    {
        perror("Can't open mobile equipment");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int me_close(struct ldal_device *dev)
{
    assert(dev);
    close(dev->fd);
    return LDAL_EOK;
}

static int me_read(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    ret = read(dev->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int me_write(struct ldal_device *dev, const void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    ret = me_ec25_send_messages(dev, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

const struct ldal_device_ops me_device_ops = 
{
    .open  = me_open,
    .close = me_close,
    .read  = me_read,
    .write = me_write,
};

int me_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for me device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_ME;
    class->device_ops = &me_device_ops;

    printf("Register me device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_ME);
}
INIT_CLASS_EXPORT(me_device_class_register);
