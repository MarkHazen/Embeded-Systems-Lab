//Use g++ -std=c++11 -o Lab4EX3 Lab4EX3.cpp -lwiringPi

#include <string>
#include <iostream>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
using namespace std;

int kobuki;

void movement(int, int);
void readData();

int main(){
	//Create connection to the Kobuki
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	bool hazardDetected = false;
	unsigned int bumper = 0;
	unsigned int drop = 0;
	unsigned int cliff = 0;
	unsigned int button = 0;
	unsigned int read = 0;

    while (serialDataAvail(kobuki) != -1) {
        while (true) {
            // If the bytes are a 1 followed by 15, then we are
            // parsing the basic sensor data packet
            read = serialGetchar(kobuki);
            if (read == 1) {
                if (serialGetchar(kobuki) == 15)
                    break;
            }
        }

        // Read past the timestamp
        serialGetchar(kobuki);
        serialGetchar(kobuki);

        /*Read the bytes containing the bumper, wheel drop,
        and cliff sensors. You can convert them into a usable data type.*/
        bumper = serialGetchar(kobuki);  // byte 2
        drop = serialGetchar(kobuki);    // byte 3
        cliff = serialGetchar(kobuki);   // byte 4

        /*Print the data to the screen.*/
        cout << "Bumper: " << bumper << "  Drop: " << drop << "  Cliff: " << cliff << endl;
        /*Read through 6 bytes between the cliff sensors and
        the button sensors.*/
        for (int i = 0; i < 6; i++)  // skip byte 5-10
            serialGetchar(kobuki);

        /*Read the byte containing the button data.*/
        button = serialGetchar(kobuki);  // byte 11
        cout << "Button: " << button << endl;

        /*Close the script and the connection to the Kobuki when
        Button 1 on the Kobuki is pressed. Use serialClose(kobuki);*/
        if (button & 0x02) {
            cout << "Button 1 pressed. Closing connection..." << endl;
            serialClose(kobuki);
            break;
        }

		bool hazard = (bumper == 6 || bumper == 1 || bumper == 2 || bumper == 3 || bumper == 4 || bumper == 5) ||
			(cliff > 0) ||
			(drop == 1 || drop == 2 || drop == 3);

		printf("%d", hazard);

		if(hazard && !hazardDetected) {
			hazardDetected = true;
			printf("Hazard Detected");

			movement(-100, 0);

			usleep(500000);

			int turn_dir = rand() % 2 ? 1: -1;

			movement(100, turn_dir);

			usleep(500000);

			movement(0, 0);
			hazardDetected = false;
		} //else movement(150, 0);

        // Pause the script so the data read receive rate is the same as the Kobuki send rate.
        usleep(20000);
    }
}

void movement(int sp, int r){
	//Create the byte stream packet with the following format:
	unsigned char b_0 = 0xAA ; /*Byte 0: Kobuki Header 0*/
	unsigned char b_1 = 0x55; /*Byte 1: Kobuki Header 1*/
	unsigned char b_2 = 0x06; /*Byte 2: Length of Payload*/
	unsigned char b_3 = 0x01; /*Byte 3: Sub-Payload Header (Base control)*/
	unsigned char b_4 = 0x04; /*Byte 4: Length of Sub-Payload*/
	
	//given to us
	unsigned char b_5 = sp & 0xff;	//Byte 5: Payload Data: Speed(mm/s)
	unsigned char b_6 = (sp >> 8) & 0xff; //Byte 6: Payload Data: Speed(mm/s)
	unsigned char b_7 = r & 0xff;	//Byte 7: Payload Data: Radius(mm)
	unsigned char b_8 = (r >> 8) & 0xff;	//Byte 8: Payload Data: Radius(mm)
	unsigned char checksum = 0;		//Byte 9: Checksum
	
	//Checksum all of the data
	char packet[] = {b_0,b_1,b_2,b_3,b_4,b_5,b_6,b_7,b_8};
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