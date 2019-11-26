#include "../include/hash_storage_thread.h"

hash_storage_thread::hash_storage_thread(int _client_fd, dht_node *_current_node)
{
    client_fd = _client_fd;
    current_node = _current_node;
}
hash_query hash_storage_thread::query_curr_table(Kid key)
{
    struct hash_query query_result;
    vector<pair<Kid, std::string>> curr_resource = current_node->get_resources();
    //cout<<"进入查询当前资源";
    for (int i = 0; i < curr_resource.size(); i++)
    {
        if (curr_resource[i].first == key)
        {
            query_result.is_contain = true;
            query_result.value = curr_resource[i].second;
            //cout<<"已查询到"<<endl;
            return query_result;
        }
    }
    query_result.is_contain = false;
    return query_result;
}

bool hash_storage_thread::run()
{
    if (type == GET)
    {
        deal_get();
    }
    else if (type == PUT)
    {
        deal_put();
    }
}
void hash_storage_thread::deal_get()
{
    Kid key;
    util::absolute_recv(client_fd, &key, sizeof(Kid));
    extern pthread_mutex_t resource_mutex;
    pthread_mutex_lock(&resource_mutex);
    struct hash_query query_result = query_curr_table(key);
    if (query_result.is_contain)
    {
        //包含数据,发回去 回去的格式  int string
        int length = query_result.value.size();
        cout<<"我发给你的数据:"<<query_result.value<<"长度"<<length;
        util::absolute_send(client_fd, &length, sizeof(int));
        util::absolute_send(client_fd, (void *)(query_result.value.c_str()), length);
    }
    else
    {
        //不包含数据,选一个合适的shortcut 向它问
        pair<Kid, pair<string, int>> fit_node = current_node->choose_fit_query(key);
        //发数据
        uint8_t *send_buf = (uint8_t *)malloc(sizeof(char) + sizeof(Kid));
        *send_buf = 'g'; //get类型
        *((Kid *)(send_buf + 1)) = key;
        message_controller messager_forward_get(fit_node.second.second,
                                                inet_addr(fit_node.second.first.c_str()));
        //向最适合节点发请求寻找.
        messager_forward_get.send((char *)send_buf, sizeof(char) + sizeof(Kid));
        //
        int length;
        messager_forward_get.recv((char *)&length, sizeof(int));
        char *value = (char *)malloc(sizeof(char) * length);
        messager_forward_get.recv(value, length);
        //获取到value了,发给client_fd
        util::absolute_send(client_fd, &length, sizeof(int));
        util::absolute_send(client_fd, value, length);
    }
    pthread_mutex_unlock(&resource_mutex);
}

void hash_storage_thread::deal_put()
{
    //消息格式:p Kid int value
    Kid key;
    int length;
    util::absolute_recv(client_fd, &key, sizeof(Kid));    //key值
    util::absolute_recv(client_fd, &length, sizeof(int)); //value的长度
    char *value = (char *)malloc(sizeof(char) * length);
    util::absolute_recv(client_fd, value, length);
    //判断一下这个hash和当前节点的关系
    extern pthread_mutex_t successor_info_mutex;
    pthread_mutex_lock(&successor_info_mutex);
    if (current_node->get_successors().first > current_node->get_nid())
    {
        if (key >= current_node->get_nid() && key < current_node->get_successors().first)
        {
            extern pthread_mutex_t resource_mutex;
            pthread_mutex_lock(&resource_mutex);
            pair<Kid, std::string> new_resource;
            new_resource.first = key;
            new_resource.second = value;
            current_node->push_resource(new_resource);
            pthread_mutex_unlock(&resource_mutex);
            pthread_mutex_unlock(&successor_info_mutex);
        }
        else
        {
            message_controller putvaluer;
            putvaluer.set_socketfd(current_node->get_successors().second.second,
                                   current_node->get_successors().second.first.c_str());
            //消息格式:p Kid int value
            pthread_mutex_unlock(&successor_info_mutex);
            uint8_t *send_buf = (uint8_t *)malloc(sizeof(char) + sizeof(int) + sizeof(Kid) + length * sizeof(char));
            *send_buf = 'p';
            *((Kid *)(send_buf + 1)) = key;
            *((int *)(send_buf + 1 + sizeof(Kid))) = length;
            for (int i = 0; i < length; i++)
            {
                *((int *)(send_buf + 1 + sizeof(Kid) + sizeof(int) + i)) = value[i];
            }
            //把消息放到缓冲区后发送
            putvaluer.send((char *)send_buf, sizeof(char) + sizeof(int) + sizeof(Kid) + length * sizeof(char));
            putvaluer.close_fd();
            free(send_buf);
        }
    }
    else
    {
        if (key < current_node->get_nid() && key >= current_node->get_successors().first)
        {
            message_controller putvaluer;
            putvaluer.set_socketfd(current_node->get_successors().second.second,
                                   current_node->get_successors().second.first.c_str());
            //消息格式:p Kid int value
            pthread_mutex_unlock(&successor_info_mutex);
            uint8_t *send_buf = (uint8_t *)malloc(sizeof(char) + sizeof(int) + sizeof(Kid) + length * sizeof(char));
            *send_buf = 'p';
            *((Kid *)(send_buf + 1)) = key;
            *((int *)(send_buf + 1 + sizeof(Kid))) = length;
            for (int i = 0; i < length; i++)
            {
                *((int *)(send_buf + 1 + sizeof(Kid) + sizeof(int) + i)) = value[i];
            }
            //把消息放到缓冲区后发送
            putvaluer.send((char *)send_buf, sizeof(char) + sizeof(int) + sizeof(Kid) + length * sizeof(char));
            putvaluer.close_fd();
            free(send_buf);
        }
        else
        {
            extern pthread_mutex_t resource_mutex;
            pthread_mutex_lock(&resource_mutex);
            pair<Kid, std::string> new_resource;
            new_resource.first = key;
            new_resource.second = value;
            current_node->push_resource(new_resource);
            pthread_mutex_unlock(&resource_mutex);
            pthread_mutex_unlock(&successor_info_mutex);
        }
    }
    free(value);
}