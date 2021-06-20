#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ldal_analog.h"

#define ANALOG_CALIB_DIR            "/etc/aicfgs/"

#define AI_CONF_VOLT_CURR           _IOW('k',1,int)
#define AI_SET_VOLT_CURR_CAL        _IOW('k',5,int)
#define AI_GET_VOLT_CURR_CAL        _IOW('k',6,int)
#define AI_SET_ANALOG_MODE          _IOW('T',7,int)
#define AI_GET_ANALOG_VALUE         _IOW('T',8,int)

#if 1
static struct calib_point g_calib_point[ADC_MODE_NUM] = {
    { ADC_CURRENT_CALIB_POINT1, ADC_CURRENT_CALIB_POINT2 },
    { ADC_VOLTAGE_CALIB_POINT1, ADC_VOLTAGE_CALIB_POINT2 },
};
#else
static struct calib_point g_calib_point[ADC_MODE_NUM];
#endif

static unsigned CRC16_212(char* buf, int len)
{
    unsigned int r;
    unsigned char hi;
    char flag;
    int i, j;
    
    r = 0xffff;
    for(j=0; j<len; j++) {
        hi = r >> 8;
        hi ^= buf[j];
        r = hi;
        
        for(i=0; i<8; i++) {
            flag = r & 0x0001;
            r = r >> 1;
            if(flag == 1) r ^= 0xa001;
        }
    }
    return r;
}

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

    strncpy(pathname, ANALOG_CALIB_DIR, strlen(ANALOG_CALIB_DIR) + 1);
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

    strncpy(pathname, ANALOG_CALIB_DIR, strlen(ANALOG_CALIB_DIR) + 1);
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
    adev->aicfgs[ADC_MODE_CURRENT].slope = 1.0f;
    adev->aicfgs[ADC_MODE_CURRENT].intercept = 0.0f;
    adev->aicfgs[ADC_MODE_VOLTAGE].is_corrected = false;
    adev->aicfgs[ADC_MODE_VOLTAGE].slope = 1.0f;
    adev->aicfgs[ADC_MODE_VOLTAGE].intercept = 0.0f;

    /* Import calib data if existed */
    get_calib_data_from_file(dev, &adev->aicfgs, sizeof(adev->aicfgs));

    adev->ai_conf.current_voltage = ADC_MODE_CURRENT;
    adev->ai_conf.value_max = 100.0f;
    adev->ai_conf.value_min = 0.0f;

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

/* 0~5V or 4~20mA */
static int analog_read(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    float value = 0.0;
    struct ldal_analog_device *adev = (struct ldal_analog_device *)dev->user_data;

    /* Check buffer length */
    if (len < sizeof(value)) {
        return -LDAL_ERROR;
    }

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
            value = 0.0;
            //return -LDAL_EINVAL;
        } else if (value < ADC_CURRENT_VALUE_MIN) {      /* 4.0mA */
            value = ADC_CURRENT_VALUE_MIN;
        } else if (value > ADC_CURRENT_VALUE_MAX) {      /* 20.0mA */
            value = ADC_CURRENT_VALUE_MAX;
        }
    } else if (ADC_MODE_VOLTAGE == adev->mode) {
        if (value < ADC_VOLTAGE_VALUE_MIN) {             /* 0V */
            value = 0.0;
            //return -LDAL_EINVAL;
        } else if (value > ADC_VOLTAGE_VALUE_MAX) {      /* 5V */
            value = ADC_VOLTAGE_VALUE_MAX;
        }
    }

    //count ai value by value_min and value_max
	if (adev->ai_conf.value_max > adev->ai_conf.value_min) {

        float delta = adev->ai_conf.value_max - adev->ai_conf.value_min;
        float base = adev->ai_conf.value_min;

		if (adev->ai_conf.current_voltage == ADC_MODE_CURRENT) {
			value = (value - ADC_CURRENT_VALUE_MIN) / (ADC_CURRENT_VALUE_MAX - ADC_CURRENT_VALUE_MIN) / 1000.0f * delta + base;
		} else {
			value = (value - ADC_VOLTAGE_VALUE_MIN) / (ADC_VOLTAGE_VALUE_MAX - ADC_VOLTAGE_VALUE_MIN) / 1000.0f * delta + base;
		}
    } else {
		value = 0.0f;
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

#define AI_AVERAGE_N 10

static int get_ai_average(int fd, float *value)
{
    int size = 0, count = 0, loop = 0;
    float valuef = 0.0f, valueMin = 0.0f, valueMax = 0.0f;
    double valueSum = 0.0;

    for (loop=0; loop<AI_AVERAGE_N; loop++) {

        size = read(fd, &valuef, sizeof(valuef));

        if(size < sizeof(valuef))
            continue;
            
        if (loop == 0) {
            valueMin = valuef;
            valueMax = valuef;
        } else {
            if (valueMin > valuef) {
                valueMin = valuef;
            } else if (valueMax < valuef) {
                valueMax = valuef;
            }
        }
        valueSum += valuef;
        count++;
        usleep(50 * 1000);  /* delay 50ms */
    }
    *value = (count > 2) ? ((valueSum - valueMin - valueMax) / (count -2)) : valuef;
	return 0;
}

static int calib_adc_value(struct ldal_analog_device *adev, int type)
{
    int ret, mode;

    if (type & (1<<1)) {
        mode = ADC_MODE_VOLTAGE;
    } else {
        mode = ADC_MODE_CURRENT;
    }

    if (type & (1<<0)) {
        /* Second point */
        ret = get_ai_average(adev->device.fd, &adev->calib_data[1]);
        if (ret < 0) {
            return -LDAL_ERROR;
        }

        struct aicfgs aicfgs[ADC_MODE_NUM];
        memcpy(aicfgs, adev->aicfgs, sizeof(adev->aicfgs));
        
        aicfgs[mode].slope = (g_calib_point[mode].p2 - g_calib_point[mode].p1) / (adev->calib_data[1] - adev->calib_data[0]);
        aicfgs[mode].intercept = g_calib_point[mode].p2 - aicfgs[mode].slope * adev->calib_data[1];
        aicfgs[mode].is_corrected = true;

        /* Save calib data to file */
        ret = save_calib_data_to_file(&adev->device, &adev->aicfgs, sizeof(adev->aicfgs));
        if (ret < 0) {
            return -LDAL_ERROR;
        }

        /* Update calib data when saved successfully */
        adev->aicfgs[mode].slope = aicfgs[mode].slope;
        adev->aicfgs[mode].intercept = aicfgs[mode].intercept;
        adev->aicfgs[mode].is_corrected = true;

        /* Update MCU data */
        struct ai_calibrate ai_cal;
        ai_cal.ai_dev = adev->pos;
        ai_cal.vol_a = adev->aicfgs[ADC_MODE_VOLTAGE].slope;
        ai_cal.vol_b = adev->aicfgs[ADC_MODE_VOLTAGE].intercept;
        ai_cal.cur_a = adev->aicfgs[ADC_MODE_CURRENT].slope;
        ai_cal.cur_b = adev->aicfgs[ADC_MODE_CURRENT].intercept;
        ai_cal.crc = CRC16_212((char *)&ai_cal.vol_a, 16);

        ret = ioctl(adev->device.fd, AI_SET_VOLT_CURR_CAL, &ai_cal);
    } else {
        /* First point */
        return get_ai_average(adev->device.fd, &adev->calib_data[0]);
    }

    return LDAL_EOK;
}

/*
 * Get calib from MCU
 * |__ If MCU has data, restore to file and device object
 * |__ If MCU hasn't data, get from file and sync to MCU
 */
static int restore_adc_value(struct ldal_analog_device *adev)
{
#define READ_TRY_COUNT    5

    struct ai_calibrate ai_cal;
    int try, ret;

    get_calib_data_from_file(&adev->device, &adev->aicfgs, sizeof(adev->aicfgs));

    for (try=0; try<READ_TRY_COUNT; try++) {	//try 5 times to get AI calibrates.
        if (0 == ioctl(adev->device.fd, AI_GET_VOLT_CURR_CAL, &ai_cal)) {
            break;
        } else {
            usleep(500* 1000);  /* delay 500ms */
            continue;
        }
    }

    if (try < READ_TRY_COUNT) {
        uint16_t CRC = CRC16_212((char *)&ai_cal.vol_a, 16);
        if (CRC != ai_cal.crc) {
            return -LDAL_ERROR;
        }
        /* MCU data is valid */
        adev->aicfgs[ADC_MODE_CURRENT].slope = ai_cal.cur_a;
        adev->aicfgs[ADC_MODE_CURRENT].intercept = ai_cal.cur_b;
        adev->aicfgs[ADC_MODE_CURRENT].is_corrected = true;
        adev->aicfgs[ADC_MODE_VOLTAGE].slope = ai_cal.vol_a;
        adev->aicfgs[ADC_MODE_VOLTAGE].intercept = ai_cal.vol_b;
        adev->aicfgs[ADC_MODE_VOLTAGE].is_corrected = true;
        ret = save_calib_data_to_file(&adev->device, &adev->aicfgs, sizeof(adev->aicfgs));
    } else {
        struct ai_calibrate ai_cal_sync;
        ai_cal_sync.ai_dev = adev->pos;
        ai_cal_sync.vol_a = adev->aicfgs[ADC_MODE_VOLTAGE].slope;
        ai_cal_sync.vol_b = adev->aicfgs[ADC_MODE_VOLTAGE].intercept;
        ai_cal_sync.cur_a = adev->aicfgs[ADC_MODE_CURRENT].slope;
        ai_cal_sync.cur_b = adev->aicfgs[ADC_MODE_CURRENT].intercept;
        ai_cal_sync.crc = CRC16_212((char *)&ai_cal_sync.vol_a, 16);
        ret = ioctl(adev->device.fd, AI_SET_VOLT_CURR_CAL, &ai_cal_sync);
    }

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
static int analog_control(struct ldal_device *dev, int cmd, void *arg)
{
    assert(dev);

    int ret = 0;
    struct ldal_analog_device *adev = (struct ldal_analog_device *)dev->user_data;

    switch(cmd) {
    case AI_SET_CURR_VOL_VALMAX_VALMIN: 
    {
        struct ai_conf *pai_conf = (struct ai_conf *)arg;
        struct ai_type aiType;

        if (!pai_conf)
            return -LDAL_EINVAL;

        adev->ai_conf = *pai_conf;
        aiType.ai_dev = adev->pos;
        aiType.type = pai_conf->current_voltage;

        ret = ioctl(dev->fd, AI_CONF_VOLT_CURR, &aiType);
    } break;

    case AI_SET_CORRECT_VOL_CUR: 
    {
        struct ai_correct_cfg *correct_cfg = (struct ai_correct_cfg *)arg;
        
        if (!correct_cfg)
            return -LDAL_EINVAL;
        
        ret = calib_adc_value(adev, correct_cfg->valueType);
    } break;

    case AI_GET_CORRECT_VOL_CUR : 
    {
        /* Restore calib data from file or MCU */
        ret = restore_adc_value(adev);
    } break;

    default: 
        ret = -LDAL_EINVAL;
        break;
    }

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
