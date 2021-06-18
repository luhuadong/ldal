#include <stdio.h>
#include <string.h>
#include "ldal_analog.h"

#define ANALOG_CALIB_DIR        "/etc/aicfgs/"

#define AI_CONF_VOLT_CURR       _IOW('k',1,int)
#define AI_SET_VOLT_CURR_CAL    _IOW('k',5,int)
#define AI_GET_VOLT_CURR_CAL    _IOW('k',6,int)
#define AI_SET_ANALOG_MODE      _IOW('T',7,int)
#define AI_GET_ANALOG_VALUE     _IOW('T',8,int)

/* Calibration point */
#define CURRENT_CORRECT_6MA     0
#define CURRENT_CORRECT_16MA    1
#define VOLTAGE_CORRECT_1V      2
#define VOLTAGE_CORRECT_4V      3

#define ADC_CURRENT_CALIB_POINT1    6000.0f        /* 6mA */
#define ADC_CURRENT_CALIB_POINT2    16000.0f       /* 16mA */
#define ADC_VOLTAGE_CALIB_POINT1    1000.0f        /* 1V */
#define ADC_VOLTAGE_CALIB_POINT2    4000.0f        /* 4V */

#define ADC_CURRENT_VALUE_MIN       4000.0f        /* 4mA */
#define ADC_CURRENT_VALUE_MAX       20000.0f       /* 20mA */
#define ADC_VOLTAGE_VALUE_MIN       0.0f           /* 0V */
#define ADC_VOLTAGE_VALUE_MAX       5000.0f        /* 5V */

/* Storage Set and get */
struct ai_calibrate {
    char ai_dev;
    float vol_a;
    float vol_b;
    float cur_a;
    float cur_b;
    unsigned short crc;
};

struct ai_type {
    char ai_dev;
    char type;
};

struct ai_correct_cfg {
    int signalType;     // 0:current 1:voltage
    int valueType;      // 0 : current (6mA); 1:current (16mA);2 : voltage (1V); 3:voltage (4V); 
};



/**
 * This function will get analog input calibration data from a file.
 *
 * @param dev the pointer of device driver structure
 * @param buf the buffer save the data
 * @param len the length of buffer (how many bytes you want to get)
 *
 * @return the result
 */
static int get_calib_data_from_file(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int fd;
    char pathname[LDAL_NAME_MAX * 2] = {0};

    strncpy(pathname, ANALOG_CALIB_DIR, strlen(ANALOG_CALIB_DIR));
    strncat(pathname, dev->name, strlen(dev->name));

    fd = open(pathname, O_RDONLY);
    if (fd == -1) {
        return -LDAL_ERROR;
    }

    if (-1 == read(fd, buf, len)) {
        close(fd);
        return -LDAL_ERROR;
    }

    close(fd);
    return LDAL_EOK;
}

/**
 * This function will save analog input calibration data to a file.
 *
 * @param dev the pointer of device driver structure
 * @param buf the buffer of calibration data
 * @param len the length of buffer (how many bytes you want to write)
 *
 * @return the result
 */
static int save_calib_data_to_file(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int fd;
    char pathname[LDAL_NAME_MAX * 2] = {0};

    strncpy(pathname, ANALOG_CALIB_DIR, strlen(ANALOG_CALIB_DIR));
    strncat(pathname, dev->name, strlen(dev->name));

    fd = open(pathname, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        return -LDAL_ERROR;
    }

    if (-1 == write(fd, buf, len)) {
        close(fd);
        return -LDAL_ERROR;
    }

    close(fd);
    return LDAL_EOK;
}

static int analog_init(struct ldal_device *dev)
{
    assert(dev);

    struct ldal_analog_device *adev = (struct ldal_analog_device *)dev->user_data;

    /* Set default value */
    adev->mode = ADC_MODE_CURRENT;
    adev->aicfgs[ADC_MODE_CURRENT].is_corrected = false;
    adev->aicfgs[ADC_MODE_CURRENT].slope = 1.0;
    adev->aicfgs[ADC_MODE_CURRENT].intercept = 0.0;
    adev->aicfgs[ADC_MODE_VOLTAGE].is_corrected = false;
    adev->aicfgs[ADC_MODE_VOLTAGE].slope = 1.0;
    adev->aicfgs[ADC_MODE_VOLTAGE].intercept = 0.0;

    /* Import calib data if existed */
    get_calib_data_from_file(dev, &adev->aicfgs, sizeof(adev->aicfgs));

    return LDAL_EOK;
}

static int analog_open(struct ldal_device *dev)
{
    assert(dev);

    dev->fd = open(dev->filename, O_RDWR | O_NONBLOCK);
    if (dev->fd < 0)
    {
        perror("Can't open analog port");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int analog_close(struct ldal_device *dev)
{
    assert(dev);
    close(dev->fd);
    return LDAL_EOK;
}

/* Fit linear regression model, intercept - Slope  */
static float fit_linear_model(const float x, const float slope, const float intercept)
{
    float y;
    y = x * slope + intercept;
    return y;
}

static int analog_read(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    float value = 0.0;
    struct ldal_analog_device *adev = (struct ldal_analog_device *)dev->user_data;

    /* Get raw analog data */
    ret = read(dev->fd, &value, sizeof(value));
    if (ret < sizeof(value)) {
        return -LDAL_ERROR;
    }

    /* Calibrate analog data */
    if (adev->aicfgs[adev->mode].is_corrected) {
        printf("Calibrate analog data ... %f -> ", value);
        value = fit_linear_model(value, adev->aicfgs[adev->mode].slope, adev->aicfgs[adev->mode].intercept);
        printf("%f\n", value);
    }

    /* Ensure value in range 0-5V or 4-20mA */
    if (ADC_MODE_CURRENT == adev->mode) {
        if (value < ADC_CURRENT_VALUE_MIN - 100) {       /* 3.9mA */
            return -LDAL_EINVAL;
        } else if (value < ADC_CURRENT_VALUE_MIN) {      /* 4.0mA */
            value = ADC_CURRENT_VALUE_MIN;
        } else if (value > ADC_CURRENT_VALUE_MAX) {      /* 20.0mA */
            value = ADC_CURRENT_VALUE_MAX;
        }
    } else if (ADC_MODE_VOLTAGE == adev->mode) {
        if (value < ADC_VOLTAGE_VALUE_MIN) {             /* 0V */
            return -LDAL_EINVAL;
        } else if (value > ADC_VOLTAGE_VALUE_MAX) {      /* 20V */
            value = 0.0;
        }
    }

    //count ai value by value_min and value_max
	if(ai_conf.value_max>ai_conf.value_min) {
		if(ai_conf.current_voltage==0) {//current
			valuef = ((valuef < 4) ? 4 : valuef);
			valuef = ((valuef > 20) ? 20 : valuef);
			valuef = (valuef - 4) / (20 - 4) * (ai_conf.value_max - ai_conf.value_min) + ai_conf.value_min;
		}else {//voltage
			valuef = ((valuef < 0) ? 0 : valuef);
			valuef = ((valuef > 5) ? 5 : valuef);
			valuef = (valuef - 0) / (5 - 0) * (ai_conf.value_max - ai_conf.value_min) + ai_conf.value_min;
		}
    }else {
		valuef = 0.0;
	}
    
    if (len < sizeof(value)) {
        return -LDAL_ERROR;
    }
    memcpy(buf, &value, sizeof(value));

    return LDAL_EOK;
}

/* Unimplentment in driver module */
static int analog_write(struct ldal_device *dev, const void *buf, size_t len)
{
    assert(dev);
    return LDAL_EOK;
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
static int analog_control(struct ldal_device *dev, int cmd, void *arg)
{
    int ret = 0;
#if 0
    struct ai_port * ai_port = (struct ai_port *)dev->parent;
	float valuef = 0.0;
	int size = 0;
	int fd = 0;
    
	

    if (!ai_port) {
        DEVICES_TRACE("config ai %s faild\n",dev->name);
        return -1;
    }

    switch(cmd) {
    case AI_SET_CURR_VOL_VALMAX_VALMIN: {
        struct ai_conf *pai_conf = (struct ai_conf *)arg;
        struct ai_type aiType;
        int i = 0;
        int fd = 0;

        if(!pai_conf) return -1;

        ai_port->ai_conf = *pai_conf;
        aiType.ai_dev = ai_port->pos;
        aiType.type = pai_conf->current_voltage;

        fd=ai_port->fd;
        ret=ioctl(fd, AI_CONF_VOLT_CURR, &aiType);
        if(ret != 0)
            SYSLOG_ERR("ai[%d] config %s error\n", aiType.ai_dev, pai_conf->current_voltage ? "voltage" : "current" );
        }
        break;
    case AI_SET_CORRECT_VOL_CUR: {
        struct ai_correct_cfg *correct_cfg = (struct ai_correct_cfg *)arg;
        ret = anaInVoltCurr_Correct(ai_port,correct_cfg);
        }
        break;
    case AI_GET_CORRECT_VOL_CUR : {
        ret = get_correct_args(ai_port);
        } break;
    default: 
        break;
    }
#endif
    return ret;
}

const struct ldal_device_ops analog_device_ops = 
{
    .init  = analog_init,
    .open  = analog_open,
    .close = analog_close,
    .read  = analog_read,
    .write = analog_write,
    .control = analog_control,
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

    printf("Register analog device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_ANALOG);
}
INIT_CLASS_EXPORT(analog_device_class_register);
