// g++ -std=c++11 -o Lab1EX4 Lab1EX4.cpp -lwiringPi

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

#define SERVO_MIN_MS 5
#define SERVO_MAX_MS 25
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

/* signal pin of the servo*/
#define servoPin    

//Specific a certain rotation angle (0-180) for the servo
void servoWrite(int pin, int angle){ 
    long time = 0;
    time = /* map the desired angle to time*/
    softPwmWrite(pin,time);   
}

int main(void)
{
    wiringPiSetup();    
    softPwmCreate(servoPin,  0, 200);

    while(1){

        /*make servo rotate from minimum angle to maximum, use the function 
        servoWrite(int pin, int angle), increase 1 degree each time*/
        delay(500);

        /*make servo rotate from maximum angle to minimum, use the function 
        servoWrite(int pin, int angle), increase 1 degree each time*/
        delay(500);
    }
    return 0;
}

