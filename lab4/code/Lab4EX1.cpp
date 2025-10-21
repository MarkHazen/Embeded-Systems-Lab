// Use g++ -std=c++11 -o Lab4EX1 Lab4EX1.cpp -lwiringPi

#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

int kobuki;

int main() {
    wiringPiSetup();
    kobuki = serialOpen("/dev/kobuki", 115200);
    unsigned int bumper;
    unsigned int drop;
    unsigned int cliff;
    unsigned int button;
    unsigned int read;

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

        // Pause the script so the data read receive rate is the same as the Kobuki send rate.
        usleep(20000);
    }

    return (0);
}