
#include <stdio.h>
#include <unistd.h>
#include "ldal.h"

static void show_netdev_info(const char *ifname)
{
    struct netdev_attr net_attr;
    char mac[MAC_SIZE];

    if (0 > ldal_get_ip_attr(ifname, &net_attr)) {
        printf("Get '%s' netdev config information failed\n", ifname);
        return ;
    }

    get_local_mac(ifname, mac);

    printf(" Netdev: %s\n", ifname);
    printf("    MAC: %s\n", mac);
    printf("IP addr: %s\n", net_attr.ipaddr);
    printf("Netmask: %s\n", net_attr.netmask);
    printf("Gateway: %s\n", net_attr.gateway);
    printf("    DNS: %s\n", net_attr.dns);
}

int main(int argc, char *argv[])
{
    char *ifname;
    struct netdev_attr net_attr;

    if (argc < 6) {
        printf("Usage: %s <ifname> <ip> <netmask> <gateway> <dns>\n", argv[0]);
        return -1;
    }

    ifname = argv[1];
    strncpy(net_attr.ipaddr,  argv[2], IP_SIZE);
    strncpy(net_attr.netmask, argv[3], IP_SIZE);
    strncpy(net_attr.gateway, argv[4], IP_SIZE);
    strncpy(net_attr.dns,     argv[5], IP_SIZE);

    show_netdev_info(ifname);

    printf("Setting...\n");
    ldal_set_ip_attr(ifname, &net_attr);
    sleep(2);

    show_netdev_info(ifname);

    return 0;
}
