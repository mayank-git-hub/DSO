// Server side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <linux/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
// #include <pigpio.h>
#define PORT 8080 

#define BUFFER_SIZE 1000

double scale = 10;
double shift = 5;
double trigger = 4;
double current;

int starting_port = 5;
uint dummy_gpio_high = 0x7fe0;
uint dummy_gpio_low = 0;
bool square_wave = true;
double prev_value = 0;

using namespace std;

uint gpioRead_Bits_0_31()
{
	if (square_wave)
		return dummy_gpio_high;
	else
		return dummy_gpio_low;
}

double read_process_GPIO()
{
	current = (double)((gpioRead_Bits_0_31()>>5)&0x03ff)/1023.0;
	return current*scale - shift;
}

bool trigger_f(double &current_value)
{
	if(prev_value < trigger && current_value > trigger)
	{
		prev_value = current_value;
		return true;
	}
	else
	{
		prev_value = current_value;
		return false;
	}
}

int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
	char to_send[BUFFER_SIZE] = "Hello from server";
	double buffer_[BUFFER_SIZE];

	
	// Creating socket file descriptor 
	// if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	// { 
	// 	perror("socket failed");
	// 	exit(EXIT_FAILURE); 
	// } 
	
	// // Forcefully attaching socket to the port 8080 
	// if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
	// 											&opt, sizeof(opt))) 
	// { 
	// 	perror("setsockopt"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	// address.sin_family = AF_INET; 
	// address.sin_addr.s_addr = INADDR_ANY; 
	// address.sin_port = htons( PORT ); 
	
	// // Forcefully attaching socket to the port 8080 
	// if (bind(server_fd, (struct sockaddr *)&address, 
	// 							sizeof(address))<0) 
	// { 
	// 	perror("bind failed"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	// if (listen(server_fd, 3) < 0) 
	// { 
	// 	perror("listen"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	// if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
	// 				(socklen_t*)&addrlen))<0) 
	// { 
	// 	perror("accept"); 
	// 	exit(EXIT_FAILURE); 
	// }
	// while(true)
	// {
	// 	valread = read( new_socket , buffer, 1024); 
	// 	char start[] = "starting";
	// 	bool check=true;
	// 	for(int i=0;i<8;++i)
	// 	{
	// 		check &= start[i] == buffer[i];
	// 	}
	// 	if(check){
	// 		break;
	// 	} 
	// }
	int count = 0;
	prev_value = read_process_GPIO();
	double value;
	bool start_storing = false;
	int point = 0;
	while(true)
	{
		value = read_process_GPIO();
		if(trigger_f(value) &&!start_storing)
		{
			start_storing = true;
			point = 0;
		}
		// cout<<value<<" ";
		count += 1;
		if (count%200 == 100)
			square_wave = false;
		else if(count%200 == 0)
			square_wave = true;
		
		if (start_storing)
		{
			if(point == BUFFER_SIZE)
			{
				// Logic for displaying the values
				start_storing = false;
				for(int i=0;i<BUFFER_SIZE;++i)
				{
					cout<<buffer_[i]<<" ";
				}
				cout<<endl<<endl;
			}
			else
			{
				buffer_[point] = value;
				++point;
			}
		}
	}
	// send(new_socket , to_send , strlen(to_send) , 0 ); 
	// printf("Hello message sent\n"); 
	// send(new_socket , to_send , strlen(to_send) , 0 ); 
	// printf("Hello message sent\n"); 
	return 0; 
} 
