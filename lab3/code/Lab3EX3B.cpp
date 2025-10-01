// Use g++ joystick.cc -std=c++11 -o Lab3EX3B Lab3EX3B.cpp

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include "joystick.hh"
#include <cstdlib>
#define PORT 8080
using namespace std;

int createSocket();

int sock = 0;

int main(int argc, char const *argv[]) {
	cout << "1" << endl;

	int speed;
	int radius;
	bool quit = false;

	cout << "2" << endl;

	// Open the file stream for the joystick
	Joystick joystick("/dev/input/js0");
	JoystickEvent event;
	if (!joystick.isFound()) {
		cout << "Error opening joystick" << endl;
		exit(1);
	}

	cout << "3" << endl;

	// Create the connection to the server
	createSocket();

	cout << "4" << endl;

	while (true) {
		int speed = 0;
		int radius = 0;

		if (joystick.sample(&event)){
			if (event.isButton()){
				printf("isButton: %u | Value: %d\n", event.number, event.value);
				/*Interpret the joystick input and use that input to move the Kobuki*/
				if (event.value == 1){ // on button press
					switch (event.number){
					case 6: // Select
						speed = 0;
						radius = 0;
						quit = true;
						printf("QUIT");
					}
				}
			}
		}

		if (event.isAxis()){
			printf("isAxis: %u | Value: %d\n", event.number, event.value);
			/*Interpret the joystick input and use that input to move the Kobuki*/
			// if (event.number == 7) {       // vertical axis
			// 	if (event.value < 0) movement(250, 0);     // up
			// 	else if (event.value > 0) movement(-200,0);// down
			// 	else stopKobuki();
			// } 
			// if (event.number == 6) {       // horizontal axis
			// 	if (event.value < 0) { 
			// 		speed = 200 / 2;
			// 		radius = 1;
			// 	}
			// 	if (event.value > 0) {
			// 		speed = 200 / 2;
			// 		radius = -1;
			// 	}
			// }

			if(event.number == 1) {
				printf("Left Stick\n");
				if(abs(event.value) < 1000)
					speed = -event.value/100;
			}

			if(event.number == 3) {
				printf("Right Stick\n");
				if(speed == 0) {
					radius = 1;
					speed = event.value/100;
				}
				else if(speed != 0){}

			}
		}

		/*Convert the event to a useable data type so it can be sent*/
		int data[2];
		data[0] = speed;
		data[1] = radius;
		
		/*Print the data stream to the terminal*/
		std::cout << speed << ", " << radius << std::endl;

		/*Send the data to the server*/
		send(sock, data, sizeof(data), 0);

		if (quit) {
			/*Closes out of all connections cleanly*/
			// When you need to close out of the connection, please
			// close TTP/IP data streams.
			// Not doing so will result in the need to restart
			// the raspberry pi and Kobuki
			close(sock);
			exit(0);

			/*Set a delay*/
			usleep(20000);
		}
	} //end while

	return 0;
}

	// Creates the connection between the client and
	// the server with the controller being the client
	int createSocket()
	{
		struct sockaddr_in address;
		struct sockaddr_in serv_addr;

		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("\nSocket creation error \n");
			return -1;
		}

		memset(&serv_addr, '0', sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);

		/*Use the IP address of the server you are connecting to*/
		if (inet_pton(AF_INET, "10.227.24.107", &serv_addr.sin_addr) <= 0)		{
			printf("\nInvalid address/ Address not supported\n");
			return -1;
		}

		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			printf("\nConnection Failed \n");
			return -1;
		}
		return 0;
	}
