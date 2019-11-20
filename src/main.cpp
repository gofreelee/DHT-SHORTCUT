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
        cin>>input;
        vector<string> splited_strs;
        if(input=="create"){
            util::create_dht_ring(current_dht);
        }
        else{
            util::split_string(input,splited_strs," ");
            if(splited_strs[0]=="join"){
                current_dht->join(splited_strs[1].c_str(),atoi(splited_strs[2].c_str()));
            }
            else if(splited_strs[0]=="put"){
                current_dht->put(atol(splited_strs[1].c_str()),splited_strs[2]);
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