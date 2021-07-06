#ifndef __K37X_SAMPLE_H__
#define __K37X_SAMPLE_H__

#define DEV_SERIAL_NAME  "serial"
#define DEV_SERIAL0      "serial0"
#define DEV_SERIAL1      "serial1"
#define DEV_SERIAL2      "serial2"
#define DEV_SERIAL3      "serial3"
#define DEV_SERIAL4      "serial4"
#define DEV_SERIAL5      "serial5"
#define DEV_SERIAL6      "serial6"
#define DEV_SERIAL7      "serial7"
#define DEV_SERIAL8      "serial8"

#define DEV_SERIAL485_10 "serial10"
#define DEV_SERIAL485_11 "serial11"
#define DEV_SERIAL485_12 "serial12"

#define DEV_SERIAL485_20 "serial20"
#define DEV_SERIAL485_21 "serial21"
#define DEV_SERIAL485_22 "serial22"

#define DEV_SERIAL485_30 "serial30"
#define DEV_SERIAL485_31 "serial31"
#define DEV_SERIAL485_32 "serial32"

#define DEV_SERIAL485_40 "serial40"
#define DEV_SERIAL485_41 "serial41"
#define DEV_SERIAL485_42 "serial42"

#define DEV_ETH_NAME     "client"
#define DEV_ETH_CLIENT1  "client1"
#define DEV_ETH_CLIENT2  "client2"
#define DEV_ETH_CLIENT3  "client3"
#define DEV_ETH_CLIENT4  "client4"
#define DEV_ETH_CLIENT5  "client5"
#define DEV_ETH_CLIENT6  "client6"
#define DEV_ETH_CLIENT7  "client7"
#define DEV_ETH_CLIENT8  "client8"
#define DEV_ETH_CLIENT9  "client9" //our company server

#define DEV_ETH_CLIENT_MAX_NUMBER  (9)


#define DEV_ETH_CLIENT100    "client100" //our configure client for configure dev eth
#define DEV_ETH_CLIENT101    "client101" // to synchronize time with server 132.163.4.102:13
#define DEV_ETH_BACKLOG      3
#define DEV_ETH_NAME_S       "server"
#define DEV_ETH_SERVER1      "server1"   // to handle Modbus TCP
#define DEV_ETH_SERVER2      "server2"   // to handle Modbus TCP
#define DEV_ETH_SERVER3      "server3"   // to handle Modbus TCP

#define PIC_DEV_ETH_NAME     "pic_client"
#define PIC_DEV_ETH_CLIENT1  "pic_client1"
#if 0 //now only one link to handle picture upload
#define PIC_DEV_ETH_CLIENT2  "pic_client2"
#define PIC_DEV_ETH_CLIENT3  "pic_client3"
#define PIC_DEV_ETH_CLIENT4  "pic_client4"
#define PIC_DEV_ETH_CLIENT5  "pic_client5"
#define PIC_DEV_ETH_CLIENT6  "pic_client6"
#define PIC_DEV_ETH_CLIENT7  "pic_client7"
#define PIC_DEV_ETH_CLIENT8  "pic_client8"
#define PIC_DEV_ETH_CLIENT9  "pic_client9"
#endif

#define DEV_AI_NAME   "ai"
#define DEV_AI0       "ai0"
#define DEV_AI1       "ai1"
#define DEV_AI2       "ai2"
#define DEV_AI3       "ai3"
#define DEV_AI4       "ai4"
#define DEV_AI5       "ai5"
#define DEV_AI6       "ai6"
#define DEV_AI7       "ai7"
#define DEV_AI8       "ai8"
#define DEV_AI9       "ai9"
#define DEV_AI10      "ai10"
#define DEV_AI11      "ai11"
#define DEV_AI12      "ai12"
#define DEV_AI13      "ai13"
#define DEV_AI14      "ai14"
#define DEV_AI15      "ai15"
#define DEV_AI16      "ai16"

#define DEV_DO_NAME   "do"
#define DEV_DO0       "do0"
#define DEV_DO1       "do1"
#define DEV_DO2       "do2"
#define DEV_DO3       "do3"
#define DEV_DO4       "do4"
#define DEV_DO5       "do5"
#define DEV_DO6       "do6"
#define DEV_DO7       "do7"

#define DEV_DI_NAME   "di"
#define DEV_DI0       "di0"
#define DEV_DI1       "di1"
#define DEV_DI2       "di2"
#define DEV_DI3       "di3"
#define DEV_DI4       "di4"
#define DEV_DI5       "di5"
#define DEV_DI6       "di6"
#define DEV_DI7       "di7"

#define DEV_RTC0      "rtc"
#define DEV_ME_NAME   "me"
#define DEV_ME_PORT0  "me_port0"
#define DEV_ME_PORT1  "me_port1"
#define DEV_ME_PORT2  "me_port2"
#define DEV_ME_PORT3  "me_port3"


#define DEV_MCU_LOCKER            "k37adev_locker"
#define DEV_MCU_DOOR              "k37adev_door"
#define DEV_MCU_IBUTTON           "k37adev_ibutton" // read ibutton code from mcu
#define DEV_MCU_SYNC_IBTN         "k37adev_sync"
#define DEV_MCU_USER_LOGIN        "k37adev_user"    // only get current login user code from mcu
#define DEV_MCU_LOG               "k37adev_log"
#define DEV_MCU_SYNC_TIME         "k37adev_time"
#define DEV_MCU_TEMPERATURE       "k37adev_temperature"
#define DEV_MCU_BETTERY           "k37adev_battery"
#define DEV_PERPHL_CONDITIONER    "k37aperphl_conditioner"
#define DEV_PERPHL_DOOR           "k37aperphl_door"
#define DEV_PERPHL_TEMPERATURE    "k37aperphl_temperature"
#define DEV_PERPHL_HUMIDITY       "k37aperphl_humidity"
#define DEV_PERPHL_CONTROLLERS    "k37aperphl_controllers"
#define DEV_PERPHL_USER           "k37aperphl_user" // room user of door

#define DEV_GPIO_OUT_RUN_LED      "run_led"      /*gpio2-0*/
#define DEV_GPIO_OUT_ACT_LED      "act_led"      /*gpio2-1*/
#define DEV_GPIO_OUT_GPRS_PWRCTRL "gprs_pwrctrl" /*gpio0-19*/
#define DEV_GPIO_OUT_NOT_USED1    "not_used1"    /*gpio1-13*/
#define DEV_GPIO_OUT_GPRS_ACT     "gprs_active"  /*gpio1-12*/
#define DEV_GPIO_OUT_USB_PWR      "usb_pwr"      /*gpio3-13*/
#define DEV_GPIO_OUT_LCD_PWR      "lcd_pwr"      /*gpio0-22*/

#endif /* __K37X_SAMPLE_H__ */
