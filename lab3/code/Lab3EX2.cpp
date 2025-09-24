// Use g++ joystick.cc -std=c++11 -o Lab3EX2 Lab3EX2.cpp -lwiringPi

#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "joystick.hh"
#include <unistd.h>
#include <cstdlib>
using namespace std;

void movement(int, int);
int kobuki;
void stopKobuki();

bool hasPressed = false;

int main(){
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	Joystick joystick("/dev/input/js0");
	JoystickEvent event;
	unsigned int button;

	// The joystick creates events when a button or axis changes value.
	// Sample event from the joystick: joystick.sample(&event)

	// You can interpret these by sampling the events.
	// Each event has three parameters.
	// A type, axis or button,
	// judge if the event is button: event.isButton()
	// judge if the event is axis: event.isAxis()
	// A number corresponding to the axis or button pressed: event.number
	// And a value, Buttons: 0-unpressed, 1-pressed, Axis: -32767 to 0 to 32767: event.value

	while (true){
		/*Create a series of commands to interpret the
		joystick input and use that input to move the Kobuki*/

		// Use the following Key Map:
		// Up     - move the Kobuki forward
		// Down   - move the Kobuki backward
		// Left   - rotate the Kobuki 90 degrees counterclockwise
		// Right  - rotate the Kobuki 90 degrees clockwise
		// Start  - immediately stop the Kobuki's movement
		// Select - exit the script and close the Kobuki's connection cleanly
		if (joystick.sample(&event)){
			if (event.isButton()){
				printf("isButton: %u | Value: %d\n", event.number, event.value);
				/*Interpret the joystick input and use that input to move the Kobuki*/
				if (event.value == 1){ // on button press
					switch (event.number){
					case 7: // Start
						stopKobuki();
						break;
					case 6: // Select
						stopKobuki();
						serialClose(kobuki);
						return 0;
					}
				}
				else{ // Stop when Up/Down released
					if (event.number == 12 || event.number == 13)
						stopKobuki();
				}
			}
		}
	
		if (event.isAxis()){
			printf("isAxis: %u | Value: %d\n", event.number, event.value);
			/*Interpret the joystick input and use that input to move the Kobuki*/
			if (event.number == 7) {       // vertical axis
				if (event.value < 0) movement(250, 0);     // up
				else if (event.value > 0) movement(-200,0);// down
				else stopKobuki();
			} 
			if (event.number == 6) {       // horizontal axis
				if (event.value < 0) { 
					movement(200/2,1);
				}
				if (event.value > 0) {
					movement(200/2,-1);
				}
			}

			int speed = 0;
			int radius = 0;

			if(event.number == 1) {
				if(abs(event.value) < 1000)
					speed = -event.value/100;
				else speed = 0;
			}

			if(event.number == 3) {
				if(speed == 0) {
					radius = 1;
					speed = event.value/100;
				}
				else if(speed != 0){

				}

			}

			movement(-event.value/50, radius);
		}
	}
	return 0;
}
void movement(int sp, int r){

	// Create the byte stream packet with the following format:
	unsigned char b_0 = 0xAA;									/*Byte 0: Kobuki Header 0*/
	unsigned char b_1 = 0x55;									/*Byte 1: Kobuki Header 1*/
	unsigned char b_2 = 0x06; /*Byte 2: Length of Payload*/		// id + length + 4 bytes
	unsigned char b_3 = 0x01;									/*Byte 3: Sub-Payload Header (Base control)*/
	unsigned char b_4 = 0x04; /*Byte 4: Length of Sub-Payload*/ // length of data (speed + radius)

	unsigned char b_5 = sp & 0xff;		  // Byte 5: Payload Data: Speed(mm/s)
	unsigned char b_6 = (sp >> 8) & 0xff; // Byte 6: Payload Data: Speed(mm/s)
	unsigned char b_7 = r & 0xff;		  // Byte 7: Payload Data: Radius(mm)
	unsigned char b_8 = (r >> 8) & 0xff;  // Byte 8: Payload Data: Radius(mm)
	unsigned char checksum = 0;			  // Byte 9: Checksum

	// Checksum all of the data
	char packet[] = {b_0, b_1, b_2, b_3, b_4, b_5, b_6, b_7, b_8};
	for (unsigned int i = 2; i < 9; i++)
		checksum ^= packet[i];

	/*Send the data (Byte 0 - Byte 8 and checksum) to Kobuki using serialPutchar (kobuki, );*/
	for(int i = 0; i <= sizeof(packet); i++) {
		serialPutchar(kobuki, packet[i]);
	}

	// serialPutchar(kobuki, packet);
	serialPutchar(kobuki, checksum);

	/*Pause the script so the data send rate is the
	same as the Kobuki data receive rate*/
	usleep(20000);
}

// Function to stop the Kobuki
void stopKobuki(){
	movement(0, 0); // Send zero speed to stop
	usleep(100000); // Wait for 100 ms to ensure the stop command is received
}