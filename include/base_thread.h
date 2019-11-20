#ifndef _BASE_THREAD_H
#define _BASE_THREAD_H
#include"libheaders.h"
#include<pthread.h>
//这只是一个基类
class base_thread{
public:
    //构造函数
    base_thread();
    virtual ~base_thread();//析构函数
    virtual bool create_thread();
    virtual bool init_instance();
    virtual bool exit_instance();
    virtual bool run()=0;
    
    void s_sleep();
    void ms_sleep();
    bool join();
    static void* thread_func(void*);
private:
    pthread_t thread_id;    
};
#endif
class base_thread;