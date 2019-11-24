#ifndef _DHT_RING_THREAD_H
#define _DHT_RING_THREAD_H
#include"task_thread.h"
#include"hash_storage_thread.h"
//这个线程是执行的控制
//需要重写run函数,以及若干方法用来处理
class dht_ring_thread: public task_thread{
public:
    static const char JOIN = 'j';//join 类型
    static const char FORWARD = 'f';//forward 类型
    static const char CHANGE_PRE = 'c';//修改前继
    static const char CHANGR_SUC = 'C';//修改后继
    static const char PRE_LEAVE = 'l';//当前的前置节点离开了
    static const char SUC_LEAVE = 'L';//当前的后继节点离开了
    static const char PUT='p';
    static const char GET='g';
    bool CLOSE_FD;
    virtual bool run();
    dht_ring_thread(){}
    virtual ~dht_ring_thread();
    dht_ring_thread(int _client_fd,dht_node* _current_node);
    //完全收取,确保能够读完__n字节的数据
    ssize_t absolute_recv(int fd, void *buf, size_t n);
    ssize_t absolute_send(int client_fd,void* sendbuf,size_t sendbuf_length);
};
#endif
class dht_ring_thread;