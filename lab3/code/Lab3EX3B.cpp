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

int main(int argc, char const *argv[])
{

	// Open the file stream for the joystick
	Joystick joystick("/dev/input/js0");
	JoystickEvent event;
	if (!joystick.isFound())
	{
		cout << "Error opening joystick" << endl;
		exit(1);
	}

	// Create the connection to the server
	createSocket();

	while (true)
	{

		/*Sample the events from the joystick*/
		if (event.isAxis())
		{
			printf("isAxis: %u | Value: %d\n", event.number, event.value);
			/*Interpret the joystick input and use that input to move the Kobuki*/
			if (event.number == 7)
			{ // vertical axis
				if (event.value < 0)
					movement(250, 0); // up
				else if (event.value > 0)
					movement(-200, 0); // down
				else
					stopKobuki();
			}
			if (event.number == 6)
			{ // horizontal axis
				if (event.value < 0)
				{
					movement(200 / 2, 1);
				}
				if (event.value > 0)
				{
					movement(200 / 2, -1);
				}
			}

			int speed = 0;
			int radius = 0;

			if (event.number == 1)
			{
				if (abs(event.value) < 1000)
					speed = -event.value / 100;
				else
					speed = 0;
			}

			if (event.number == 3)
			{
				if (speed == 0)
				{
					radius = 1;
					speed = event.value / 100;
				}
				else if (speed != 0)
				{
				}
			}

			movement(-event.value / 50, radius);
		}
		/*Convert the event to a useable data type so it can be sent*/

		/*Print the data stream to the terminal*/

		/*Send the data to the server*/

		if (/**/)
		{
			/*Closes out of all connections cleanly*/

			// When you need to close out of the connection, please
			// close TTP/IP data streams.
			// Not doing so will result in the need to restart
			// the raspberry pi and Kobuki
			close(sock);
			exit(0);

			/*Set a delay*/
		}
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
		if (inet_pton(AF_INET, "XX.XX.XX.XX", &serv_addr.sin_addr) <= 0)
		{
			printf("\nInvalid address/ Address not supported \n");
			return -1;
		}

		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			printf("\nConnection Failed \n");
			return -1;
		}
		return 0;
	}
