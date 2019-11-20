#include"../include/utils.h"
#include"../include/listen_thread.h"
#include<iostream>
//SHA1算法.获取对应的哈希值.
Nid util::get_hash(std::string key){

}

    //获取本机的ip
std::string util::get_ip(){

}

    //选择一个可用的端口号．
short util::choose_port(){

}

    //是否当前的port正在被使用．
bool util::port_in_use(short port){
    
}

bool util::create_dht_ring( dht_node* node){
    bool cur_status=node->get_ring_flag();

    if(cur_status){
        std::cout<<"warning: 当前节点已加入一个分布式哈希表环! "<<std::endl;
        return false;
    }
    else{
        std::pair<Nid,pair<std::string,short>> tmp_precessor;
        std::pair<Nid,pair<std::string,short>> tmp_successor;
        //初始的时候,前置后继都是自己
        tmp_precessor.first=node->get_nid();
        tmp_precessor.second.first=node->ip;
        tmp_precessor.second.second=node->get_current_port();
        tmp_successor=tmp_precessor;
        //创建完成后需要开启一个监听线程
        //先创建本地监听套接字
        int listen_fd=socket(AF_INET,SOCK_STREAM,0);
        if(listen_fd<0){
            cout<<"创建监听线程失败"<<endl;
            return false;
        }
        struct sockaddr_in curr_addr;
        curr_addr.sin_port=node->get_current_port();
        curr_addr.sin_family=AF_INET;
        curr_addr.sin_addr.s_addr=node->get_current_ip();
        if(bind(listen_fd,(struct sockaddr*)&curr_addr,sizeof(curr_addr))==-1){
            cout<<"创建监听线程失败"<<endl;
            return false;
        }
        listen_thread listen_task(node,listen_fd);
        listen_task.create_thread();
    }
}

string util::ip_convert_string(u_int32_t ip){
    char* ctr_ip;
    in_addr temp_addr;
    temp_addr.s_addr=ip;
    ctr_ip=inet_ntoa(temp_addr);
    string ip_str=ctr_ip; //存疑,极可能有bug
    return ip_str;
}

ssize_t util::absolute_send(int fd,void* buf,size_t n){
    int index=0;
    int length=n;
    int ret;
    u_int8_t* tmp_buf=(u_int8_t*)buf;
    while (length>0){
        ret=::send(fd,tmp_buf,n,0);
        if(ret==-1){
            if(errno==EINTR){
                continue;
            }
            else{
                std::cout<<"error: 发送数据错误"<<std::endl;
                return -1;
            }
        }
        else if(ret==0){
            std::cout<<"waring: 对端关闭了连接"<<std::endl;//?
            break;
        }
        length-=ret;
        tmp_buf+=ret;
    }
    return (n-length);    
}

//确保从读取n字节的数据.
ssize_t util::absolute_recv(int fd, void *buf, size_t n){
    char* tmp_buf=(char*)buf;
    int index=0;
    int counter=0;
    int ret;
    while(index!=n){
        ret=::recv(fd,buf,n,0);
        if(ret==-1){
            if(errno==EINTR){
                continue;
            }
            else {
                std::cout<<"error: 接受数据错误"<<std::endl;
                return -1;
            }
        }
        else if(ret==0){
            std::cout<<"warning:对端关闭了连接"<<std::endl;
            ::close(fd);
            return counter;
        }
        else {
            counter+=ret;
            tmp_buf=tmp_buf+ret;
            index+=ret;
        }
    }
    return n;
}

void split_string(const string& s, vector<string>& v, const string& c)
 {
      string::size_type pos1, pos2;
      pos2 = s.find(c);
     pos1 = 0;
     while(string::npos != pos2)
     {
         v.push_back(s.substr(pos1, pos2-pos1));
          
         pos1 = pos2 + c.size();
         pos2 = s.find(c, pos1);
     }
     if(pos1 != s.length())
         v.push_back(s.substr(pos1));
 }