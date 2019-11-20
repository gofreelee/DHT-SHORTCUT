#include"libheaders.h"
#include"dht_node.h"
#include"base_thread.h"

class listen_thread:public base_thread{
private:
    dht_node* current_node;
    int listen_socket_fd;
public:
     listen_thread(dht_node* _current_node,int listen_fd);     
    ~listen_thread();
    virtual bool init_instance();
    virtual bool exit_instance();
    virtual bool run();
};