# 串口设备



## 串口配置

为兼容原有代码，仍然使用 port_option 结构体传递串口配置参数。串口号在设备注册时确定，启动后无法更改。其他参数包括波特率、数据位、校验位、停止位、流控，可以通过 control_device() 接口传入 `SERIAL_SET_OPTIONS` 命令和 port_option 结构体进行更改。

```c
struct port_option {
    unsigned int baudrate;     /* Baudrate:  */
    unsigned char data_bits;   /* ByteSize: 5, 6, 7, 8 */
    ser_parity_t parity;       /* Parity: None, Odd, Even, Mark, Space */
    ser_stopbits_t stop_bits;  /* StopBits: 1, 1.5, 2  */
    ser_flowctrl_t flowctrl;   /* Flow Control: SW, HW, No Ctrl Flow */
    struct flow_ctrl fcopt;
	char MN[25];
	int addr;
};
```

### 波特率（Baudrate）

下面列出波特率的配置值。需要注意的是，可配置并不意味着该波特率在您的系统中有效，请根据控制器和接入的仪表设备选择合适的波特率。设备抽象层的默认配置为 B115200，即 115200bps。

```c
#define  B0        0000000      /* hang up */
#define  B50       0000001
#define  B75       0000002
#define  B110      0000003
#define  B134      0000004
#define  B150      0000005
#define  B200      0000006
#define  B300      0000007
#define  B600      0000010
#define  B1200     0000011
#define  B1800     0000012
#define  B2400     0000013
#define  B4800     0000014
#define  B9600     0000015
#define  B19200    0000016
#define  B38400    0000017
#define  B57600    0010001      /* Extra output baud rates (not in POSIX) */
#define  B115200   0010002
#define  B230400   0010003
#define  B460800   0010004
#define  B500000   0010005
#define  B576000   0010006
#define  B921600   0010007
#define  B1000000  0010010
#define  B1152000  0010011
#define  B1500000  0010012
#define  B2000000  0010013
#define  B2500000  0010014
#define  B3000000  0010015
#define  B3500000  0010016
#define  B4000000  0010017
#define __MAX_BAUD B4000000
```

### 数据位（ByteSize）

串口的数据位表示传输数据的位数，范围是 5 ~ 8，设备抽象层的默认配置为 8 位。对应的宏定义如下：

```c
#define   CS5   0000000
#define   CS6   0000020
#define   CS7   0000040
#define   CS8   0000060
```

注意：由于串行通信在空闲的时候，总线上的电平为高电平（逻辑1），因此在开始传输数据时，要先把总线上的电平拉低一个时间单位（逻辑0），称为 1 个起始位。该起始位不需要设置，默认为 1 位，包含在数据位中。也就是说，数据位为 8 时，有效数据为 7 位。

### 校验位（Parity）

校验是数据传送时采用的一种检查数据是否错误的方式，在串口通信中，可以选择下列几种校验位。

| 校验位            | 说明                                                         |
| ----------------- | ------------------------------------------------------------ |
| 奇校验（Odd）     | 数据在传输过程中，为 1 的位（bit）的总数应为奇数。<br/>若不是奇数，则校验位 = 1<br/>若是奇数，则校验位 = 0 |
| 偶校验（Even）    | 数据在传输过程中，为 1 的位（bit）的总数应为偶数。<br/>若不是偶数，则校验位 = 1<br/>若是偶数，则校验位 = 0 |
| 空位校验（Space） | 校验位 = 0                                                   |
| 标记校验（Mark）  | 校验位 = 1                                                   |
| 无校验（None）    | 不使用校验位                                                 |

设备抽象层使用枚举类型限定该值，具体定义如下。

```c
typedef enum {
    PAR_NONE = 0,
    PAR_EVEN = 1,
    PAR_ODD = 2,
} ser_parity_t;
```

校验位是非必须的，设备抽象层默认设置为 None，也就是 0 位。

### 停止位（StopBits）

由于受时钟精度、分频系数等影响，波特率可能存在误差，并且在一帧数据的传输过程中，这种误差会累积。而使用停止位，可以清除累积误差，避免影响下一帧数据。停止位也是以时间长度来衡量的，通常可以选择 1 位、1.5 位或者 2 位。

设备抽象层使用枚举类型限定该值，具体定义如下。

```c
typedef enum {
    BITS_1P0 = 1,
    BITS_1P5 = 0,
    BITS_2P0 = 2,
} ser_stopbits_t;
```

设备抽象层默认使用 1 位停止位。

### 流控制（Flow Control）

流控的开启和关闭通过下面枚举类型控制。

```c
typedef enum {
    FC_OFF = 0,
    FC_ON = 1,
} ser_flowctrl_t;
```

流控的类型由结构体 flow_ctrl 控制，该部分兼容原有代码。

```c
struct flow_ctrl {
    unsigned char dtr_dsr;
    unsigned char rts_cts;
    unsigned char xon_xoff;
};
```

设备抽象层默认不启用流控。

## 阻塞超时

LDAL 的串口设备支持配置阻塞超时时间，默认为一直阻塞。如果需要设置串口读取的超时时间，可以通过 `SERIAL_READ_TIMEOUT` 命令传入超时时间，单位为毫秒。

例如：

```c
control_device(serial, SERIAL_READ_TIMEOUT, 3000); /* 3s */
```

