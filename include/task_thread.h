#ifndef _TASK_THREAD_H
#define _TASK_THREAD_H
#include "base_thread.h"
#include "libheaders.h"
#include "dht_node.h"

class task_thread : public base_thread
{
    //这是用于应答其他节点请求的工作线程
protected:
    dht_node *current_node; //工作线程也需要持有
    int client_fd;
    bool CLOSE_FD;

public:
    task_thread(int _client_fd, dht_node *_current_node);
    task_thread(){};
    ~task_thread();
    virtual bool init_instance();
    virtual bool exit_instance();
    virtual bool run();
};
#endif
class task_thread;