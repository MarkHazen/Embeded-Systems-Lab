// g++ -std=c++11 -o FinalB2_CPP FinalB2_CPP.cpp -lwiringPi -pthread

#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <chrono>
#include <iostream>
// #include <ratio>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>

using namespace std;
#define PORT 8000
#define IP "127.0.0.1"

int sock = 0;
void movement(int, int);
FILE* file;
char checksum(char* packet, int packet_size);
int createSocket();
int kobuki;

unsigned int bumper;
unsigned int drop;
unsigned int cliff;
unsigned int button;
char cmd = 's';

int speed = 0;
int radius = 0;

void readData();
void processData(string);
int speed(string);   // this function can parse the received buffer and return the speed value
int radius(string);  // this function can parse the received buffer and return the radius value

void read_socket() {
    char buffer[100];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) continue;

        std::string value(buffer);

        // Case 1: single-character D-pad command
        if (value.length() == 1) {
            char cmd = value[0];
            if (cmd == 'u')
                movement(200, 0);  // forward
            else if (cmd == 's')
                movement(-200, 0);  // backward
            else if (cmd == 'a')
                movement(100, 1);  // left turn
            else if (cmd == 'd')
                movement(100, -1);  // right turn
            else if (cmd == 'x')
                movement(0, 0);  // stop
        }

        // Case 2: joystick data (has 'x' and 'y')
        else if (value.find("x") != std::string::npos && value.find("y") != std::string::npos && value.find("z") == std::string::npos) {
            int xpos = 0, ypos = 0;
            try {
                int x1 = value.find("x") + 5;
                int x2 = value.find("'", x1);
                xpos = stoi(value.substr(x1, x2 - x1));

                int y1 = value.find("y") + 5;
                int y2 = value.find("'", y1);
                ypos = stoi(value.substr(y1, y2 - y1));
            } catch (...) {
            }
            printf("Joystick: x=%d y=%d\n", xpos, ypos);
            movement(ypos * 3, xpos);
        }

        // Case 3: phone data (has 'x', 'y', and 'z')
        else if (value.find("x") != std::string::npos && value.find("z") != std::string::npos) {
            int x = 0, y = 0, z = 0;
            try {
                int x1 = value.find("x") + 5;
                int x2 = value.find("'", x1);
                x = stoi(value.substr(x1, x2 - x1));

                int y1 = value.find("y") + 5;
                int y2 = value.find("'", y1);
                y = stoi(value.substr(y1, y2 - y1));

                int z1 = value.find("z") + 5;
                int z2 = value.find("'", z1);
                z = stoi(value.substr(z1, z2 - z1));
            } catch (...) {
            }
            printf("Phone: x=%d y=%d z=%d\n", x, y, z);
            movement(y * 4, x);  // interpret phone tilt
        }
    }
}

int main() {
    setenv("WIRINGPI_GPIOMEM", "1", 1);
    wiringPiSetup();
    kobuki = serialOpen("/dev/kobuki", 115200);
    createSocket();
    char buffer[10];
    // char* p;
    std::thread t(read_socket);
    while (serialDataAvail(kobuki) != -1) {
        // Read the sensor data.
        readData();

        // Construct an string data like 'b0c0d0', you can use "sprintf" function. You can also define your own data protocal.
        char status[10];
        sprintf(status, "b%dc%dd%d", bumper, cliff, drop);

        // Send the sensor data through the socket
        send(sock, status, sizeof(status), 0);

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        // You can refer to the code in previous labs.
        usleep(20000);
    }
    serialClose(kobuki);

    return (0);
}

void movement(int sp, int r) {
    // Create the byte stream packet with the following format:
    unsigned char b_0 = 0xAA;             /*Byte 0: Kobuki Header 0*/
    unsigned char b_1 = 0x55;             /*Byte 1: Kobuki Header 1*/
    unsigned char b_2 = 0x06;             /*Byte 2: Length of Payload*/
    unsigned char b_3 = 0x01;             /*Byte 3: Payload Header*/
    unsigned char b_4 = 0x04;             /*Byte 4: Payload Data: Length*/
    unsigned char b_5 = sp & 0xff;        /*Byte 5: Payload Data: Speed(mm/s)*/
    unsigned char b_6 = (sp >> 8) & 0xff; /*Byte 6: Payload Data: Speed(mm/s)*/
    unsigned char b_7 = r & 0xff;         /*Byte 7: Payload Data: Radius(mm)*/
    unsigned char b_8 = (r >> 8) & 0xff;  /*Byte 8: Payload Data: Radius(mm)*/
    unsigned char checksum = 0;           /*Byte 9: Checksum*/
    // file = fopen("/dev/kobuki","w");
    char packet[] = {b_0, b_1, b_2, b_3, b_4, b_5, b_6, b_7, b_8};
    for (unsigned int i = 2; i < 9; i++)
        checksum ^= packet[i];
    serialPutchar(kobuki, b_0);
    serialPutchar(kobuki, b_1);
    serialPutchar(kobuki, b_2);
    serialPutchar(kobuki, b_3);
    serialPutchar(kobuki, b_4);
    serialPutchar(kobuki, b_5);
    serialPutchar(kobuki, b_6);
    serialPutchar(kobuki, b_7);
    serialPutchar(kobuki, b_8);
    serialPutchar(kobuki, checksum);
    delay(30);
}

int createSocket() {
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    /*Use the IP address of the server you are connecting to*/
    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return 0;
}

void readData() {
    unsigned int read;
    while (true) {
        // If the bytes are a 1 followed by 15, then we are
        // parsing the basic sensor data packet
        read = serialGetchar(kobuki);
        if (read == 1) {
            if (serialGetchar(kobuki) == 15) break;
        }
    }

    // Read past the timestamp
    serialGetchar(kobuki);
    serialGetchar(kobuki);
    /*Read the bytes containing the bumper, wheel drop,
            and cliff sensors.*/
    bumper = serialGetchar(kobuki);
    drop = serialGetchar(kobuki);
    cliff = serialGetchar(kobuki);

    /*Read through the bytes between the cliff sensors and
    the button sensors.*/
    serialGetchar(kobuki);
    serialGetchar(kobuki);
    serialGetchar(kobuki);
    serialGetchar(kobuki);
    serialGetchar(kobuki);
    serialGetchar(kobuki);
    /*Read the byte containing the button data.*/
    button = serialGetchar(kobuki);

    if (button == 2) {
        cout << "button B1 pushed" << endl;
        serialClose(kobuki);
        // break;
    }

    /*Pause the script so the data read receive rate is
    the same as the Kobuki send rate.*/
    usleep(20000);
}

void processData(string value) {
    printf(value);
}

int speed(string value) {
    int ind = value.find('x', 0) + 5;
    int ind2 = value.find("\'", ind);
    string index = value.substr(ind, ind2 - ind);
    printf("%s\n", index);
    ind = -stoi(index);
    // cout<<ind<<endl;
    if (ind > 50) ind = 50;
    if (ind < -50) ind = -50;
    if (ind > -20 && ind < 5) ind = 0;

    return 10 * ind;
}

int radius(string value) {
    int ind0 = value.find('z', 0) + 5;
    int ind = value.find("\',", ind0);
    string index = value.substr(ind0, (ind - ind0));

    ind = stoi(index);
    // cout<<ind<<endl;
    if (ind >= 0 && ind <= 20) ind = 0;
    if (ind >= 340 && ind <= 360) ind = 0;
    if (ind >= 0 && ind <= 180) {
        ind = ind * 2;
    } else {
        ind = (ind - 360) * 2;
    }
    return ind;
}