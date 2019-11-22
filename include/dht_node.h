#ifndef _DHT_NODE_H
#define _DHT_NODE_H
#include"libheaders.h"
#include"storage_value.h"
#include"message_controller.h"
#include"utils.h"
#include"listen_thread.h"
#include<iostream>
using namespace std;
#define MSIZE 8
typedef long Nid;//表示节点ID.
typedef long Kid;//数据哈希后的ID

class dht_node{
private:
    Nid nid;
    pair<Nid,pair<string,int>> predecessors;//前置节点
    pair<Nid,pair<string,int>> successors;//pair<string,int>用来标识对应的主机 ip　和 port
    vector<pair<Kid,std::string>> resoures;//既然是分布式哈希表,这个成员实际上就是空的一个部分的hash表
    vector<pair<Nid,pair<string,int> >> short_cuts;//
    bool ring_flag;//true为已创建环，false为未创建环

public:
    message_controller messager;//这个对象用于进行网络交互,发信息
    string ip;//当前节点的ip地址
    short port;//端口
    dht_node();
    bool join(const char* ip,short port);//n加入一个Chord环，已知其中有一个节点n0.
    bool leave();//当节点离开网络的时候需要进行的操作.
    bool stabilize();//当前节点向后继节点查询,如果后继节点的前置节点不是自己,那么将自己的后继节点改成
    void notify(const dht_node& des_node);//向目标节点通知自己的存在.
    void notify(u_int32_t des_ip,u_int16_t des_port);
    void init_short_cuts();
    pair<Kid,pair<string,int>> choose_fit_query(Kid key)const;//
    int calculate_is_between(Kid key,Nid node_hash);//key的hash与node_hash之间的距离是多少
    string get(Kid key)const;
    void put(Kid key,string value);
    void push_resource(pair<Kid,std::string> new_resource);
    //下面是一系列的get set 方法
    Nid get_nid()const;
    pair<Nid,pair<string,int>> get_successors()const;
    pair<Nid,pair<string,int>> get_predecessors()const;
    vector<pair<Kid,std::string>> get_resources()const;
   // map<Kid,storaged_value> get_resources();
    vector<pair<Nid,pair<string,int>>> get_short_cuts()const;

    u_int32_t get_successors_ip();
    u_int32_t get_predecessors_ip();
    u_int32_t get_current_ip();
    u_int16_t get_current_port();
    u_int16_t get_successors_port();
    u_int16_t get_predecessors_port();
    bool get_ring_flag()const;
    void set_nid(const dht_node& res_node);
    void set_successors(const pair<Nid,pair<string,int>>& res_successors);
    void set_successors(uint32_t suc_ip,uint16_t suc_port,Nid suc_hash);
    void set_predecessors(const pair<Nid,pair<string,int>>& res_predecessors);
    void set_predecessors(uint32_t pre_ip,uint16_t pre_port,Nid pre_hash);
    void set_resources(const map<Kid,storaged_value>& res_resources);
    void set_short_cuts(const vector<pair<Nid,pair<string,int>>>&  res_short_cuts);
    void set_ring_flag(bool flag);   
};
#endif
class dht_node;
//整个dht的工作原理:最开始一个节点创建ring.
//当有新的要加入时?计算下自己在第一个创建的节点的位置