# 编译



## 本地编译

### 安装依赖

```shell
sudo apt update
sudo apt install libev-dev 
sudo apt install linux-tools-common linux-tools-generic linux-tools-`uname -r`
sudo apt install libncurses5-dev
```

### 构建模块

编译 libldal.so 共享库

```shell
make
```

安装 libldal.so 共享库

```shell
make install
```

编译测试模块

```shell
make testcase
```

编译生成的文件存放在 bin 目录下。

### 测试

```shell
echo "{\"method\":\"sayHello\"}" | nc 127.0.0.1 1601
```



## 交叉编译

为了顺利编译，需要创建 libev.so 软链接

```shell
cd /opt/fsl-imx-xwayland/5.4-zeus/sysroots/aarch64-poky-linux/usr/lib
sudo ln -s libev.so.4 libev.so
```

配置交叉编译环境

```shell
source /opt/fsl-imx-xwayland/5.4-zeus/environment-setup-aarch64-poky-linux
```

编译 libldal.so 共享库

```shell
make
```

编译测试模块

```shell
make testcase
```

将编译好的文件传送到开发板

```shell
scp -r ./bin root@192.168.31.47:/home/root/
```

安装共享库

```shell
install libldal.so /usr/lib
```

另外，libldal.so 模块依赖 libev.so，如果 ARM 平台没有，请从下载并安装对应的 deb 包

```shell
dpkg -i libev4_4.27-r0_arm64.deb
dpkg -i libev-dev_4.27-r0_arm64.deb
```

最后，更新系统共享库缓存列表

```shell
ldconfig
```

现在，您可以将 demo 运行起来了

```shell
./bin/udp_server
```

