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
float THRESHOLD = 20;
bool running = true;
int sonar_pin = 1;
int MAX_ECHO_TIME = 18500;

int currentState = 0;

int main() {
    wiringPiSetup();
    kobuki = serialOpen("/dev/kobuki", 115200);
    if (kobuki < 0) {
        cerr << "Failed to open Kobuki" << endl;
        return -1;
    }

    float dist;

    int sp = 250;  // speed in mm/s
    int r = 500;   // radius 50 cm
    int b = 230;   // distance between 2 wheels
    int w = 1;

    while (running) {
        float distance = read_sonar();

        printf("State: %d\n", currentState);

        switch (currentState) {
            case 0:
                if (distance < THRESHOLD) {
                    printf("Break State\n");
                    movement(0, 0);
                    currentState = 1;
                    break;
                }

                movement(200, 0);

                break;
            case 1:
                movement(w * b / 2, -1);

                usleep(2000000);

                movement(0, 0);
                currentState = 2;

                break;
            case 2:
                if (distance < THRESHOLD) {
                    movement(0, 0);
                    currentState = 3;
                    break;
                }

                movement(200, 0);

                break;
            case 3:
                movement(w * b / 2, 1);

                usleep(2000000);

                movement(0, 0);
                currentState = 4;

                break;
            case 4:
                if (distance < THRESHOLD) {
                    movement(0, 0);
                    running = false;
                    break;
                }

                movement(200, 0);

                break;
        }
    }

    serialClose(kobuki);
    return 0;
}

float read_sonar() {
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

    int pulse_width = 0;

    /*Get the time it takes for signal to leave sensor and come back.*/

    // 1. define a varable to get the current time t1. Refer to "High_Resolution_Clock_Reference.pdf" for more information
    auto t1 = high_resolution_clock::now();
    while (digitalRead(sonar_pin)) { /*read signal pin, the stay is the loop when the signal pin is high*/
        // 2. defind a varable to get the current time t2.
        auto t2 = high_resolution_clock::now();
        // 3. calculate the time duration: t2 - t1
        pulse_width = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
        // cout << "1" << endl;
        //  4. if the duration is larger than the Pulse Maxium 18.5ms, break the loop.
        if (pulse_width >= MAX_ECHO_TIME) {
            break;
        }
    }

    /*Calculate the distance by using the time duration that you just obtained.*/  // Speed of sound is 340m/s
    double distance = (pulse_width * 340.0 / 1000000) / 2.0;                       // in meters
    distance = distance * 100.0;                                                   // convert to cm
    /*Print the distance.*/
    cout << "Distance: " << distance << " cm" << endl;
    return distance;
    /*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
    usleep(60000);  // 60 ms delay
}

void movement(int sp, int r) {
    // Create the byte stream packet with the following format:
    unsigned char b_0 = 0xAA; /*Byte 0: Kobuki Header 0*/
    unsigned char b_1 = 0x55; /*Byte 1: Kobuki Header 1*/
    unsigned char b_2 = 0x06; /*Byte 2: Length of Payload*/
    unsigned char b_3 = 0x01; /*Byte 3: Sub-Payload Header (Base control)*/
    unsigned char b_4 = 0x04; /*Byte 4: Length of Sub-Payload*/

    // given to us
    unsigned char b_5 = sp & 0xff;         // Byte 5: Payload Data: Speed(mm/s)
    unsigned char b_6 = (sp >> 8) & 0xff;  // Byte 6: Payload Data: Speed(mm/s)
    unsigned char b_7 = r & 0xff;          // Byte 7: Payload Data: Radius(mm)
    unsigned char b_8 = (r >> 8) & 0xff;   // Byte 8: Payload Data: Radius(mm)
    unsigned char checksum = 0;            // Byte 9: Checksum

    // Checksum all of the data
    char packet[] = {b_0, b_1, b_2, b_3, b_4, b_5, b_6, b_7, b_8};
    for (unsigned int i = 2; i < 9; i++)
        checksum ^= packet[i];

    /*Send the data (Byte 0 - Byte 8 and checksum) to Kobuki using serialPutchar (kobuki, );*/
    for (int i = 0; i < sizeof(packet); i++) {
        serialPutchar(kobuki, packet[i]);
    }

    // serialPutchar(kobuki, packet);
    serialPutchar(kobuki, checksum);

    /*Pause the script so the data send rate is the
    same as the Kobuki data receive rate*/
    usleep(20000);
}
