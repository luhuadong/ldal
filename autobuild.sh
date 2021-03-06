#!/bin/bash -e

NFSDIR=/srv/nfs/rootfs-docker/home/root/

echo $1

sleep 3

make clean
make

if [ "$1" == "arm" ]; then
    sudo cp bin/libldal.so /opt/fsl-imx-xwayland/5.4-zeus/sysroots/aarch64-poky-linux/usr/lib/
    make testcase
    make tool
    #scp -r ./bin root@192.168.31.47:/home/root/
    cp -r ./bin ${NFSDIR}
else
    make testcase
    sudo make install
    make tool
fi


exit 0
