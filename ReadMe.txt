Rashmi Prava Patro
Student ID: AK14498

CMSC 621: Advanced Operating Systems 
PROJECT 2

Part I: Berkeley Algorithm

File named Berkeley.cpp has the code for Part I. It can be compiled as below:

g++ -std=c++11 -o berkeley Berkeley.cpp -pthread

./berkeley process 1

./berkeley process 2

./berkeley process 3

./berkeley master

All client processes are executed before runnin the master program. Those client processes would be waiting for the master server to connect. Once 
it starts, all waiting processes would start running and generate a random logical clock for themselves. Those clock values are used further for clock 
synchronization.

Part II: Causal Ordering

File named CausalOrdering.cpp has the code for Part II. The non-causal ordering part is implemented in the file named NonCausalOrdering.cpp. These files 
can be compiled as below:

g++ -std=c++11 -o causalordering CausalOrdering.cpp -pthread

./causalordering 0 <message>

./causalordering 1 <message>

./causalordering 2 <message>

g++ -std=c++11 -o noncausalordering NonCausalOrdering.cpp -pthread

./noncausalordering 0 <message>

./noncausalordering 1 <message>

./noncausalordering 2 <message>

Process 0 is the first process, process 1 is the second process and process 2 is the third process. All these processes send messages to each other and themselves
as well. CausalOrdering.cpp program ensures that all the messages are reached in a causal ordered manner whereas NonCausalOrdering.cpp program displays how the
messages are delivered without the implementation of causal ordering.

Part III: Distributed Lock

This part is combined with the Part I itself.

Locks are used for the shared file and the file is read by all the processes and the counter value present in the file is updated accordingly.