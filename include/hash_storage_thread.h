#ifndef _HASH_STORAGE_THREAD_H
#define _HASH_STORAGE_THREAD_H
#include "task_thread.h"
#include "libheaders.h"

//这个线程是负责查找value的
//当一台主机查找一个key时,找不到,会选一个节点,问问他有没有
class hash_storage_thread: public task_thread{
public:
    // dht_node* current_node;//工作线程也需要持有
    // int client_fd;
    static const char GET='g';//GET操作
    static const char PUT='p';//PUT操作
    char type;
    virtual bool run();
    hash_storage_thread(int _client_fd,dht_node* _current_node);
    ssize_t absolute_recv(int fd, void *buf, size_t n);
    ssize_t absolute_send(int fd, void *buf, size_t n);
    hash_query query_curr_table(Kid key);
    void deal_get();
    void deal_put();
};
#endif
class hash_storage_thread;