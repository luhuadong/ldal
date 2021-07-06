#!/usr/bin/python3

"""
 UDP server
"""

import socket
import time
import sys

def main():
    if len(sys.argv) < 2:
        port = 8080
    else:
        port = int(sys.argv[1])
    
    # 1. 创建 udp 套接字
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # 2. 绑定本地信息
    udp_socket.bind(('', port))

    print("wait...")

    # 3. 接收发送的数据
    cnt = 100;
    while cnt > 0:
        recv_data = udp_socket.recvfrom(1024)
        #print(recv_data.decode('gbk'))
        #print(recv_data.decode('utf-8'))
        print(recv_data)
        #data = recv_data.decode('utf-8')
        #print('0x%x'%data)
        cnt = cnt - 1

    time.sleep(10)

    # 7. 关闭套接字
    udp_socket.close()
    print("close")


if __name__ == '__main__':
    main()

