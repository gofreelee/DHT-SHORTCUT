#include"../include/libheaders.h"
#include"../include/message_controller.h"
#include<iostream>
#include<string.h>

message_controller:: message_controller(short _port,const char* _ip){
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd<0){
        std::cout<<"创建套接字失败"<<std::endl;
        return;
    }
    addr.sin_family=AF_INET;
    addr.sin_port=htons(_port);
    addr.sin_addr.s_addr=inet_addr(_ip);
    flag=true;
}

message_controller:: message_controller(u_int16_t _port,u_int32_t _ip){
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd<0){
        std::cout<<"创建套接字失败"<<std::endl;
        return;
    }
    addr.sin_family=AF_INET;
    addr.sin_port=htons(_port);
    addr.sin_addr.s_addr=_ip;
    flag=true;
}

size_t message_controller::send(char* buf,size_t message_size){
    if(flag){
    if(connect(socket_fd,(struct sockaddr*)&addr,sizeof(addr))==-1){
        std::cout<<"目标主机已离开"<<std::endl;
        return -1;
        }
        flag=false;
    }
    int index=0;
    int temp_counter=0;
    while(index<message_size){
        temp_counter=::send(socket_fd,buf,message_size,0);
        buf+=temp_counter;
        message_size-=temp_counter;
        index+=temp_counter;
    }
    return temp_counter;
}

size_t message_controller::recv(char* buf,size_t message_size){
    char* tmp_buf=(char*)buf;
    int index=0;
    int counter=0;
    int ret;
    while(index!=message_size){
        ret=::recv(socket_fd,buf,message_size,0);
        if(ret==-1){
            if(errno==EINTR){
                continue;
            }
            else {
                std::cout<<strerror(errno);
                std::cout<<"error: 接受数据错误"<<std::endl;
                return -1;
            }
        }
        else if(ret==0){
            std::cout<<"warning:对端关闭了连接"<<std::endl;
            ::close(socket_fd);
            return counter;
        }
        else {
            counter+=ret;
            tmp_buf=tmp_buf+ret;
            index+=ret;
        }
    }
    return message_size;
}

//设置套接字
void message_controller::set_socketfd(short _port,const char* _ip){
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd<0){
        std::cout<<"创建套接字失败"<<std::endl;
        return;
    }
    addr.sin_family=AF_INET;
    addr.sin_port=htons(_port);
    addr.sin_addr.s_addr=inet_addr(_ip);
    flag=true;
}

void message_controller::close_fd(){
    
    ::close(socket_fd);
    flag=true;
}
message_controller::~message_controller(){
    if(socket_fd>0){
        close_fd();
    }
}