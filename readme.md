## 分布式哈希表的简单介绍
分布式哈希表,与单机的哈希表不同之处在于,单机的哈希表是将"key-value"结构存储在一台机器上.当这台机器出问题时,所有的数据都将受到影响.而分布式哈希表,首先每个主机根据自身的"ip:port"计算出一个hash值.每个主机根据这个hash值负责一部分的数据管理.

## short-cut环形DHT
根据所有参与到存储中的节点的hash值,将它们构建成一个环.每个节点存储着前置节点与后继节点的信息.

### create
DHT在创建时,当前的环中只有一个节点--即初始节点.该节点的前置节点与后继节点均为自己本身. 

![avatar](/pic/create.png)
### join 
在程序中执行如下指令:join <ip> <port> 可通过ip:port所标识的对应主机的进程来加入集群.

![avatar](/pic/join.png)
### put 
在程序中执行如下命令: put <key> <value> 会将该键值对存储到最适合它的主机上.

![avatar](/pic/put.png)
### get
在程序中执行如下命令: get <key> 会从集群中获取该key所对应的value

![avatar](/pic/get.png)
### info
在程序中执行如下命令: info 会获取当前节点自身的信息

![avatar](/pic/info.png)
### ls
在程序中执行如下命令: ls 会获取当前节点的前置节点信息与后继节点信息

![avatar](/pic/ls.png)
### shortcut
在程序中执行如下命令: shortcut <ip1> <port1> <ip2> <port2> 会配置当前节点的shortcuts

![avatar](/pic/shortcut.png)
### leave
在程序中执行如下命令: leave 会离开当前所在集群

#### leave前:
![avatar](/pic/leave0.png)

#### leave后:
![avatar](/pic/leave1.png)

## 使用:
git clone到本地

cd DHT

make

./DHT
