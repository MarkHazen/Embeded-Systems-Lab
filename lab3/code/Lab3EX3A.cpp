//Use g++ -std=c++11 -o Lab3EX3A Lab3EX3A.cpp -lwiringPi

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#define PORT 8080
using namespace std;

void movement(int, int);
void createSocket();
void readData();

int kobuki, new_socket;

/*Create char buffer to store transmitted data*/

int main(){
	//Initialize filestream for the Kobuki
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);

	//Create connection to client
	createSocket();

	while(true){
		//Read data from client
		readData();
	}
	return 0;
}

void movement(int sp, int r){
	//Create the byte stream packet with the following format:
	/*Byte 0: Kobuki Header 0*/
	/*Byte 1: Kobuki Header 1*/
	/*Byte 2: Length of Payload*/
	/*Byte 3: Payload Header*/
	/*Byte 4: Payload Data: Length*/
	/*Byte 5: Payload Data: Speed(mm/s)*/
	/*Byte 6: Payload Data: Speed(mm/s)*/
	/*Byte 7: Payload Data: Radius(mm)*/
	/*Byte 8: Payload Data: Radius(mm)*/
	/*Byte 9: Checksum*/

	/*Send the data to the Kobuki over a serial stream*/

	/*Checksum all the data and send that as well*/

	/*Pause the script so the data send rate is the
	same as the Kobuki receive rate*/
}

//Creates the connection between the client and
//the server with the Kobuki being the server
void createSocket(){
	int server_fd;
	struct sockaddr_in address;
	int opt =1;
	int addrlen = sizeof(address);

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family      = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port        = htons(PORT);

	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, 3) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
		perror("accept");
		exit(EXIT_FAILURE);
	}
}

void readData(){
	/*Read the incoming data stream from the controller*/


	
	
	/*Print the data to the terminal*/


	

	/*Use the received data to control the Kobuki*/

	
	if(/**/) {
	/*Closes out of all connections cleanly*/

	//When you need to close out of all connections, please
	//close both the Kobuki and TTP/IP data streams.
	//Not doing so will result in the need to restart
	//the raspberry pi and Kobuki
		close(new_socket);
		serialClose(kobuki);
		exit(0);
	}




	/*Reset the buffer*/
	memset(&/*buffer*/, '0', sizeof(/*buffer*/));


}
