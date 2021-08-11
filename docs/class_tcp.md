# TCP 客户端

主要功能：

- TCP Socket Client 创建、关闭、发送、接收
- TCP Socket 参数配置
- 绑定网卡



## control 接口

设置允许地址的立即重用

```
SOCKET_SET_REUSEADDR
```



将 socket 绑定到指定网卡

```
SOCKET_SET_NETDEV
```



设置是否开启保持连接

```
SOCKET_SET_KEEPALIVE
```



设置接收超时

```
SOCKET_SET_RECVTIMEO
```



设置发送超时

```
SOCKET_SET_SENDTIMEO
```

