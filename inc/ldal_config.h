#ifndef __LDAL_CONFIG_H__
#define __LDAL_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LDAL_VERSION           "0.0.1"

#define LDAL_NAME_MAX          32
#define ANALOG_CALIB_DIR       "/etc/aicfgs/"

#ifndef MN_LENGTH
#define MN_LENGTH              (25)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_CONFIG_H__ */