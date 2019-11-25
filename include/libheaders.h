#ifndef _LIB_HEADERS_H
#define _LIB_HEADERS_H
#include<utility>
#include<vector>
#include<map>
#include<string>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<ifaddrs.h>
#include<netdb.h>
struct  hash_query
{
    bool is_contain;
    std::string value;
    //这个结构体是查询用的,如果本机有数据,则将is_contain 设置为true 再把string值设置为要发的数据
    //
};
#endif