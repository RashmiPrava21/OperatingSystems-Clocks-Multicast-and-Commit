/*CMSC 621 Advanced Operating Systems
Project 2
Rashmi Prava Patro - AK14498*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <ctime>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>

#define PORT 5000
#define IP "225.0.0.37"
//#define IP "127.0.0.1"
//#define INUSE "/home/rashmipatro/Documents/Project2/main/filecounter.txt"
using namespace std;


int main(int argc, char *argv[])
{
	//if(argc == 4)
	//{
	
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	cout <<"*********Start - Read the file*********" << endl;
	//FILE *f, *f1;
	pthread_mutex_lock(&mut);
	int file_value;
	FILE *f = fopen("/home/rashmipatro/Documents/Project2/project/filecounter.txt","r+");
	if(f == NULL)
	{
		cout << "Cannot open file to read" << endl;
		return 0;
	}
	//cout <<"Continue1.............." << endl;
	else
	{
		char buffer[256];
		fgets(buffer,sizeof(buffer),f);
		file_value = atoi(buffer);
		cout << "Current value in file: " << file_value << endl;
		//printf("Cu counter  is %d\n",count_lock);
		file_value++;
	}
	//printf("Updated value before writing %d\n",count_lock);
	//fprintf(fp, "%d\n",count_lock );
	fclose(f);
	//cout <<"Continue2.............." << endl;
	FILE *f1 = fopen("/home/rashmipatro/Documents/Project2/project/filecounter.txt","w+");
	if(f1 == NULL)
	{
		cout << "Cannot open file to write" << endl;
		return 0;
	}
	//cout <<"Continue3.............." << endl;
	else
	{
		fprintf(f1, "%d\n",file_value);
		//printf("Value after writing is %d\n",count_lock);
		cout << "Updated value: " << file_value << endl;
	}
	fclose(f1);
	//cout <<"Continue4.............." << endl;
	pthread_mutex_unlock(&mut);
	cout <<"*********Updated File Successfully*********" << endl;
	//}
	//continue;
	
	srand(time(NULL));
	int m_sock, p_sock;
	int logical_clock, process, p, m, send_clock, send_port;
	struct sockaddr_in p_addr, port_master, m_addr, m_r_addr;
	struct ip_mreq mreq;
	u_int on = 1;
	char message1[256];
	char message2[256];
	char message3[256];
	char message4[256];
	char buffer1[256];
	int m1, m2, m3;
	int initial_value, drift, send_drift;
	
	/*int rank;
	rank = atoi(argv[1]);*/
	srand(time(NULL));
	logical_clock = rand()%100;

	//create multicast socket
	m_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_sock < 0)
	{
		cout << "Cannot create socket" << endl;
		return 0;
	}
	
	//set up destination address	
	memset((char*) &m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = inet_addr(IP);
	m_addr.sin_port = htons(PORT);

	//set up destination address on receiver side
	memset((char*) &m_r_addr, 0, sizeof(m_r_addr));
	m_r_addr.sin_family = AF_INET;
	m_r_addr.sin_addr.s_addr =htonl(INADDR_ANY);
	m_r_addr.sin_port = htons(PORT);

	//allow multiple sockets to use the same PORT number
	if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		cout << "Unable to allow multiple sockets to use the same PORT number" << endl;
		return 0;
	}

	//bind to destination address
	m = bind(m_sock, (struct sockaddr *) &m_r_addr, sizeof(m_r_addr));
	if(m < 0)
	{
		cout << "Cannot bind" << endl;
		return 0;
	}

	//use setsockopt() to request the kernel to join the multicast group
	mreq.imr_multiaddr.s_addr = inet_addr(IP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	
	if(setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		cout << "Error in setsockopt" << endl;
		return 0;
	}
	
	//create point to point socket
	p_sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	//set destination address
	p_addr.sin_family = AF_INET;
	p_addr.sin_addr.s_addr = INADDR_ANY;
	p_addr.sin_port = htons(INADDR_ANY);

	//Request to use same address
	if(setsockopt(p_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		cout << "Cannot reuse address" << endl;
		return 0;
	}

	//Bind point to point socket
	p = bind(p_sock, (struct sockaddr *) &p_addr, sizeof(p_addr));
	if(p < 0)
	{
		cout << "Cannot bind point to point socket" << endl;
		return 0;
	}
	//sleep(5);
	socklen_t sa_len;
	//Send the master clock and master port number
	if(argc == 2)
	//if(rank == 0)
	{
		process = 0;
		cout << endl << endl;
		cout << "*********Starting Clock Synchronization*********" << endl;
		cout << "Logical Clock of Master: " << logical_clock << endl;
		
		//stores the int value in char
		char buffer[256];
		sprintf(buffer, "%d", logical_clock);
		//cout << "Buffer: " << buffer << endl;
		sa_len = sizeof(port_master);
		int sockname = getsockname(p_sock,(struct sockaddr *)&port_master,&sa_len);
		
		//multicast master clock to all the processes
		send_clock = sendto(m_sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &m_addr, sizeof(m_addr));
		//cout << "Sent logical clock value" << endl;
		
		//store the port number of master 
		int master_port = (int)ntohs(port_master.sin_port);
		//cout << "Master port: " << master_port << endl;
		//char buffer1[256];
		sprintf(buffer1, "%d", master_port);
		//cout << "Buffer1: " << buffer1 << endl;
		//multicast master port to all the processes
		send_clock = sendto(m_sock, buffer1, sizeof(buffer1), 0, (struct sockaddr *) &m_addr, sizeof(m_addr));
		//cout << "Sent port number" << endl;		
	}
		 
	
	//Receive master clock
	socklen_t addr = sizeof(m_r_addr);
	if(argc == 3)
	{
		if((m1 = recvfrom(m_sock, message1, sizeof(message1), 0, (struct sockaddr *) &m_r_addr, &addr)))
		{
			cout << endl << endl;
			cout << "*********Starting Clock Synchronization*********" << endl;
			cout << "Logical Clock of Master: " << message1 << endl;	
			process = atoi(argv[2]);
			cout << "Logical Clock of Process " << process << " is: " << logical_clock << endl;	
			//cout << "Logical Clock of Master: "<< message1 << endl;
		}
		
	}
	

	//Receive master port number
	//cout <<"Receiving Port number" << endl;
	if(argc == 3)
	{
		if((m1 = recvfrom(m_sock, message2, sizeof(message2), 0, (struct sockaddr *) &m_r_addr, &addr)))
		{
			//cout << "Message for port: " << message2 << endl;
			//process = atoi(argv[2]);
			//cout << "Logical Clock of Process " << process << " is: " << logical_clock << endl;	
			cout << "Port number of Master: "<< message2 << endl;
		}
	}

	if(argc == 2)
	{
		sprintf(message2, "%s", buffer1);
		cout << "Port number of Master: " << message2 << endl;
		//p_addr.sin_port = htons(atoi(message2));
	}
	//send drift to master
	if(argc == 3)
	{
		//cout << "Reached now: " << endl; 
		//sleep(5);
		//cout <<"Sending clock difference to master" << endl;
		initial_value = atoi(message1);
		drift = logical_clock - initial_value;
		//cout << "Before - P_addr.sin_port: " << p_addr.sin_port << endl;
		p_addr.sin_port = htons(atoi(message2));
		//cout << "After - P_addr.sin_port: " << p_addr.sin_port << endl;
		sprintf(message3, "%d", drift);
		//pthread_mutex_lock(&mut);
		send_drift = sendto(p_sock, message3, sizeof(message3), 0, (struct sockaddr *) &p_addr, sizeof(p_addr));
		cout << "Sent clock difference to master" << endl;
		//cout << "Drift: " << message3 << endl;
		//cout << "value of send_drift: " <<  send_drift << endl;
		
	}

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	if(setsockopt(p_sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout))<0)
	{
	  cout << "Timeout eror" << endl;  
	}

	
	//send the updated logical clock value to all the processes
	
	
	//int berkeley_calculate = recv(p_sock, buffer2, sizeof(buffer2), 0);
	//cout << "Value of berkeley_calculate: " << berkeley_calculate << " Message: " << buffer2 << endl;
	/*while(berkeley_calculate >= 0)
	{
		//pthread_mutex_lock(&trial);
		if(argc == 2)
		{
			pthread_mutex_lock(&trial);
			cout << "C: " << counter << endl;
			int process_clock = atoi(buffer2);
			sum = sum + process_clock;
			counter = counter + 1;
			cout << "C: " << counter << endl;
			pthread_mutex_unlock(&trial);
		}
		cout << "Sum: " << sum << endl;
		cout << "Counter: " << counter << endl;
		average = sum/(counter + 1);
		cout << "Total number of processes: " << (counter + 1) << endl;
		logical_clock = logical_clock + average;
		char buffer3[256];
		sprintf(buffer3, "%d", logical_clock);
		send_updatedclock = sendto(m_sock, buffer3, sizeof(buffer3), 0, (struct sockaddr *) &m_addr, sizeof(m_addr));
		//pthread_mutex_unlock(&trial);
	}*/
	char buffer2[256];
	int sum = 0, average = 0, counter = 0, send_updatedclock;
	socklen_t addr1 = sizeof(p_addr);
	if(argc == 2)
	{
		//m1 = recvfrom(p_sock, buffer2, sizeof(buffer2), 0, (struct sockaddr *) &p_addr, &addr1);
		//cout << "Value of berkeley_calculate: " << m1 << " Message: " << buffer2 << endl;
		//receive logical clock of other processes
		while((recvfrom(p_sock, buffer2, sizeof(buffer2), 0, (struct sockaddr *) &p_addr, &addr1)) >= 0)
		{
			//if(argc == 2)
			//{
			sum = sum + atoi(buffer2);
			counter++;
			//cout << "S: " << sum << endl;
			//cout << "C: " << counter << endl;
			//}
			
		}
		//cout << "Sum: " << sum << endl;
		//cout << "Counter: " << counter << endl;
		average = sum/(counter + 1);
		//cout << "Average: " << average << endl;
		cout << "Total number of processes: " << (counter + 1) << endl;
		logical_clock = logical_clock + average;
		cout << "Updated value of logical clock of master: " << logical_clock << endl;
		char buffer3[256];
		sprintf(buffer3, "%d", logical_clock);
		send_updatedclock = sendto(m_sock, buffer3, sizeof(buffer3), 0, (struct sockaddr *) &m_addr, sizeof(m_addr));
		cout << "****Multicasted the updated logical clock of master to all processes****" << endl;		
		
	}

	
	//receive master updated logical clock value
	if(argc == 3)
	{
		if((m1 = recvfrom(m_sock, message4, sizeof(message4), 0, (struct sockaddr *) &m_r_addr, &addr)))
		{
		 
			//m3 = recvfrom(m_sock, message4, sizeof(message4), 0, (struct sockaddr *) &m_r_addr, &addr);		
			int drift1 = atoi(message4) - logical_clock;
			//cout << "Drift1: " << drift1 << endl;
			//int offset = drift1 - atoi(message4);
			logical_clock = logical_clock + drift1;
			//cout << "Final clock value: " << logical_clock << endl;
			cout << "Logical clock of process " << process << " is: " << logical_clock << endl;
			cout << "*********Clock synchronized successfully*********" << endl;
		}
	}
	
}

/*References:
1. https://stackoverflow.com/questions/4233598/about-setsockopt-and-getsockopt-function
2. ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/example.html
3. https://stackoverflow.com/questions/18050065/specifying-port-number-on-client-side
4. https://social.msdn.microsoft.com/Forumns/vstudio/en-US/da286445-72b3-4b0c-87f7-4703121f9974/how-to-put-int-values-to-char-array?forum=vcgeneral
5. https://msdn.microsoft.com/en-us/library/windows/desktop/ms738543(v=vs.85).aspx
*/
	
