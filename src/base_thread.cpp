#include"../include/base_thread.h"

base_thread::base_thread(){
    thread_id=0;
}
void* base_thread::thread_func(void* parm){
    base_thread* run_thread=(base_thread*)parm;
    if(run_thread->init_instance()){
        run_thread->run();
    }
    run_thread->exit_instance();
    return nullptr;
}

bool base_thread::join(){
    if(thread_id!=0){
        return pthread_join(thread_id,nullptr)==0;
    }
    return true;
}
bool base_thread::create_thread(){
    if(thread_id==0){
        return pthread_create(&thread_id,nullptr,&thread_func,this)==0;
    }
    return true;
}