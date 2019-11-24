#ifndef _MESSAGE_CONTROLLER_H
#define _MESSAGE_CONTRILLER_H
#include "libheaders.h"
using namespace std;
//可能用到的网络操作.
class message_controller
{
private:
    int socket_fd;
    short _port;
    string _ip;
    int des_socket_fd;
    struct sockaddr_in addr;
    bool flag; //标志套接字创建后是否还为连接过
public:
    message_controller(short _port, const char *_ip);
    message_controller(u_int16_t port, u_int32_t ip);
    message_controller() {}
    ~message_controller();
    size_t send(char *buf, size_t message_size);
    size_t recv(char *buf, size_t message_size);
    void set_socketfd(short _port, const char *_ip); //调整当前socket
    void close_fd();
};
#endif