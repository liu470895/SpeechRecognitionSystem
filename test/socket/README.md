# Socket

建立一个简单的socket通信链路，实现客户端与服务器之间收发数据。因为这只是个功能测试程序，所以不考虑代码结构以及接口设计方面，从逻辑方面来说，就是主函数一条线走到底实现想要的功能。

## 测试功能

- [x] socket连接
- [x] 客户端发送文件到服务器，接收服务器发送来的文件
- [x] 服务器发送文件到客户端，接收客户端发送来的文件

## 测试环境搭建

* 搭建一个典型的基于socket的tcp客户端服务器连接程序[^1][^2][^3][^4]

* 自定义一个超级简单的应用层通信协议

  |    起始位(7bytes)    |    长度(4bytes)    | 数据块(length) |
  | :---------------: | :--------------: | :---------: |
  | (char *)start\r\n | (uint32_t)length |   data...   |

## 参考链接

1. man 7 ip
2. man htonl
3. man inet
4. man socket