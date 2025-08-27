// The stator in the Stepper Motor we have supplied has 32 magnetic poles. Therefore, to complete
//  one full revolution requires 32 full steps. The rotor (or output shaft) of the Stepper
// Motor is connected to a speed reduction set of gears and the reduction ratio is 1:64. Therefore,
// the final output shaft (exiting the Stepper Motor’s housing) requi res 32 X 64 = 2048
// steps to make one full revolution.

// g++ -std=c++11 -o Lab1EX3 Lab1EX3.cpp -lwiringPi
#include <stdio.h>
#include <wiringPi.h>
#include <iostream>

using namespace std;

#define shortest_time_period_ms 3

int IN1 = 1;
int IN2 = 4;
int IN3 = 5;
int IN4 = 6;

void moveOnePeriod(int dir) {
    if (dir == 1) {
        /* clockwise, there are four steps in one period, set a delay after each step*/
        // Step A
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        delay(shortest_time_period_ms);
        // Step B
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        delay(shortest_time_period_ms);
        // Step C
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        delay(shortest_time_period_ms);
        // Step D
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        delay(shortest_time_period_ms);
    } else {
        /* anticlockwise, there are four steps in one period, set a delay after each step*/

        // Step D
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        delay(shortest_time_period_ms);
        // Step C
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        delay(shortest_time_period_ms);
        // Step B
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        delay(shortest_time_period_ms);
        // Step A
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        delay(shortest_time_period_ms);
    }
}
// continuous rotation function, the parameter steps specifies the rotation cycles, every four steps is a cycle
void moveCycles(int dir, int cycles) {
    for (int i = 0; i < cycles; i++) {
        moveOnePeriod(dir);
    }
}

int main(void) {
    cout << "1" << std::endl;
    
    wiringPiSetup();
    /* set the pin mode*/

    cout << "2" << std::endl;

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    cout << "3" << std::endl;

    while (1) {
        /*rotating 360° clockwise, a total of 2048 steps in one full revolution, namely, 512 cycles.
        use function moveCycles(int dir,int cycles)*/
        cout << "Moving Clockwise" << std::endl;
        moveCycles(1, 512);
        delay(500);

        /*rotating 360° anticlockwise, a total of 2048 steps in one full revolution, namely, 512 cycles.
        use function moveCycles(int dir,int cycles)*/
        cout << "Moving Counter Clockwise" << std::endl;
        moveCycles(0, 512);
        delay(500);
    }
    return 0;
}
