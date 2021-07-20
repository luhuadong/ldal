#include <stdio.h>
#include <fcntl.h>    /*文件控制定义*/
#include <termios.h>  /*POSIX 终端控制定义*/
#include <errno.h>    /*错误号定义*/
#include <string.h>
#include <assert.h>
#include "ldal_me.h"

#define ME_ECHO_OFF             "ATE0\r\n"
#define ME_ECHO_ON              "ATE1\r\n"
#define ME_GET_VENDOR_ID        "AT+CGMI\r\n"
#define ME_GET_MODEL_ID         "AT+CGMM\r\n"
#define ME_GET_ICCID            "AT+QCCID\r\n"
#define ME_GET_IMSI             "AT+CIMI\r\n"
#define ME_GET_IMEI             "AT+CGSN\r\n"

#define ME_EC20_SET_NETWORK     "AT+CREG=2\r\n"
#define ME_EC20_GET_LAC_CID     "AT+CREG?\r\n"
#define ME_EC20_GET_MODLE_INFO  "ATI\r\n"
#define ME_GET_LAC_CID          "AT+CMGS?\r\n"
#define ME_EC20_GNSS_UP         "AT+QGPS=1\r\n"
#define ME_EC20_GNSS_DOWN       "AT+QGPSEND\r\n"
#define ME_GET_CENTER_NUM       "AT+CSCA?\r\n"

#define ME_GET_NETWORK          "AT+CGREG?\r\n"

//msg cmd
/* 指令收发短信主要有两种模式：Text模式和PDU（Protocol Data Unit，协议数据单元）模式。
 * 使用Text模式收发短信代码简单，很容易实现，最大缺点不支持中文短信。
 * PDU模式不仅能发送中文短信，也能发送英文短信，如果想只是接发英文短信，可以设置为TEXT模式。
*/
#define ME_SET_PDU_MODE         "AT+CMGF=0\r\n"
#define ME_SET_TEXT_MODE        "AT+CMGF=1\r\n"
#define ME_SEND_PDU_MSGS        "AT+CMGS="

//interface define
#define ME_EC20_MODOM_DEV_USB   "/dev/ttyUSB3"
#define ME_UMA2_MODOM_DEV_USB   "/dev/ttyUSB5"
#define ME_EC20_MODOM_DEV_UART  "/dev/ttyO1"

#define ME_EC25_DM_PORT         "/dev/ttyUSB0"
#define ME_EC25_NMEA_PORT       "/dev/ttyUSB1"
#define ME_EC25_AT_PORT         "/dev/ttyUSB2"
#define ME_EC25_PPP_PORT        "/dev/ttyUSB3"

#if 0
bool check_send_cmd(const char *cmd, const char *resp, uint32_t timeout)
{
	int i = 0;
	uint32_t timeStart = millis();

	cleanBuffer(ATbuffer, BUF_LEN);
	Serial.print(cmd);
	Serial.flush();

	while(1) {

		while(Serial.available()) {

			ATbuffer[i++] = Serial.read();
			if(i >= BUF_LEN) {
				i = 0;
			}
		}
		if(NULL != strstr(ATbuffer, resp)) {
			break;
		}
		if((uint32_t)(millis() - timeStart > timeout * 1000)) {
			break;
		}
	}

	if(NULL != strstr(ATbuffer, resp)) {
		return true;
	}
	return false;
}
#endif

#define AT_CLIENT_RECV_BUFF_LEN       256

// timeout  millisecond
static int check_send_cmd(struct ldal_me_device *client, const char* cmd, const char* resp_expr, const size_t lines, const int32_t timeout)
{
    at_response_t resp = NULL;
    int result = 0;
    char resp_arg[AT_CMD_MAX_LEN] = { 0 };

    resp = at_create_resp(AT_CLIENT_RECV_BUFF_LEN, lines, timeout);
    if (resp == NULL)
    {
        printf("No memory for response structure!");
        return -LDAL_ENOMEM;
    }

    result = at_obj_exec_cmd(client, resp, cmd);
    if (result < 0)
    {
        printf("AT client send commands failed or wait response timeout!");
        goto __exit;
    }

    //show_resp_info(resp);

    if (resp_expr) 
    {
        if (at_resp_parse_line_args_by_kw(resp, resp_expr, "%s", resp_arg) <= 0)
        {
            printf("# >_< Failed");
            result = -LDAL_ERROR;
            goto __exit;
        }
        else
        {
            printf("# ^_^ successed");
        }
    }
    
    result = LDAL_EOK;

__exit:
    if (resp)
    {
        at_delete_resp(resp);
    }

    return result;
}

static int me_ec25_send_messages(struct ldal_device *dev, const void *message_pdu, int len_udp)
{
    assert(dev);

    char buffer[300] = {0};
    //struct ldal_me_device *me = (struct ldal_me_device *)dev->user_data;

    write(dev->fd, ME_SET_PDU_MODE, strlen(ME_SET_PDU_MODE));
    sleep(2);
    memset(buffer, 0, sizeof(buffer));
    read(dev->fd, buffer, sizeof(buffer));
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

static int serial_default_config(int fd)
{
    struct termios old_tio = {0};
    struct termios new_tio = {0};
    tcgetattr(fd, &old_tio);
    // 设置波特率为 115200
    new_tio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
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

static int me_init(struct ldal_device *dev)
{
    assert(dev);

    pthread_attr_t attr;
    struct ldal_me_device *client = (struct ldal_me_device *)dev->user_data;

    client->status = AT_STATUS_UNINITIALIZED;

    client->recv_line_len = 0;
    client->recv_line_buf = (char *) calloc(1, AT_RECV_BUFF_SIZE);
    if (client->recv_line_buf == NULL)
    {
        printf("AT client initialize failed! No memory for receive buffer.");
        return -LDAL_ENOMEM;
    }

    pthread_mutex_init(&client->lock, NULL);
    sem_init(&client->resp_notice, 0, 0);

    /* Release child thread resources in the main thread */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&client->parser, &attr, at_client_parser, (void *)client);
    
    pthread_attr_destroy(&attr);

    return LDAL_EOK;
}

static int me_open(struct ldal_device *dev)
{
    assert(dev);

    dev->fd = open(dev->filename, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == dev->fd) {
        perror("Can't Open Serial Port");
        return -LDAL_ERROR;
    }

    // Determine whether the state of the serial port is blocked or not.
    if (fcntl(dev->fd, F_SETFL, 0) < 0) {
        perror("fcntl failed!");
        return -LDAL_ERROR;
    } else {
        printf("fcntl = %d\n", fcntl(dev->fd, F_SETFL, 0));
    }

    if (0 == isatty(STDIN_FILENO)) {
        printf("standard input is not a terminal device\n");
        return -LDAL_ERROR;
    } else {
        serial_default_config(dev->fd);
        printf("isatty success!\n");
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
    .init  = me_init,
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
