#!/usr/bin/python3

"""
 TCP server
"""

import socket
import time
import sys

def main():
    if len(sys.argv) < 2:
        port = 9955
    else:
        port = int(sys.argv[1])
    
    # 1. 创建 tcp 套接字
    tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 2. 绑定本地信息
    tcp_server.bind(('', port))

    # 3. 让默认的套接字由主动变为被动 listen
    tcp_server.listen(128)
    print("wait...")

    # 4. 等待客户端的连接
    client_socket, client_addr = tcp_server.accept()
    print("accept one")

    # 5. 接收发送的数据
    recv_data = client_socket.recv(1024)
    #print(recv_data.decode('gbk'))
    #print(recv_data.decode('utf-8'))
    print(recv_data)
    #data = recv_data.decode('utf-8')
    #print('0x%x'%data)

    time.sleep(10)

    # 6. 回送一部分数据给客户端
    #client_socket.send('收到了'.encode('gbk'))
    client_socket.send(recv_data)

    time.sleep(10)

    # 7. 关闭套接字
    client_socket.close()
    tcp_server.close()
    print("close")


if __name__ == '__main__':
    main()

