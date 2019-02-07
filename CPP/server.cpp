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
#include <time.h>
// #include <pigpio.h>
#define PORT 8080 

#define BUFFER_SIZE 10000
long long int read_speed_values_s;

float scale = 10;
float shift = 5;
float trigger = 8;
float current;

int starting_port = 5;
int cycle_count = 0;
uint dummy_gpio_high = 0x7fe0;
uint dummy_gpio_low = 0;

float prev_value = 0;
float value;
// Specific For square wave **************************
	int count = 0;
	bool square_wave = true;
// Specific For square wave **************************

char trigger_function = 'r', trigger_mode = 'a';
bool start_storing = false, displaying_wait=false;
int point = 0;
int count_signal = 0;
double calculate_speed = 0;
clock_t t, curr_time, periodic_check, calculate_speed_clock;

int server_fd, new_socket, valread; 
struct sockaddr_in address; 
int opt = 1; 
int addrlen = sizeof(address); 
char buffer[1024] = {0}; 

// Net efficiency code ********************************



// Net efficiency code ********************************

union number {
    float d;
    uint32_t i;
};

number buffer_[BUFFER_SIZE];
number continuous_[BUFFER_SIZE];

char to_send[BUFFER_SIZE*4];
char to_send_continuous[BUFFER_SIZE*4];

using namespace std;

uint gpioRead_Bits_0_31()
{
	// Specific For square wave **************************
		count += 1;
		if (count%2600 == 1300)
		{
			square_wave = false;
		}
		else if(count%2600 == 0)
		{
			square_wave = true;
			count = 0;
		}

	// Specific For square wave **************************
	if (square_wave)
		return dummy_gpio_high;
	else
		return dummy_gpio_low;
}

float read_process_GPIO()
{
	current = (float)((gpioRead_Bits_0_31()>>5)&0x03ff)/1023.0;
	return current*scale - shift;
}

// Rising edge triggered trigger function
bool trigger_f_re(float &current_value)
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

// Falling edge triggered trigger function
bool trigger_f_fe(float &current_value)
{
	if(prev_value > trigger && current_value < trigger)
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
// class room of the elite

void send_data(int new_socket)
{
	for(int i=0;i<BUFFER_SIZE;++i)
	{
		to_send[4*i] = (buffer_[i].i>>24)%256;
		to_send[4*i+1] = (buffer_[i].i>>16)%256;
		to_send[4*i+2] = (buffer_[i].i>>8)%256;
		to_send[4*i+3] = (buffer_[i].i)%256;
	}
	// cout<<(int)to_send[0]<<" "<<(int)to_send[1]<<" "<<(int)to_send[2]<<" "<<(int)to_send[3]<<" "<<buffer_[0].i<<" "<<buffer_[0].d<<endl;
	send(new_socket , to_send , 4*BUFFER_SIZE , 0 );
}

void send_data_automatic(int new_socket)
{
	for(int i=0;i<BUFFER_SIZE;++i)
	{
		to_send_continuous[4*i] = (continuous_[i].i>>24)%256;
		to_send_continuous[4*i+1] = (continuous_[i].i>>16)%256;
		to_send_continuous[4*i+2] = (continuous_[i].i>>8)%256;
		to_send_continuous[4*i+3] = (continuous_[i].i)%256;
	}
	// cout<<(int)to_send[0]<<" "<<(int)to_send[1]<<" "<<(int)to_send[2]<<" "<<(int)to_send[3]<<" "<<buffer_[0].i<<" "<<buffer_[0].d<<endl;
	send(new_socket , to_send_continuous , 4*BUFFER_SIZE , 0 );
}

void display_data()
{
	for(int i=0;i<BUFFER_SIZE;++i)
	{
		cout<<buffer_[i].i<<" ";
	}
	cout<<endl<<cycle_count<<endl<<endl<<endl;
}

void main_f()
{
	value = read_process_GPIO();
	continuous_[count_signal].d = value;
	++count_signal;
	if(count_signal%BUFFER_SIZE == 0)
	{
		count_signal = 0;
	}

	if(trigger_function == 'r')
	{
		if(trigger_f_re(value) &&!start_storing &&!displaying_wait)
		{
			start_storing = true;
			periodic_check = clock();
		}
	}
	else if(trigger_function == 'f')
	{
		if(trigger_f_fe(value) &&!start_storing &&!displaying_wait)
		{
			start_storing = true;
			periodic_check = clock();
		}
	}

	if(((float)(clock() - periodic_check))/CLOCKS_PER_SEC > 0.1)
	{
		for(int i=0;i<BUFFER_SIZE;++i)
		{
			value = read_process_GPIO();
			continuous_[i].d = value;
		}
		if(trigger_mode=='n')
		{
			valread = read( new_socket , buffer, 1024);
			if(valread == 1)
				send_data(new_socket);
			else
				cout<<"Some error"<<valread<<endl;
		}
		else if(trigger_mode=='a')
		{
			valread = read( new_socket , buffer, 1024);
			if(valread == 1)
				send_data_automatic(new_socket);
			else
				cout<<"Some error"<<valread<<endl;
		}
		periodic_check = clock();
	}
	
	if (start_storing)
	{
		for(int point=0;point<BUFFER_SIZE;++point)
		{
			buffer_[point].d = value;
		}
		// Logic for displaying the values
		displaying_wait = true;
		curr_time = clock() - t;
		if(((float)curr_time)/CLOCKS_PER_SEC > 0.00625)
		{
			displaying_wait = false;
			start_storing = false;
			valread = read( new_socket , buffer, 1024);
			if(valread == 1)
				send_data(new_socket);
			else
				cout<<"Some error"<<valread<<endl;
			cycle_count++;
			t = clock();
		}
	}
}

int main(int argc, char const *argv[]) 
{ 
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed");
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	}
	while(true)
	{
		valread = read( new_socket , buffer, 1024); 
		char start[] = "starting";
		bool check=true;
		for(int i=0;i<8;++i)
		{
			check &= start[i] == buffer[i];
		}
		if(check){
			break;
		} 
	}
	send(new_socket , "started" , strlen("started") , 0 );
	
	prev_value = read_process_GPIO();
	t = clock();

	// For Triggering Modes ******************************
	periodic_check = clock();

	// For Triggering Modes ******************************

		
	// for(int i=0;i<BUFFER_SIZE;++i)
	// {
	// 	main_f();
	// }
	

	calculate_speed_clock = clock();
	long long int speed_count = 0;

	while(true)
	{
		main_f();
		// value = read_process_GPIO();

		++speed_count;
		if(speed_count%(100*BUFFER_SIZE) == 0)
		{
			speed_count = 0;
			calculate_speed = clock() - calculate_speed_clock;
			read_speed_values_s = (long long int)(100*BUFFER_SIZE/(((double)calculate_speed)/CLOCKS_PER_SEC));
			calculate_speed_clock = clock();
		}
	}
	return 0; 
}