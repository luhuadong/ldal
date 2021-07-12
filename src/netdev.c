/*
 * /proc/net/route
 */

#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <algorithm>
#include <net/if.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <net/if_arp.h>  /* ARPHRD_ETHER */
#include <resolv.h>

#include "ldal.h"

#define NONE_IP  "0.0.0.0"

// 获取本机mac
bool get_local_mac(const char *ifname, char *mac)
{
    struct ifreq ifr;
    int sd;
    
    bzero(&ifr, sizeof(struct ifreq));
    if( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("get %s mac address socket creat error\n", ifname);
        return false;
    }
    
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);

    if(ioctl(sd, SIOCGIFHWADDR, &ifr) < 0) {
        printf("get %s mac address error\n", ifname);
        close(sd);
        return false;
    }
 
    snprintf(mac, MAC_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
        (unsigned char)ifr.ifr_hwaddr.sa_data[0], 
        (unsigned char)ifr.ifr_hwaddr.sa_data[1],
        (unsigned char)ifr.ifr_hwaddr.sa_data[2], 
        (unsigned char)ifr.ifr_hwaddr.sa_data[3],
        (unsigned char)ifr.ifr_hwaddr.sa_data[4],
        (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
 
    close(sd);
    return true;
}

bool set_local_mac(const char *ifname, const char *mac)
{
    int sd, ret;
    struct ifreq ifr;
    int values[6];
    
    if((0 != getuid()) && (0 != geteuid())) {
        return false;
    }

    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return false;
    }

    strcpy(ifr.ifr_name, ifname);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

    ret = sscanf(mac, "%x:%x:%x:%x:%x:%x%*c", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]);
    if(6 > ret) {
        printf("invalid mac address\n");
        return false;
    }

    /* convert and save to ifr_hwaddr */
    for(int i=0; i<6; i++) {
        ifr.ifr_hwaddr.sa_data[0] = (uint8_t)values[0];
        ifr.ifr_hwaddr.sa_data[1] = (uint8_t)values[1];
        ifr.ifr_hwaddr.sa_data[2] = (uint8_t)values[2];
        ifr.ifr_hwaddr.sa_data[3] = (uint8_t)values[3];
        ifr.ifr_hwaddr.sa_data[4] = (uint8_t)values[4];
        ifr.ifr_hwaddr.sa_data[5] = (uint8_t)values[5];
    }
    
    if(ioctl(sd, SIOCSIFHWADDR, &ifr) < 0) {
        printf("get %s mac address error\n", ifname);
        close(sd);
        return false;
    }
    
    close(sd);
    return true;
}

// 获取本机ip
bool get_local_ip(const char *ifname, char *ip)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;
 
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd) {
        printf("socket error: %s\n", strerror(errno));
        return false;		
    }

    //bzero(ifr.ifr_name, sizeof(ifr.ifr_name));
    //strncpy(ifr.ifr_name, ifName,sizeof(ifr.ifr_name)-1);
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    
    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0) {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return false;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ip, IP_SIZE, "%s", inet_ntoa(sin.sin_addr));
    //snprintf(ip, IP_SIZE, "%s", inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

    close(sd);
    return true;
}

bool set_local_ip(const char *ifname, const char *ip)
{
    int sd;
    struct ifreq ifr;
    struct sockaddr_in *sin;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd) {
        printf("socket error: %s\n", strerror(errno));
        return false;		
    }

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    sin->sin_addr.s_addr = inet_addr(ip);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(sd, SIOCSIFADDR, &ifr) < 0) {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return false;
    }

    close(sd);
    return true;
}

//获取本机mask
bool get_local_netmask(const char *ifname, char *netmask_addr)
{
    int sd;  
    struct ifreq ifr;  
          
    sd = socket(AF_INET, SOCK_STREAM, 0);  
    if (sd == -1) {  
        perror("create socket failture... get_local_netmask\n");  
        return false;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);

    if ((ioctl(sd, SIOCGIFNETMASK, &ifr)) < 0 ) {
        printf("netmask ioctl error\n");
        close(sd);
        return false;
    }
    
    strncpy(netmask_addr, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_netmask)->sin_addr), IP_SIZE);
    close(sd);
    return true;
}

bool set_local_netmask(const char *ifname, const char *netmask_addr)
{
    int sd;
    struct ifreq ifr;
    struct sockaddr_in *sin_net_mask;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Not create network socket connect\n");
        return false;
    }  

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);

    sin_net_mask = (struct sockaddr_in *)&ifr.ifr_addr;
    sin_net_mask->sin_family = AF_INET;
    inet_pton(AF_INET, netmask_addr, &sin_net_mask->sin_addr);

    if (ioctl(sd, SIOCSIFNETMASK, &ifr) < 0) {
        printf("sock_netmask ioctl error\n");
        close(sd);
        return false;
    }

    close(sd);
    return true;
}

//获取本机gateway
bool get_local_gateway(char* gateway)
{  
    FILE *fp;
    char buf[512];
    char cmd[128];
    char *tmp;
  
    strcpy(cmd, "ip route");
    fp = popen(cmd, "r");
    if (NULL == fp)
    {
        perror("popen error");  
        return false;  
    }
    while (fgets(buf, sizeof(buf), fp) != NULL)  
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
            ++ tmp;
        if(strncmp(tmp, "default", strlen("default")) == 0)
            break;
    }
    sscanf(buf, "%*s%*s%s", gateway);
    pclose(fp);

    return true;
}

bool set_local_gateway(const char *gateway)
{
    int ret = 0;
    char cmd[128];
    char old_gateway[GATEWAY_SIZE] = {0};

    get_local_gateway(old_gateway);

    strcpy(cmd, "route del default gw ");
    strcat(cmd, old_gateway);
    ret = system(cmd);

    if(ret < 0) {
        perror("route error");
        return false;
    }
    strcpy(cmd, "route add default gw ");
    strcat(cmd, gateway);

    ret = system(cmd);
    if(ret < 0) {
        perror("route error");
        return false;
    }

    return true;  
}

bool get_local_dns(char* dns_addr)
{
    struct sockaddr_in sin;

    res_state res = malloc(sizeof(struct __res_state));
    res_ninit(res);
    if (res->nscount < 1) {
        return false;
    }

    memcpy(&sin, &res->nsaddr_list[0], sizeof(sin));
    snprintf(dns_addr, IP_SIZE, "%s", inet_ntoa(sin.sin_addr));

    res_nclose(res);
    if (res)
        free(res);

    return true;
}

bool set_local_dns(const char* dns_addr)
{
    char dnsconf[100] = {0};

    snprintf(dnsconf, sizeof(dnsconf), "echo 'nameserver %s' > /etc/resolv.conf", dns_addr);
    system(dnsconf);

    return true;
}

int ldal_get_ip_attr(const char *ifname, netdev_attr_t *attr)
{
    if (!get_local_ip(ifname, attr->ipaddr)) {
        strncpy(attr->ipaddr, NONE_IP, IP_SIZE);
    }

    if (!get_local_netmask(ifname, attr->netmask)) {
        strncpy(attr->netmask, NONE_IP, IP_SIZE);
    }

    if (!get_local_gateway(attr->gateway)) {
        strncpy(attr->gateway, NONE_IP, IP_SIZE);
    }

    if (!get_local_dns(attr->dns)) {
        strncpy(attr->dns, NONE_IP, IP_SIZE);
    }

    return LDAL_EOK;
}

int ldal_set_ip_attr(const char *ifname, const netdev_attr_t *attr)
{
    if (!set_local_ip(ifname, attr->ipaddr)) {
        strncpy(attr->ipaddr, NONE_IP, IP_SIZE);
    }

    if (!set_local_netmask(ifname, attr->netmask)) {
        strncpy(attr->netmask, NONE_IP, IP_SIZE);
    }

    if (!set_local_gateway(attr->gateway)) {
        strncpy(attr->gateway, NONE_IP, IP_SIZE);
    }

    if (!set_local_dns(attr->dns)) {
        strncpy(attr->dns, NONE_IP, IP_SIZE);
    }

    return LDAL_EOK;
}
