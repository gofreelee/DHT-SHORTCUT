#include "../include/dht_ring_thread.h"
#include <iostream>
#include <string.h>
dht_ring_thread::dht_ring_thread(int _client_fd, dht_node *_current_node)
{
    client_fd = _client_fd;
    current_node = _current_node;
    CLOSE_FD = true;
}
bool dht_ring_thread::run()
{
    //从client_fd中读取一个字节
    char type;
    struct sockaddr_in new_node_addr; //变量声明位置有待考虑.
    if (absolute_recv(client_fd, &type, 1) == 1)
    {
        if (type == JOIN)
        {
            //cout<<"JOIN";
            //当第一个字符是J时,表示有个节点想要通过当前这个节点加入到整个环中
            long node_hash;
            if (absolute_recv(client_fd, &node_hash, 8) == 8)
            {
                //拿到新节点的hash值,下面需要拿ip 和 port
                absolute_recv(client_fd, &new_node_addr.sin_addr.s_addr, 4);
                absolute_recv(client_fd, &new_node_addr.sin_port, 2);
                //下面是新节点加入的业务操作了
                if (current_node->get_successors().first > current_node->get_nid())
                {
                    cout << 1;
                    if (current_node->get_nid() < node_hash &&
                        node_hash < current_node->get_successors().first)
                    {
                        //这个时候说明找到了,找到新node在这个环里面的直接后继了,把这个后继的信息,发给新node
                        int sendbuf_length = 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                        u_int8_t *sendbuf = (u_int8_t *)malloc(sendbuf_length);
                        //下面设置要发送的数据
                        *((long *)sendbuf) = current_node->get_successors().first;
                        *((uint32_t *)(sendbuf + 8)) = current_node->get_successors_ip();
                        *((u_int16_t *)(sendbuf + 8 + sizeof(new_node_addr.sin_addr.s_addr))) = current_node->get_successors_port();
                        //cout<<"发回去的信息是:"<<"ip:"<<*((uint32_t*)(sendbuf+8))<<" port:"
                        //<<*((u_int16_t*)(sendbuf+8+sizeof(new_node_addr.sin_addr.s_addr)));
                        absolute_send(client_fd, sendbuf, sendbuf_length);
                        free(sendbuf);
                        //是否此时关闭client_fd? todo
                    }
                    else
                    {
                        cout << 2;
                        //当前节点的直接后继并不是,直接向下个节点发送此新节点的消息.
                        //向下一节点发送f nid ip port 然后等待数据.
                        int forward_length = 8 + 1 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                        uint8_t *forward_send_buf = (u_int8_t *)malloc(forward_length);
                        *forward_send_buf = 'f';
                        *((long *)(forward_send_buf + 1)) = node_hash;
                        *((uint32_t *)(forward_send_buf + 9)) = new_node_addr.sin_addr.s_addr;
                        *((u_int16_t *)(forward_send_buf + 9 + sizeof(new_node_addr.sin_addr.s_addr))) = new_node_addr.sin_port;
                        // cout<<"for ward发回去的信息是:"<<"ip:"<<new_node_addr.sin_addr.s_addr<<" port:"<<new_node_addr.sin_port;
                        //注意,这里要向下个后继发消息,
                        message_controller forward_message(current_node->get_successors_port(), current_node->get_successors_ip());
                        forward_message.send((char *)forward_send_buf, forward_length);
                        u_int8_t *forward_recv_buf = (u_int8_t *)malloc(forward_length - 1);
                        forward_message.recv((char *)forward_recv_buf, forward_length - 1);
                        absolute_send(client_fd, forward_recv_buf, forward_length - 1); //发送转发消息
                        //应该没有什么别的工作要做了
                        free(forward_send_buf);
                        free(forward_recv_buf);
                    }
                }
                else if (current_node->get_successors().first < current_node->get_nid())
                {
                    cout << 3;
                    if (node_hash < current_node->get_successors().first ||
                        node_hash > current_node->get_nid())
                    {
                        int sendbuf_length = 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                        u_int8_t *sendbuf = (u_int8_t *)malloc(sendbuf_length);
                        //下面设置要发送的数据
                        *((long *)sendbuf) = current_node->get_successors().first;
                        *((uint32_t *)(sendbuf + 8)) = current_node->get_successors_ip();
                        *((u_int16_t *)(sendbuf + 8 + sizeof(new_node_addr.sin_addr.s_addr))) = current_node->get_successors_port();
                        absolute_send(client_fd, sendbuf, sendbuf_length);
                        free(sendbuf);
                    }
                    else
                    {
                        int forward_length = 1 + 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                        uint8_t *forward_send_buf = (u_int8_t *)malloc(forward_length);
                        *forward_send_buf = 'f';
                        *((long *)(forward_send_buf + 1)) = node_hash;
                        *((uint32_t *)(forward_send_buf + 9)) = new_node_addr.sin_addr.s_addr;
                        *((u_int16_t *)(forward_send_buf + 9 + sizeof(new_node_addr.sin_addr.s_addr))) = new_node_addr.sin_port;
                        //注意,这里要向下个后继发消息,
                        message_controller forward_message(current_node->get_successors_port(), current_node->get_successors_ip());
                        forward_message.send((char *)forward_send_buf, forward_length);
                        u_int8_t *forward_recv_buf = (u_int8_t *)malloc(forward_length - 1);
                        forward_message.recv((char *)forward_recv_buf, forward_length - 1);
                        absolute_send(client_fd, forward_recv_buf, forward_length - 1); //发送转发消息
                        //应该没有什么别的工作要做了
                        free(forward_send_buf);
                        free(forward_recv_buf);
                    }
                }
                else
                {
                    //cout<<"here:"<<node_hash;
                    //等于的情况,就把自己发给新节点 hhhh
                    int sendbuf_length = 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                    u_int8_t *sendbuf = (u_int8_t *)malloc(sendbuf_length * sizeof(char));
                    //下面设置要发送的数据
                    *((long *)sendbuf) = current_node->get_nid();
                    *((uint32_t *)(sendbuf + 8)) = current_node->get_current_ip();
                    *((u_int16_t *)(sendbuf + 8 + sizeof(new_node_addr.sin_addr.s_addr))) = current_node->port;
                    absolute_send(client_fd, sendbuf, sendbuf_length);

                    free(sendbuf);
                }
            }
            else
            {
                //cout<<"error?";
                return false;
            }
        }
        else if (type == FORWARD)
        {
            //当该节点是遇到了转发的情况,
            long node_hash;
            absolute_recv(client_fd, &node_hash, 8);
            //拿到新节点的hash值,下面需要拿ip 和 port
            absolute_recv(client_fd, &new_node_addr.sin_addr.s_addr, 4);
            absolute_recv(client_fd, &new_node_addr.sin_port, 2);
            //下面是新节点加入的业务操作了
            if (current_node->get_successors().first > current_node->get_nid())
            {
                if (current_node->get_nid() < node_hash &&
                    node_hash < current_node->get_successors().first)
                {
                    //这个时候说明找到了,找到新node在这个环里面的直接后继了,把这个后继的信息,发给新node
                    int sendbuf_length = 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                    u_int8_t *sendbuf = (u_int8_t *)malloc(sendbuf_length);
                    //下面设置要发送的数据
                    *((long *)sendbuf) = current_node->get_successors().first;
                    *((uint32_t *)(sendbuf + 8)) = current_node->get_successors_ip();
                    *((u_int16_t *)(sendbuf + 8 + sizeof(new_node_addr.sin_addr.s_addr))) = current_node->get_current_port();
                    absolute_send(client_fd, sendbuf, sendbuf_length);
                    free(sendbuf);
                    //是否此时关闭client_fd? todo
                }
                else
                {
                    //当前节点的直接后继并不是,直接向下个节点发送此新节点的消息.
                    //向下一节点发送f nid ip port 然后等待数据.
                    int forward_length = 8 + 1 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                    uint8_t *forward_send_buf = (u_int8_t *)malloc(forward_length);
                    *forward_send_buf = 'f';
                    *((long *)(forward_send_buf + 1)) = node_hash;
                    *((uint32_t *)(forward_send_buf + 9)) = new_node_addr.sin_addr.s_addr;
                    *((u_int16_t *)(forward_send_buf + 9 + sizeof(new_node_addr.sin_addr.s_addr))) = new_node_addr.sin_port;
                    //注意,这里要向下个后继发消息,
                    message_controller forward_message(current_node->get_successors_port(), current_node->get_successors_ip());
                    forward_message.send((char *)forward_send_buf, forward_length);
                    u_int8_t *forward_recv_buf = (u_int8_t *)malloc(forward_length - 1);
                    forward_message.recv((char *)forward_recv_buf, forward_length - 1);
                    absolute_send(client_fd, forward_recv_buf, forward_length - 1); //发送转发消息
                    //应该没有什么别的工作要做了
                    free(forward_send_buf);
                    free(forward_recv_buf);
                }
            }
            else if (current_node->get_successors().first < current_node->get_nid())
            {
                if (node_hash < current_node->get_successors().first ||
                    node_hash > current_node->get_nid())
                {
                    int sendbuf_length = 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                    u_int8_t *sendbuf = (u_int8_t *)malloc(sendbuf_length);
                    //下面设置要发送的数据
                    *((long *)(sendbuf)) = current_node->get_successors().first;
                    *((uint32_t *)(sendbuf + 8)) = current_node->get_successors_ip();
                    *((u_int16_t *)(sendbuf + 8 + sizeof(new_node_addr.sin_addr.s_addr))) = current_node->get_successors_port();
                    absolute_send(client_fd, sendbuf, sendbuf_length);
                    free(sendbuf);
                }
                else
                {
                    int forward_length = 1 + 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                    uint8_t *forward_send_buf = (u_int8_t *)malloc(forward_length);
                    *forward_send_buf = 'f';
                    *((long *)(forward_send_buf + 1)) = node_hash;
                    *((uint32_t *)(forward_send_buf + 1 + 8)) = new_node_addr.sin_addr.s_addr;
                    *((u_int16_t *)(forward_send_buf + 8 + 1 + sizeof(new_node_addr.sin_addr.s_addr))) = new_node_addr.sin_port;
                    //注意,这里要向下个后继发消息,
                    message_controller forward_message(current_node->get_successors_port(), current_node->get_successors_ip());
                    forward_message.send((char *)forward_send_buf, forward_length);
                    u_int8_t *forward_recv_buf = (u_int8_t *)malloc(forward_length - 1);
                    forward_message.recv((char *)forward_recv_buf, forward_length - 1);
                    absolute_send(client_fd, forward_recv_buf, forward_length - 1); //发送转发消息
                    //应该没有什么别的工作要做了
                    free(forward_send_buf);
                    free(forward_recv_buf);
                }
            }
            else
            {
                //等于的情况,就把自己发给新节点 hhhh
                int sendbuf_length = 8 + sizeof(new_node_addr.sin_addr.s_addr) + sizeof(new_node_addr.sin_port);
                u_int8_t *sendbuf = (u_int8_t *)malloc(sendbuf_length);
                //下面设置要发送的数据
                *((long *)sendbuf) = current_node->get_nid();
                *((uint32_t *)(sendbuf + 8)) = current_node->get_current_ip();
                *((u_int16_t *)(sendbuf + 8 + sizeof(new_node_addr.sin_addr.s_addr))) = current_node->port;
                for (int i = 0; i < 14; i++)
                {
                    cout << sendbuf[i];
                }
                absolute_send(client_fd, sendbuf, sendbuf_length);
                free(sendbuf);
            }
        }
        else if (type == CHANGE_PRE)
        {
            //处理下这个了,当前节点的前置节点要改变了
            Nid node_hash;
            absolute_recv(client_fd, &node_hash, sizeof(Nid));
            uint32_t new_precessor_ip;
            absolute_recv(client_fd, &new_precessor_ip, sizeof(uint32_t));
            uint16_t new_precessor_port;
            absolute_recv(client_fd, &new_precessor_port, sizeof(uint16_t));
            //把自己的前继发给新节点.
            uint8_t *back_curr_preinfo = (uint8_t *)malloc(sizeof(Nid) + sizeof(uint32_t) + sizeof(uint16_t));
            Nid pre_hash;
            uint32_t pre_ip;
            uint32_t pre_port;
            pre_hash = current_node->get_predecessors().first; //前继节点的hash
            pre_ip = current_node->get_predecessors_ip();
            pre_port = current_node->get_predecessors_port();
            cout << "我发给你的原来的前置节点:" << pre_hash << " " << pre_ip << " " << pre_port;
            (*(long *)back_curr_preinfo) = pre_hash;
            (*(uint32_t *)(back_curr_preinfo + 8)) = pre_ip;
            (*(uint16_t *)(back_curr_preinfo + 12)) = pre_port;
            absolute_send(client_fd, back_curr_preinfo, sizeof(Nid) + sizeof(uint32_t) + sizeof(uint16_t));
            //是不是要加锁?.我觉得要 ==
            extern pthread_mutex_t precessor_mutex;
            pthread_mutex_lock(&precessor_mutex);
            current_node->set_predecessors(new_precessor_ip, new_precessor_port, node_hash);
            pthread_mutex_unlock(&precessor_mutex);
        }
        else if (type == CHANGR_SUC)
        {
            //current_node 的 后继变了
            Nid node_hash;
            absolute_recv(client_fd, &node_hash, sizeof(Nid));
            uint32_t new_successor_ip;
            absolute_recv(client_fd, &new_successor_ip, sizeof(uint32_t));
            uint16_t new_successor_port;
            absolute_recv(client_fd, &new_successor_port, sizeof(uint16_t));
            //修改后继,加锁
            extern pthread_mutex_t successor_info_mutex;
            pthread_mutex_lock(&successor_info_mutex);
            current_node->set_successors(new_successor_ip, new_successor_port, node_hash);
            pthread_mutex_unlock(&successor_info_mutex);
        }
        else if (type == PRE_LEAVE)
        {
            Nid node_hash;
            absolute_recv(client_fd, &node_hash, sizeof(Nid));
            uint32_t pre_ip;
            absolute_recv(client_fd, &pre_ip, sizeof(uint32_t));
            uint16_t pre_port;
            absolute_recv(client_fd, &pre_port, sizeof(uint16_t));
            extern pthread_mutex_t precessor_mutex;
            pthread_mutex_lock(&precessor_mutex);
            current_node->set_predecessors(pre_ip, pre_port, node_hash);
            pthread_mutex_unlock(&precessor_mutex);
        }
        else if (type == SUC_LEAVE)
        {
            Nid node_hash;
            absolute_recv(client_fd, &node_hash, sizeof(Nid));
            uint32_t suc_ip;
            absolute_recv(client_fd, &suc_ip, sizeof(uint32_t));
            uint16_t suc_port;
            absolute_recv(client_fd, &suc_port, sizeof(uint16_t));
            extern pthread_mutex_t successor_info_mutex;
            pthread_mutex_lock(&successor_info_mutex);
            current_node->set_successors(suc_ip, suc_port, node_hash);
            pthread_mutex_unlock(&successor_info_mutex);
        }
        else if (type == PUT)
        {
            CLOSE_FD = false;
            hash_storage_thread *hash_storage_task = new hash_storage_thread(client_fd, current_node);
            hash_storage_task->type = PUT;
            hash_storage_task->create_thread();
        }
        else if (type == GET)
        {
            CLOSE_FD = false;
            hash_storage_thread *hash_storage_task = new hash_storage_thread(client_fd, current_node);
            hash_storage_task->type = GET;
            hash_storage_task->create_thread();
        }
    }
    else
    {
        return false;
    }
}

//确保从读取n字节的数据.
ssize_t dht_ring_thread::absolute_recv(int fd, void *buf, size_t n)
{
    char *tmp_buf = (char *)buf;
    int index = 0;
    int counter = 0;
    int ret;
    while (index != n)
    {
        ret = ::recv(fd, buf, n, 0);
        if (ret == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                std::cout << strerror(errno);
                std::cout << "error: 接受数据错误" << std::endl;
                return -1;
            }
        }
        else if (ret == 0)
        {
            std::cout << "warning:对端关闭了连接" << std::endl;
            ::close(fd);
            return counter;
        }
        else
        {
            counter += ret;
            tmp_buf = tmp_buf + ret;
            index += ret;
        }
    }
    return n;
}
//确保发送n字节的数据
ssize_t dht_ring_thread::absolute_send(int client_socket, void *buf, size_t length)
{
    int size = length;
    while (size > 0)
    {
        int send_length = send(client_socket, buf, length, 0);
        //cout<<"向缓冲区写入了一次数据"<<endl;
        size -= send_length;
        buf = buf + send_length;
    }
    return length;
}

dht_ring_thread::~dht_ring_thread()
{
}