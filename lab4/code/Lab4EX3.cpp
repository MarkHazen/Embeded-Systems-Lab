// Use g++ -std=c++11 -o Lab4EX3 Lab4EX3.cpp -lwiringPi

#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

int kobuki;

unsigned int bumper;
unsigned int drop;
unsigned int cliff;
unsigned int button;
unsigned int read;

void movement(int, int);
void readData();

int main() {
    // Create connection to the Kobuki
    wiringPiSetup();
    kobuki = serialOpen("/dev/kobuki", 115200);
    if (kobuki < 0) {
        cerr << "Failed to open Kobuki" << endl;
        return -1;
    }

    srand(time(NULL));  // for random movement

    cout << "===== Lab 4 Exercise 3: Random Autonomous Movement =====" << endl;

    while (serialDataAvail(kobuki) != -1) {
        /*Read the initial data. If there are no flags,
        the default condition is forward.*/
        readData();

        /*Move slowly to give the sensors enough time to read data,
        the recommended speed is 100mm/s*/

        if (button & 0x02) {  // Button 1 pressed
            cout << "Button 1 pressed. Stopping and closing connection..." << endl;
            movement(0, 0);
            serialClose(kobuki);
            break;
        }

        /*Create different states as to satisfy the conditions above.
        Remember, a single press of a bumper may last longer
        than one data cycle.*/
        if (cliff || drop) {
            cout << "Cliff or wheel drop detected! Reversing..." << endl;
            movement(-100, 0);
            usleep(800000);
            movement(0, 0);
            usleep(200000);
            int turn_dir = rand() % 2 ? 1 : -1;
            cout << "Avoiding hazard, turning " << (turn_dir == 1 ? "left" : "right") << endl;
            movement(100, turn_dir);
            usleep(1000000);
        } else if (bumper) {
            cout << "Bumper hit detected! ";
            if (bumper & 0x01) cout << "(Right) ";
            if (bumper & 0x02) cout << "(Center) ";
            if (bumper & 0x04) cout << "(Left) ";
            cout << endl;

            // Back up and randomly turn away
            movement(-100, 0);
            usleep(700000);
            movement(0, 0);
            usleep(200000);
            int turn_dir = rand() % 2 ? 1 : -1;
            movement(100, turn_dir);
            usleep(1000000);
        } else {
            // Default forward motion
            movement(100, 0);
        }

        /*Cleanly close out of all connections using Button 1.*/
        /*Use serialFlush(kobuki) to discard all data received, or waiting to be send down the given device.*/
        serialFlush(kobuki);
        usleep(20000);  // match data rate
    }
    return 0;
}

void movement(int sp, int r) {
    // you can reuse your code from Lab 3

    unsigned char b_0 = 0xAA;  // Header 0
    unsigned char b_1 = 0x55;  // Header 1
    unsigned char b_2 = 0x06;  // Payload length
    unsigned char b_3 = 0x01;  // Sub-payload header (Base Control)
    unsigned char b_4 = 0x04;  // Sub-payload data length

    unsigned char b_5 = sp & 0xff;         // Speed low byte
    unsigned char b_6 = (sp >> 8) & 0xff;  // Speed high byte
    unsigned char b_7 = r & 0xff;          // Radius low byte
    unsigned char b_8 = (r >> 8) & 0xff;   // Radius high byte
    unsigned char checksum = 0;

    char packet[] = {b_0, b_1, b_2, b_3, b_4, b_5, b_6, b_7, b_8};

    // Calculate checksum (XOR of bytes 2-8)
    for (unsigned int i = 2; i < 9; i++)
        checksum ^= packet[i];

    // Send bytes to the Kobuki
    for (int i = 0; i <= sizeof(packet); i++)
        serialPutchar(kobuki, packet[i]);

    serialPutchar(kobuki, checksum);
    usleep(20000);  // 20 ms to match Kobuki receive rate
}

void readData() {
    // you can reuse your code from EXE1, Lab 4

    while (true) {
        read = serialGetchar(kobuki);
        if (read == 1 && serialGetchar(kobuki) == 15) break;
    }

    // Skip timestamp (2 bytes)
    serialGetchar(kobuki);
    serialGetchar(kobuki);

    // Read sensor bytes
    bumper = serialGetchar(kobuki);
    drop = serialGetchar(kobuki);
    cliff = serialGetchar(kobuki);

    // Skip encoder/PWM (6 bytes)
    for (int i = 0; i < 6; i++) serialGetchar(kobuki);

    // Read button
    button = serialGetchar(kobuki);

    // Skip remaining 3 bytes
    for (int i = 0; i < 3; i++) serialGetchar(kobuki);
}
