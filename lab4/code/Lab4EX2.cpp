// use g++ -std=c++11 -o Lab4EX2 Lab4EX2.cpp -lwiringPi

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>
using namespace std;
using namespace std::chrono;

int kobuki;
float read_sonar();
void movement(int, int);

int main() {
    wiringPiSetup();
    kobuki = serialOpen("/dev/kobuki", 115200);
    if (kobuki < 0) {
        cerr << "Failed to open Kobuki" << endl;
        return -1;
    }  // debug

    /*Move from a random point within the area designated "X" to the
      point B as shown on the diagram. Use a sonar sensor to navigate through the channel.
      You can reuse your code from Lab 2 and 3*/
    float dist;

    // ---- a. Move to first turn ----
    cout << "[A] Moving toward first turn..." << endl;
    while (true) {
        dist = read_sonar();
        if (dist < 25.0) {  // obstacle ahead
            movement(0, 0);
            break;
        }
        movement(200, 0);  // straight
        usleep(20000);
    }

    // ---- b. Turn 90 degrees ----
    cout << "[B] Turning 90 degrees to the right..." << endl;
    movement(200 / 2, -1);  // rotate right
    usleep(2300000);        // ~2.3s = 90 degrees (adjust)
    movement(0, 0);
    usleep(500000);

    // ---- c. Move to second turn ----
    cout << "[C] Moving toward second turn..." << endl;
    while (true) {
        dist = read_sonar();
        if (dist < 25.0) {
            cout << "Reached second turn (obstacle < 25 cm)" << endl;
            movement(0, 0);
            break;
        }
        movement(200, 0);
        usleep(20000);
    }

    // ---- d. Turn 90 degrees ----
    cout << "[D] Turning 90 degrees to the left..." << endl;
    movement(200 / 2, 1);  // rotate left
    usleep(2300000);
    movement(0, 0);
    usleep(500000);

    // ---- e. Cross the final line ----
    cout << "[E] Crossing final line at corridor exit..." << endl;
    movement(200, 0);
    usleep(3000000);  // move forward 3s to fully cross
    movement(0, 0);

    serialClose(kobuki);
    return 0;

    /*Note: the Kobuki must completely pass point B as shown to receive full credit*/
}

float read_sonar() {
    // you can reuse your code from Lab 2

    int sonar_pin = 1;
    int pulse_width = 0;
    const int MAX_ECHO_TIME = 18500;

    /*Set the pinMode to output and generate a LOW-HIGH-LOW signal using "digitalWrite" to trigger the sensor.
    Use a 2 us delay between a LOW-HIGH and then a 5 us delay between HIGH-LOW. You can use
    the function "usleep" to set the delay. The unit of usleep is microsecond. */

    // Set the pinMode to output
    pinMode(sonar_pin, OUTPUT);

    // Generate a LOW-HIGH-LOW signal
    digitalWrite(sonar_pin, LOW);
    usleep(2);  // 2 microseconds delay
    digitalWrite(sonar_pin, HIGH);
    usleep(5);  // 5 microseconds delay
    digitalWrite(sonar_pin, LOW);

    /*Echo holdoff delay 750 us*/
    usleep(750);

    /*Switch the pinMode to input*/
    pinMode(sonar_pin, INPUT);

    /*Get the time it takes for signal to leave sensor and come back.*/

    // 1. define a variable to get the current time t1. Refer to "High_Resolution_Clock_Reference.pdf" for more information
    auto t1 = chrono::high_resolution_clock::now();
    while (digitalRead(sonar_pin)) {
        /*read signal pin, then stay in the loop when the signal pin is high*/
        // 2. define a variable to get the current time t2.
        auto t2 = chrono::high_resolution_clock::now();
        // 3. calculate the time duration: t2 - t1
        pulse_width = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
        // 4. if the duration is larger than the Pulse Maximum 18.5ms, break the loop.
        if (pulse_width >= MAX_ECHO_TIME) {
            break;
        }
    }

    /*Calculate the distance by using the time duration that you just obtained.*/  // Speed of sound is 340m/s
    double distance = (pulse_width * 340.0 / 1000000) / 2.0;                       // in meters
    distance = distance * 100.0;                                                   // convert to cm
    /*Print the distance.*/
    cout << "Distance: " << distance << " cm" << endl;
    /*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
    usleep(60000);  // 60 ms delay
    return distance;
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

    // Match Kobuki’s 50 Hz command rate
    usleep(20000);
}

void read_kobuki_sensors() {  // Created Function from lab 1 to read kobuki sensors
    unsigned int read, bumper, drop, cliff, button;

    if (serialDataAvail(kobuki) == -1) return;

    // Wait for the Basic Sensor Data packet
    while (true) {
        read = serialGetchar(kobuki);
        if (read == 1 && serialGetchar(kobuki) == 15) break;
    }

    serialGetchar(kobuki);  // timestamp low
    serialGetchar(kobuki);  // timestamp high
    bumper = serialGetchar(kobuki);
    drop = serialGetchar(kobuki);
    cliff = serialGetchar(kobuki);

    for (int i = 0; i < 6; i++) serialGetchar(kobuki);  // skip encoder+PWM
    button = serialGetchar(kobuki);
    for (int i = 0; i < 3; i++) serialGetchar(kobuki);  // skip rest

    if (bumper) {
        cout << "Bumper hit!  ";
        if (bumper & 0x01) cout << "Right ";
        if (bumper & 0x02) cout << "Center ";
        if (bumper & 0x04) cout << "Left ";
        cout << endl;
    }
    if (button & 0x02) {  // middle button
        movement(0, 0);
        serialClose(kobuki);
        exit(0);
    }
}
