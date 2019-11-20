#include"../include/task_thread.h"
#include<iostream>

task_thread::task_thread(int _client_fd,dht_node* _current_node){
        client_fd=_client_fd;
        current_node=_current_node;
}

bool task_thread::init_instance(){
        std::cout<<"开始任务线程处理 "<<std::endl;
        return true;
}

bool task_thread::exit_instance(){
        close(client_fd);
         return true;
}

task_thread::~task_thread(){
        exit_instance();
}

bool task_thread::run(){
    
}
