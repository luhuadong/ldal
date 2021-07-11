
#include <stdio.h>
#include "ldal.h"

#if 0
static struct netdev_cfg ipcfg;

int main(int argc, char *argv[])
{
    int ret;
    char *ifname;

    if (argc > 1) {
        ifname = argv[1];
    }
    else {
        ifname = "eth0";
    }
    
    ret = get_local_ip(ifname, &ipcfg);
    if (ret < 0) {
        printf("Get '%s' netdev config information failed\n", ifname);
        return -1;
    }

    printf("Netdev : %s\n", ifname);
    printf("ip addr: %s\n", ipcfg.ipaddr);
    printf("netmask: %s\n", ipcfg.netmask);
    printf("gateway: %s\n", ipcfg.gateway);

    return 0;
}
#endif

typedef struct _NET_WORK_ATTR
{
    char name[NAME_SIZE];
    char ip[IP_SIZE];
    char mac[MAC_SIZE];
    char netmask_addr[MASK_SIZE];
    char gateway[GATEWAY_SIZE];
    char dns[IP_SIZE];
    
} NET_WORK_ATTR;

static struct netdev_attr net_attr;

/****** main test **********/
int main(int argc, char *argv[])
{
    int ret;
    char *ifname;

    NET_WORK_ATTR net_attr = {0};

    if (argc > 1) {
        ifname = argv[1];
    }
    else {
        ifname = "eth0";
    }

    sprintf(net_attr.name, ifname);

    get_local_mac(net_attr.name, net_attr.mac);
    get_local_ip(net_attr.name, net_attr.ip);
    get_local_netmask(net_attr.name, net_attr.netmask_addr);
    get_local_gateway(net_attr.gateway);
    get_local_dns(net_attr.dns);

    printf(" Netdev: %s\n", ifname);
    printf("    MAC: %s\n", net_attr.mac);
    printf("IP addr: %s\n", net_attr.ip);
    printf("Netmask: %s\n", net_attr.netmask_addr);
    printf("Gateway: %s\n", net_attr.gateway);
    printf("    DNS: %s\n", net_attr.dns);

    return 0;
}

