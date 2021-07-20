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

#define CODE_LEN                         10
#ifndef SINGLE_ARGUMENT_N
#define SINGLE_ARGUMENT_N                30
#endif

#define ADC_CURRENT_VALUE_MIN            4000.0f        /* 4mA */
#define ADC_CURRENT_VALUE_MAX            20000.0f       /* 20mA */
#define ADC_VOLTAGE_VALUE_MIN            0.0f           /* 0V */
#define ADC_VOLTAGE_VALUE_MAX            5000.0f        /* 5V */

#define ADC_CURRENT_CALIB_POINT1         6000.0f        /* 6mA */
#define ADC_CURRENT_CALIB_POINT2         16000.0f       /* 16mA */
#define ADC_VOLTAGE_CALIB_POINT1         1000.0f        /* 1V */
#define ADC_VOLTAGE_CALIB_POINT2         4000.0f        /* 4V */

/* Calibration point */
typedef enum {
    CURRENT_CORRECT_6MA  = 0,         /* 0 : current (6mA)  */
    CURRENT_CORRECT_16MA = 1,         /* 1 : current (16mA) */
    VOLTAGE_CORRECT_1V   = 2,         /* 2 : voltage (1V)   */
    VOLTAGE_CORRECT_4V   = 3,         /* 3 : voltage (4V)   */
} adc_calib_t;

struct calib_point {
    float p1;
    float p2;
};

typedef enum {
    ADC_MODE_CURRENT = 0,
    ADC_MODE_VOLTAGE = 1,
    ADC_MODE_NUM = 2,
} adc_mode_t;

struct ai_conf{
    int current_voltage;                /* ADC mode: 0=current, 1=voltage */
    float value_max, value_min;         /* Count ai value */
    char polcode[CODE_LEN];             /* Code for Air/Water Pollutants */
    char polcode_zs_num;
    char polcode_zs[SINGLE_ARGUMENT_N][CODE_LEN];
};

/* Storage Set and get */
struct ai_calibrate {                   /* $$ Compatible with old code $$ */
    char ai_dev;
    float vol_a;
    float vol_b;
    float cur_a;
    float cur_b;
    unsigned short crc;
};

struct ai_type {                        /* $$ Compatible with old code $$ */
    char ai_dev;
    char type;
};

struct ai_correct_cfg {                 /* $$ Compatible with old code $$ */
    int signalType;                     /* 0:current 1:voltage */
    adc_calib_t valueType;
};

/* 
 * Calibration data, Slope-Intercept Form of current or voltage type.
 * Save the struct to files directly.
 */
struct aicfgs {
    bool is_corrected;                  /* Corrected or not */
    float slope;                        /* Slope of linear model */
    float intercept;                    /* Intercept of linear model */
};

struct ldal_analog_device
{
    char *device_name;
    char *file_name;
    int pos;                            /* AI pos number, for spi protocol */
    adc_mode_t mode;                    /* ADC mode: Current or Voltage */
    struct aicfgs aicfgs[ADC_MODE_NUM]; /* Save AI calibration data */
    struct ai_conf ai_conf;             /* Compatible with old code */
    float calib_data[2];                /* Temporary storage of calibration data  */
    int status;

    struct ldal_device device;
    void *user_data;
};

int analog_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_ANALOG_H__ */
