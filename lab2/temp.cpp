// Use g++ -std=c++11 -o Lab2EX2 Lab2EX2.cpp -lwiringPi

#include <softPwm.h>
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

// variables
float distance_previous_error, distance_error;
int adc;
int motor_pin = 26, sonar_pin = 1;
float
    PID_p,
    PID_d, PID_total, PID_i = 0;

float kp = 5, ki = 0.001, kd = 2;

int time_inter_ms = 25;

// functions
void sigroutine(int);
int adcVal();
void PID(float, float, float);
float read_potentionmeter();
float read_sonar();

int main()
{
    wiringPiSetup();

    struct itimerval value, ovalue;
    signal(SIGALRM, sigroutine);
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = time_inter_ms * 1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = time_inter_ms * 1000;
    setitimer(ITIMER_REAL, &value, &ovalue);

    pinMode(motor_pin, PWM_OUTPUT);
    adc = wiringPiI2CSetup(0x48);
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

void PID(float kp, float ki, float kd)
{
    obj_value = read_potentionmeter();
    measured_value = read_sonar();
    distance_error = measured_value - obj_value;
    if (obj_value > 50 && obj_value <= 60)
    {
        kp = 3;
        ki = 0.001;
        kd = 50;
    }
    if (obj_value > 60 && obj_value <= 80)
    {
        kp = 5;
        ki = 0.0011;
        kd = 70;
    }

    PID_p = kp * distance_error;
    PID_i = PID_i + (ki * distance_error * time_inter_ms);
    PID_d = kd * ((distance_error - distance_previous_error) / time_inter_ms);
    PID_total = PID_p + PID_d + PID_i;

    if (PID_total < 0)
        PID_total = 0;
    if (PID_total > 1024)
        PID_total = 1024;
    pwmWrite(motor_pin, int(PID_total));
    distance_previous_error = distance_error;
}

float read_sonar()
{
    // low
    pinMode(sonar_pin, OUTPUT);
    digitalWrite(sonar_pin, LOW);
    usleep(2);
    // high
    digitalWrite(sonar_pin, HIGH);
    usleep(5);
    // low
    digitalWrite(sonar_pin, LOW);

    pinMode(sonar_pin, INPUT);
    usleep(750);
    high_resolution_clock::time_point start = high_resolution_clock::now();
    float dura;
    while (digitalRead(sonar_pin))
    {
        high_resolution_clock::time_point end = high_resolution_clock::now();
        dura = chrono::duration_cast<chrono::microseconds>(end - start).count();
        if (dura >= 18500)
            // if(dura>=6167)
            break;
    }
    float distance = (17 * dura) / 1000;
    usleep(2000);

    return distance;
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
    // int dis = data*100/1742;
    float dis = data * 100 / 1742;
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
