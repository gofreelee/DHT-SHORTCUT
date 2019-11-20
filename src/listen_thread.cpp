#include"../include/listen_thread.h"
#include"../include/dht_ring_thread.h"
#include<iostream>

//构造函数，给监听线程构造时设置它持有的dht_node,
//因为在后面会有对这个dht_node的改变,因此需要以指针的形式传递.
listen_thread::listen_thread(dht_node* _current_node,int listen_fd){
    current_node=_current_node;
    listen_socket_fd=listen_fd;
}

//监听线程初始要创建一个监听套接字,并绑定该套接字到current_node的ip:port
bool listen_thread::init_instance(){
    //初始化工作
    listen_socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_socket_fd<0){
        std::cout<<"error: 监听套接字创建失败!"<<std::endl;
        return false;
    }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=current_node->port;
    //inet_addr函数需要的 const char*
    //因此需要把string转到char* 里

    char *current_node_ip=(char*)malloc(sizeof(char)*(current_node->ip.size()+1));
    for(int i=0; i<=current_node->ip.size(); i++){
        if(i==current_node->ip.size()) current_node_ip[i]='\0';
        else current_node_ip[i]=current_node->ip[i];
    }
    addr.sin_addr.s_addr=inet_addr(current_node_ip);
    free(current_node_ip);

    //绑定端口号.
    if(bind(listen_socket_fd,(struct sockaddr*)&addr,sizeof(addr))==-1){
        std::cout<<"error: 绑定端口失败"<<std::endl;
        return false;
    }
    return true;
}

bool listen_thread::exit_instance(){
    close(listen_socket_fd);
}

listen_thread::~listen_thread(){
    //千万别动current_node* 
    exit_instance();
}

bool  listen_thread::run(){
    //1.0的想法就是一个fd一个线程吧,先实现,然后改成一个任务线程管理多个fd.
    while(true){
        int client_fd=accept(listen_socket_fd,nullptr,nullptr);
        //把这个clientfd扔到一个任务线程里.
        dht_ring_thread dht_ring_task(client_fd,current_node);
        dht_ring_task.create_thread();        
    }
}
