// Use g++ -std=c++11 -o Lab3EX2 Lab3EX2.cpp -lwiringPi

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <chrono>
#include <cmath>

using namespace std::chrono;
using namespace std;

// functions
void sigroutine(int);
int adcVal();
void PID(float, float, float);
float read_potentionmeter();
float read_sonar();

// variables
float distance_previous_error, distance_error;
float obj_value = 0.0f;      // potentionmeter reading
float measured_value = 0.0f; // sonar reading
int adc;
float PID_p, PID_d, PID_total, PID_i = 0;
int time_inter_ms = 25; // time interval, you can use different time interval
const int MAX_ECHO_TIME = 18500;

/*set your pin numbers and pid values*/
int motor_pin = 26;
int sonar_pin = 1;
float kp = 5;
float ki = 0.001;
float kd = 2;

int pulse_width = 0;

int main()
{
    wiringPiSetup();
    adc = wiringPiI2CSetup(0x48);

    /*Set the pinMode (fan pin)*/
    pinMode(motor_pin, PWM_OUTPUT);

    // This part is to set a system timer, the function "sigroutine" will be triggered
    // every time_inter_ms milliseconds.
    struct itimerval value, ovalue;
    signal(SIGALRM, sigroutine);
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = time_inter_ms * 1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = time_inter_ms * 1000;
    setitimer(ITIMER_REAL, &value, &ovalue);

    while (true)
    {
        cout << "obj_value: " << obj_value << " measured_value: " << measured_value << endl;
        cout << "PID_p: " << PID_p << endl;
        cout << "PID_i: " << PID_i << endl;
        cout << "PID_d: " << PID_d << endl;
        cout << "PID_total: " << PID_total << endl;
        delay(20);
    }
}

void sigroutine(int signo)
{
    PID(kp, ki, kd);
    return;
}

/* based on the obj distance and measured distance, implement a PID control algorithm to
the speed of the fan so that the Ping-Pang ball can stay in the obj position*/
void PID(float kp, float ki, float kd)
{
    /*read the objective position/distance of the ball*/
    obj_value = read_potentionmeter();
    /*read the measured position/distance of the ball*/
    measured_value = read_sonar();
    /*calculate the distance error between the obj and measured distance */
    distance_error = obj_value - measured_value;
    /*calculate the proportional, integral and derivative output */
    PID_p = kp * distance_error;
    PID_i = PID_i + (ki * distance_error * time_inter_ms);
    PID_d = kd * ((distance_error - distance_previous_error) / time_inter_ms);
    PID_total = PID_p + PID_d + PID_i;

    /*assign distance_error to distance_previous_error*/
    distance_previous_error = distance_error;

    /*use PID_total to control your fan*/
    pwmWrite(motor_pin, int(PID_total));
}

/* use a sonar sensor to measure the position of the Ping-Pang ball. you may reuse
your code in EX1.*/
float read_sonar() {
    /*Set the pinMode to output and generate a LOW-HIGH-LOW signal using "digitalWrite" to trigger the sensor.
    Use a 2 us delay between a LOW-HIGH and then a 5 us delay between HIGH-LOW. You can use
    the function "usleep" to set the delay. The unit of usleep is microsecond. */

    // Set the pinMode to output
    pinMode(sonar_pin, OUTPUT);

    // Generate a LOW-HIGH-LOW signal
    digitalWrite(sonar_pin, LOW);
    usleep(2); // 2 microseconds delay
    digitalWrite(sonar_pin, HIGH);
    usleep(5); // 5 microseconds delay
    digitalWrite(sonar_pin, LOW);

    /*Echo holdoff delay 750 us*/
    usleep(750);

    /*Switch the pinMode to input*/
    pinMode(sonar_pin, INPUT);

    /*Get the time it takes for signal to leave sensor and come back.*/

    // 1. define a varable to get the current time t1. Refer to "High_Resolution_Clock_Reference.pdf" for more information
    auto t1 = high_resolution_clock::now();
    while (digitalRead(sonar_pin))
    { /*read signal pin, the stay is the loop when the signal pin is high*/
        // 2. defind a varable to get the current time t2.
        auto t2 = high_resolution_clock::now();
        // 3. calculate the time duration: t2 - t1
        pulse_width = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
        // 4. if the duration is larger than the Pulse Maxium 18.5ms, break the loop.
        if (pulse_width >= MAX_ECHO_TIME)
        {
            break;
        }
    }

    /*Calculate the distance by using the time duration that you just obtained.*/ // Speed of sound is 340m/s
    double distance = (pulse_width * 340.0 / 1000000)/ 2.0;					  // in meters
    distance = distance * 100.0;												  // convert to cm
    /*Print the distance.*/
    cout << "Distance: " << distance << " cm" << endl;
    return 0;
    /*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
    usleep(60000); // 60 ms delay
}

float read_potentionmeter()
{
    float data;
    float sum = 0;
    int buff = 1;
    for (int i = 0; i < buff; i++)
    {
        data = adcVal();
        sum += data;
    }
    
    data = sum / buff;
    if (data > 1800)
        data = 0;
    
    double v;
    v = (data / 2047.0) * 6.144; // fs=6.144
    float dis = data * 100 / 1742;
    
    //limits
    if (dis < 5)
        dis = 5;
    if (dis > 90)
        dis = 90;

    return dis;
}

int adcVal()
{
    uint16_t low, high, value;
    wiringPiI2CWriteReg16(adc, 0x01, 0xc5c1);
    usleep(1000);
    uint16_t data = wiringPiI2CReadReg16(adc, 0x00);
    low = (data & 0xFF00) >> 8;
    high = (data & 0x00FF) << 8;
    value = (high | low) >> 4;
    return value;
}
