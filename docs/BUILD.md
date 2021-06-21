# 编译



安装依赖

```shell
sudo apt update
sudo apt install libev-dev 
sudo apt install linux-tools-common linux-tools-generic linux-tools-`uname -r`
sudo apt install libncurses5-dev
```



测试

```shell
echo "{\"method\":\"sayHello\"}" | nc 127.0.0.1 1601
```

