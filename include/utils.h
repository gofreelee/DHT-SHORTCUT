#ifndef _UTIL_H
#define _UTIL_H
#include "libheaders.h"
#include "dht_node.h"
class util
{
    typedef long Nid; //表示节点ID.
public:
    //SHA1算法.获取对应的哈希值.
    static Nid get_hash(std::string key);

    //获取本机的ip
    static std::string get_ip();

    //选择一个可用的端口号．
    static short choose_port();

    //是否当前的port正在被使用．
    static bool port_in_use(short port);

    //创建节点.
    static bool create_dht_ring(dht_node *node);

    //把ip转成 string
    static string ip_convert_string(u_int32_t ip);

    static ssize_t absolute_recv(int fd, void *buf, size_t n);

    static ssize_t absolute_send(int fd, void *buf, size_t n);

    static void split_string(const string &s, vector<string> &v, const string &c);
};
#endif
class util;