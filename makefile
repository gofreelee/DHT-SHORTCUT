all: compile

compile: main.o utils.o message_controller.o dht_node.o dht_ring_thread.o hash_storage_thread.o task_thread.o base_thread.o
	g++ -o DHT main.o utils.o message_controller.o dht_node.o dht_ring_thread.o hash_storage_thread.o task_thread.o base_thread.o -lcrypto -lpthread -g

main.o:src/main.cpp
	g++ -c src/main.cpp -std=c++11 -g

utils.o:src/utils.cpp
	g++ -c src/utils.cpp -std=c++11 -g

message_controller.o:src/message_controller.cpp
	g++ -c src/message_controller.cpp -std=c++11 -g

dht_node.o:src/dht_node.cpp
	g++ -c src/dht_node.cpp -std=c++11 -g

dht_ring_thread.o:src/dht_ring_thread.cpp
	g++ -c src/dht_ring_thread.cpp -std=c++11 -g

hash_storage_thread.o:src/hash_storage_thread.cpp
	g++ -c src/hash_storage_thread.cpp -std=c++11 -g

task_thread.o:src/task_thread.cpp
	g++ -c src/task_thread.cpp -std=c++11 -g

base_thread.o:src/base_thread.cpp
	g++ -c src/base_thread.cpp -std=c++11 -g
