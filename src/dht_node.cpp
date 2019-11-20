#include"../include/dht_node.h"
Nid dht_node::get_nid()const{
    return nid;
}
bool dht_node::get_ring_flag()const{
    return ring_flag;
}
u_int16_t dht_node::get_current_port(){
    return port;
}
pair<Nid,pair<string,int>> dht_node::get_successors()const{
    return successors;
}
pair<Nid,pair<string,int>> dht_node::get_predecessors()const{
    return predecessors;
}
vector<pair<Kid,std::string>> dht_node::get_resources()const{
    return resoures;
}

dht_node::dht_node(){
    //节点初始化,需要初始化下
    string ip=util::get_ip();
    short port=util::choose_port();
    string key_str=ip+":"+to_string(port);//ip:port　格式的字符串
    nid=util::get_hash(key_str);//将字符串hash. 作为当前主机的node id
    short_cuts=vector< pair<Nid,pair<string,int> > >(MSIZE+1);
}
//
u_int32_t dht_node::get_successors_ip(){
    string ip_string=successors.second.first;
    char tmp_ip_ctr[ip_string.size()+1];
    for(int i=0;i<ip_string.size();i++){
        if(i==ip_string.size()) tmp_ip_ctr[i]='\0';
        else tmp_ip_ctr[i]=ip_string[i];//有待检查
    }
    return inet_addr(tmp_ip_ctr);
}

u_int16_t dht_node::get_successors_port(){
    return successors.second.second;
}

u_int32_t dht_node::get_predecessors_ip(){
    string ip_string=predecessors.second.first;
    char tmp_ip_ctr[ip_string.size()+1];
    for(int i=0;i<ip_string.size();i++){
        if(i==ip_string.size()) tmp_ip_ctr[i]='\0';
        else tmp_ip_ctr[i]=ip_string[i];//有待检查
    }
    return inet_addr(tmp_ip_ctr);
}

u_int16_t dht_node::get_predecessors_port(){
    return predecessors.second.second;
}

u_int32_t dht_node::get_current_ip(){
    char tmp_ip_ctr[ip.size()+1];
    for(int i=0;i<ip.size();i++){
        if(i==ip.size()) tmp_ip_ctr[i]='\0';
        else tmp_ip_ctr[i]=ip[i];//有待检查
    }
    return inet_addr(tmp_ip_ctr);
}

//join()函数
bool dht_node::join(const char* des_ip,short des_port){
//n加入一个Chord环，已知其中有一个节点n0.
    //向目标节点发自己的hash ip port 
    messager.set_socketfd(des_port,des_ip);
    struct sockaddr_in addr;
    
    int join_length=8+1+sizeof(addr.sin_addr.s_addr)+sizeof(addr.sin_port);
    uint8_t* join_send_buf=(u_int8_t*)malloc(join_length);
    *join_send_buf='j';
    *((u_int64_t*)(join_send_buf+1))=nid;
    *((uint32_t*)(join_send_buf+9))=get_current_ip();
    *((u_int16_t*)(join_send_buf+9+sizeof(addr.sin_addr.s_addr)))=port;
    messager.send((char*)join_send_buf,join_length);//发送请求加入的消息了

    uint8_t* recv_successor_buf=(u_int8_t*)malloc(join_length-1);
    messager.recv((char*)recv_successor_buf,join_length-1);
    //现在拿到了successors的东西了
    long successor_hash=*((uint64_t*)recv_successor_buf);//后继的hash值
    uint32_t successor_ip=*((uint32_t*)recv_successor_buf+8);//后继的ip
    uint16_t successor_port=*((uint16_t*)recv_successor_buf+12);//后继的port
    //下面完成对后继节点的赋值,要加锁
    extern pthread_mutex_t successor_info_mutex;
    pthread_mutex_lock(&successor_info_mutex);
    successors.first=successor_hash;
    successors.second.first=util::ip_convert_string(successor_ip);
    successors.second.second=successor_port;
    pthread_mutex_unlock(&successor_info_mutex);
    free(join_send_buf);
    free(recv_successor_buf);
    //赋值完通知后继节点
    //c hash ip port 
    notify(successor_ip,successor_port);
    
    //下面要设置shortcut吗? 我的想法是 
}

void dht_node::notify(u_int32_t successor_ip,u_int8_t successor_port){
    in_addr successor_addr;
    in_addr current_addr;
    successor_addr.s_addr=successor_ip;
    messager.set_socketfd(successor_port,inet_ntoa(successor_addr));
    //向后继通知
    struct  sockaddr_in addr;//提供给sizeof()用 而已
    u_int8_t* notify_buf=(u_int8_t*)malloc(9+sizeof(addr.sin_addr.s_addr)+
    sizeof(addr.sin_port));
    //设置发送的消息
    *notify_buf='c';//change 表示 
    *((u_int64_t*)(notify_buf+1))=nid;
    *((u_int32_t*)(notify_buf+1+8))=inet_addr(ip.c_str());
    *((u_int16_t*)(notify_buf+13))=port;
    messager.send((char*)notify_buf,9+sizeof(addr.sin_addr.s_addr)+
    sizeof(addr.sin_port));
    //接受自己后继节点的原前置节点的部分
    uint8_t* back_curr_preinfo=(uint8_t*)malloc(sizeof(Nid)+sizeof(uint32_t)+sizeof(uint16_t));
    messager.recv((char*)back_curr_preinfo,14);
    Nid pre_hash=*((Nid*)back_curr_preinfo);
    uint32_t pre_ip=*((uint32_t*)(back_curr_preinfo+8));
    uint16_t pre_port=*((uint16_t*)(back_curr_preinfo+12));
    struct in_addr pre_addr;
    pre_addr.s_addr=pre_ip;
    //改下自己当前前置节点的信息
    extern pthread_mutex_t precessor_mutex;
    pthread_mutex_lock(&precessor_mutex);
    predecessors.first=pre_hash;
    predecessors.second.first=inet_ntoa(pre_addr);
    predecessors.second.second=pre_port;
    pthread_mutex_unlock(&precessor_mutex);    
    messager.close_fd();
    messager.set_socketfd(pre_port,inet_ntoa(pre_addr));
    //向自己的前置节点通知一下
    *notify_buf='C';
    messager.send((char*)notify_buf,9+sizeof(addr.sin_addr.s_addr)+
    sizeof(addr.sin_port));
    messager.close_fd();
    //释放内存
    free(notify_buf);
    free(back_curr_preinfo);    
}
//leave 函数实现
bool dht_node::leave(){
    //离开时,向自己的pre和suc都发一下消息,
    uint8_t* pre_leave_info_buf=(uint8_t*)malloc(1+sizeof(Nid)+sizeof(uint32_t)+sizeof(uint16_t));
    uint8_t* suc_leave_info_buf=(uint8_t*)malloc(1+sizeof(Nid)+sizeof(uint32_t)+sizeof(uint16_t));
    *pre_leave_info_buf='l';
    *suc_leave_info_buf='L';
    *((Nid*)(pre_leave_info_buf+1))=predecessors.first;
    *((Nid*)(suc_leave_info_buf+1))=successors.first;
    *((uint32_t*)(pre_leave_info_buf+9))=inet_addr(predecessors.second.first.c_str());
    *((uint32_t*)(suc_leave_info_buf+9))=inet_addr(successors.second.first.c_str());
    *((uint16_t*)(pre_leave_info_buf+13))=predecessors.second.second;
    *((uint16_t*)(pre_leave_info_buf+13))=predecessors.second.second;
    messager.set_socketfd(successors.second.second,successors.second.first.c_str());
    messager.send((char*)pre_leave_info_buf,1+sizeof(Nid)+sizeof(uint32_t)+sizeof(uint16_t));
    messager.close_fd();
    messager.set_socketfd(predecessors.second.second,predecessors.second.first.c_str());
    messager.send((char*)suc_leave_info_buf,1+sizeof(Nid)+sizeof(uint32_t)+sizeof(uint16_t));
    messager.close_fd();
}

void dht_node::set_predecessors(uint32_t pre_ip,uint16_t pre_port,Nid pre_hash){
    predecessors.first=pre_hash;
    struct in_addr addr;
    addr.s_addr=pre_ip;
    string ip_str=inet_ntoa(addr);
    predecessors.second.first=ip_str;
    predecessors.second.second=pre_port;
}

void dht_node::set_successors(uint32_t suc_ip,uint16_t suc_port,Nid suc_hash){
    successors.first=suc_hash;
    struct in_addr addr;
    addr.s_addr=suc_ip;
    successors.second.first=inet_ntoa(addr);
    successors.second.second=suc_port;
}

pair<Kid,pair<string,int>> dht_node::choose_fit_query(Kid key)const{
     //选择最接近key的节点.能进入这个函数说明自己本身已经没有这个资源了.    
}

string dht_node::get(Kid key)const{
    extern pthread_mutex_t resource_mutex;
    pthread_mutex_lock(&resource_mutex);
    for(int i=0;i<resoures.size();i++){
        if(resoures[i].first==key){
            //本机上就有的情况
            pthread_mutex_unlock(&resource_mutex);
            return resoures[i].second;
        }
    }
    pthread_mutex_unlock(&resource_mutex);
    //本机没有,向后找.
    pair<Kid,pair<string,int>> fit_node=choose_fit_query(key);
    message_controller queryer;
    queryer.set_socketfd(fit_node.second.second,fit_node.second.first.c_str());
    //向最适合的节点发消息,然后等待,消息格式为g Kid
    uint8_t* send_buf=(uint8_t*)malloc(sizeof(char)+sizeof(Kid));
    *send_buf='g';
    *((Kid*)(send_buf+1))=key;
    queryer.send((char*)send_buf,sizeof(char)+sizeof(Kid));//向别的节点查询
    int value_length;
    queryer.recv((char*)(&value_length),sizeof(int));
    char* value=(char*)malloc(sizeof(char)*value_length);
    queryer.recv(value,value_length);
    queryer.close_fd();
    return value;
}
void dht_node::put(Kid key,string value){
    extern pthread_mutex_t successor_info_mutex;
    pthread_mutex_lock(&successor_info_mutex);
    //要考虑后继的hash是比自己大还是小
    if(successors.first>nid){
        if(key>=nid&&key<successors.first){
            extern pthread_mutex_t resource_mutex;
            pthread_mutex_lock(&resource_mutex);
            pair<Kid,std::string> new_resource;
            new_resource.first=key;
            new_resource.second=value;
            resoures.push_back(new_resource);
            pthread_mutex_unlock(&resource_mutex);
            pthread_mutex_unlock(&successor_info_mutex);
        }
        else{
            //把东西转发给后继
            message_controller putvaluer;
            putvaluer.set_socketfd(successors.second.second,successors.second.first.c_str());
            //消息格式:p Kid int value
            pthread_mutex_unlock(&successor_info_mutex);
            uint8_t* send_buf=(uint8_t*)malloc(sizeof(char)+sizeof(int)
            +sizeof(Kid)+value.size()*sizeof(char));
            *send_buf='p';
            *((Kid*)(send_buf+1))=key;
            *((int*)(send_buf+1+sizeof(Kid)))=value.size();
            const char* value_str=value.c_str();
            for(int i=0;i<value.size();i++){
                *((char*)(send_buf+1+sizeof(Kid)+sizeof(int)+i))=value_str[i];
            }
            //把消息放到缓冲区后发送
            putvaluer.send((char*)send_buf,sizeof(char)+sizeof(int)
            +sizeof(Kid)+value.size()*sizeof(char));
            putvaluer.close_fd();

        }
    }
    else {
        //当后继比当前的hash还小时.
        if(key>=successors.first&&key<nid){
            message_controller putvaluer;
            putvaluer.set_socketfd(successors.second.second,successors.second.first.c_str());
            //消息格式:p Kid int value
            pthread_mutex_unlock(&successor_info_mutex);
            uint8_t* send_buf=(uint8_t*)malloc(sizeof(char)+sizeof(int)
            +sizeof(Kid)+value.size()*sizeof(char));
            *send_buf='p';
            *((Kid*)(send_buf+1))=key;
            *((int*)(send_buf+1+sizeof(Kid)))=value.size();
            const char* value_str=value.c_str();
            for(int i=0;i<value.size();i++){
                *((char*)(send_buf+1+sizeof(Kid)+sizeof(int)+i))=value_str[i];
            }
            //把消息放到缓冲区后发送
            putvaluer.send((char*)send_buf,sizeof(char)+sizeof(int)
            +sizeof(Kid)+value.size()*sizeof(char));
            putvaluer.close_fd();
        }
        else{
            extern pthread_mutex_t resource_mutex;
            pthread_mutex_lock(&resource_mutex);
            pair<Kid,std::string> new_resource;
            new_resource.first=key;
            new_resource.second=value;
            resoures.push_back(new_resource);
            pthread_mutex_unlock(&resource_mutex);
            pthread_mutex_unlock(&successor_info_mutex);
        }

    }
}

void dht_node::push_resource(pair<Kid,std::string> new_resource){
    resoures.push_back(new_resource);
    return;
}