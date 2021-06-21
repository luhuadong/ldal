#!/bin/bash -e

make clean
make

if [ $1 == "arm" ]; then
    sudo cp bin/libldal.so /opt/fsl-imx-xwayland/5.4-zeus/sysroots/aarch64-poky-linux/usr/lib/
    make testcase
    scp -r ./bin root@192.168.31.47:/home/root/
else
    make testcase
    sudo make install
fi


exit 0
