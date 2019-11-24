#include"../include/libheaders.h"
#include"../include/dht_node.h"
#include<iostream>

pthread_mutex_t precessor_mutex;
pthread_mutex_t successor_info_mutex;
pthread_mutex_t resource_mutex;

void show_help(){
    cout<<"1) create : 创建\n\n";
    cout<<"2) join <ip> <port> : 将通过指定的节点加入一个集群中\n\n";
    cout<<"3) put <key> <value> : will put key and value to the node it belongs to\n\n";
    cout<<"4) get <key> : will get value of mentioned key\n\n";
    cout<<"5) info : 获取当前的ip与port\n\n";
    cout<<"5) ls : 列出当前主机的前置节点与后继节点\n\n";
}
int main(){
    pthread_mutex_init(&resource_mutex,NULL);
    pthread_mutex_init(&successor_info_mutex,NULL);
    pthread_mutex_init(&precessor_mutex,NULL);
    dht_node* current_dht=new dht_node();
    while(1){
        show_help();
        string input;
        cout<<"->  ";
        getline(cin,input);
        vector<string> splited_strs;
        if(input=="create"){
            util::create_dht_ring(current_dht);
        }
        else if(input=="info"){
            cout<<"\n->  当前主机的网络地址为ip:"<<current_dht->ip
            <<" 端口: "<<current_dht->port
            <<"哈希:"<<current_dht->get_nid()<<endl;
        }
        else if(input=="ls"){
            cout<<"\n->  当前主机的前置节点网络地址为ip:"<<current_dht->get_predecessors().second.first
            <<" 端口: "<<current_dht->get_predecessors().second.second<<endl;
            cout<<"\n->  当前主机的后继节点网络地址为ip:"<<current_dht->get_successors().second.first
            <<" 端口: "<<current_dht->get_successors().second.second<<endl;
        }
        else{
            util::split_string(input,splited_strs," ");
            if(splited_strs[0]=="join"){
                const char* ip_c_str=splited_strs[1].c_str();
                const char* port_c_str=splited_strs[2].c_str();
                short port=atoi(port_c_str);
                current_dht->join(ip_c_str,port);
            }
            else if(splited_strs[0]=="put"){
                Kid key=util::get_hash(splited_strs[1]);
                current_dht->put(key,splited_strs[2]);
            }
            else if(splited_strs[0]=="get"){
                //把key转成hash Kid的值
                string value=current_dht->get(util::get_hash(splited_strs[1]));
                cout<<endl;
                cout<<"->  "<<value;
            }
            else{
                cout<<"输入错误请重试";
            }
        }
        cout<<endl;
    }

}