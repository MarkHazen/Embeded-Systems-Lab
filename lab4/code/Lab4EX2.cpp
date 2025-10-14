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
    }

    float dist;

    // ---- a. Move to first turn ----
    cout << "[A] Moving toward first turn..." << endl;
    while (true) {
        read_kobuki_sensors();  // check Kobuki internal sensors
        dist = read_sonar();    // get sonar distance

        if (dist < 25.0) {  // obstacle/wall detected
            cout << "Reached first turn (wall < 25 cm)" << endl;
            movement(0, 0);
            break;
        }
        movement(200, 0);  // forward motion
        usleep(20000);
    }

    // ---- b. Turn 90° right ----
    cout << "[B] Turning 90° to the right..." << endl;
    movement(100, -1);  // rotate right
    usleep(2300000);    // ≈90 degrees
    movement(0, 0);
    usleep(500000);

    // ---- c. Move to second turn ----
    cout << "[C] Moving toward second turn..." << endl;
    while (true) {
        read_kobuki_sensors();
        dist = read_sonar();

        if (dist < 25.0) {
            cout << "Reached second turn (wall < 25 cm)" << endl;
            movement(0, 0);
            break;
        }
        movement(200, 0);
        usleep(20000);
    }

    // ---- d. Turn 90° left ----
    cout << "[D] Turning 90° to the left..." << endl;
    movement(100, 1);  // rotate left
    usleep(2300000);
    movement(0, 0);
    usleep(500000);

    // ---- e. Cross the final line ----
    cout << "[E] Crossing final line at corridor exit..." << endl;
    movement(200, 0);
    usleep(3000000);  // drive straight to exit
    movement(0, 0);

    cout << "Navigation complete — Kobuki has exited the corridor!" << endl;

    serialClose(kobuki);
    return 0;
}

float read_sonar() {
    // you can reuse your code from Lab 2
}

void movement(int sp, int r) {
    // you can reuse your code from Lab 3
}
void read_kobuki_sensors() {
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
