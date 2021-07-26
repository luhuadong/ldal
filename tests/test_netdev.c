
#include <stdio.h>
#include "ldal.h"

int main(int argc, char *argv[])
{
    int ret;
    char *ifname;
    struct netdev_attr net_attr;
    char mac[MAC_SIZE];

    if (argc > 1) {
        ifname = argv[1];
    }
    else {
        ifname = "eth0";
    }

    ret = ldal_get_ip_attr(ifname, &net_attr);
    if (ret < 0) {
        printf("Get '%s' netdev config information failed\n", ifname);
        return -1;
    }

    get_local_mac(ifname, mac);

    printf(" Netdev: %s\n", ifname);
    printf("    MAC: %s\n", mac);
    printf("IP addr: %s\n", net_attr.ipaddr);
    printf("Netmask: %s\n", net_attr.netmask);
    printf("Gateway: %s\n", net_attr.gateway);
    printf("    DNS: %s\n", net_attr.dns);

    return 0;
}
