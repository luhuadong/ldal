#ifndef __LDAL_DEVICE_ANALOG_H__
#define __LDAL_DEVICE_ANALOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "ldal.h"

#define AI_SET_CURR_VOL_VALMAX_VALMIN    0x101
#define AI_SET_CORRECT_VOL_CUR           0x102
#define AI_GET_CORRECT_VOL_CUR           0x103

#define CODE_LEN 10
#ifndef SINGLE_ARGUMENT_N
#define SINGLE_ARGUMENT_N 30
#endif

typedef enum {
    ADC_MODE_CURRENT = 0,
    ADC_MODE_VOLTAGE = 1,
    ADC_MODE_NUM = 2,
} adc_mode_t;

struct ai_correct_args {
	int is_sync;
    int is_corrected;//0 : not correct ,1 : correct
    float a,b;       //for analog voltage or current input correct.
};

struct ai_conf{
    int current_voltage;//  0=current,1=voltage
    float value_max,value_min; //count ai value
    char polcode[CODE_LEN];
    char polcode_zs_num;
    char polcode_zs[SINGLE_ARGUMENT_N][CODE_LEN];
};

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



/* Calibration data, Slope-Intercept Form of current and voltage type */
struct aicfgs {
    bool is_corrected;
    float slope;
    float intercept;
};

struct ldal_analog_device
{
    char *device_name;
    char *file_name;
    int status;
    int pos;                            /* AI pos number, for spi protocol */
    adc_mode_t mode;                    /* Current or Voltage */
    struct aicfgs aicfgs[ADC_MODE_NUM]; /* Save AI calibration data */
    struct ai_conf ai_conf;

    struct ldal_device device;
    void *user_data;
};

int analog_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_ANALOG_H__ */