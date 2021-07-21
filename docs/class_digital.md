# 数字量设备

读取离散输入端口 DI6 的值

```shell
root@imx8mqevk:~/ldal# ./test/test_di 6
Digital Port Test Start
Device: DI6 -> /dev/didev6
Reading digital port ...
> 0
> 0
> 0
> 0
> 0
Digital Port Test End
```

测试输出端口 DI0 ~ DI7 继电器

```shell
root@imx8mqevk:~/ldal# ./test/test_do
Turn ON digital port...
Device: DO0 -> /dev/dodev0
Device: DO1 -> /dev/dodev1
Device: DO2 -> /dev/dodev2
Device: DO3 -> /dev/dodev3
Device: DO4 -> /dev/dodev4
Device: DO5 -> /dev/dodev5
Device: DO6 -> /dev/dodev6
Device: DO7 -> /dev/dodev7
Turn OFF digital port...
Device: DO0 -> /dev/dodev0
Device: DO1 -> /dev/dodev1
Device: DO2 -> /dev/dodev2
Device: DO3 -> /dev/dodev3
Device: DO4 -> /dev/dodev4
Device: DO5 -> /dev/dodev5
Device: DO6 -> /dev/dodev6
Device: DO7 -> /dev/dodev7
Digital Port Test End
```
