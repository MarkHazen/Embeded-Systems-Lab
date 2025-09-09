// Use g++ -std=c++11 -o Lab2EX1 Lab2EX1.cpp -lwiringPi

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std;
using namespace std::chrono;

int main()
{
	// Set up wiringPi
	wiringPiSetup();

	// Set the pin number for the sensor trigger
	int triggerPin = 0; // Replace 0 with the actual GPIO pin number

	while (true)
	{
		/*Set the pinMode to output and generate a LOW-HIGH-LOW signal using "digitalWrite" to trigger the sensor.
		Use a 2 us delay between a LOW-HIGH and then a 5 us delay between HIGH-LOW. You can use
		the function "usleep" to set the delay. The unit of usleep is microsecond. */

		// Set the pinMode to output
		pinMode(triggerPin, OUTPUT);

		// Generate a LOW-HIGH-LOW signal
		digitalWrite(triggerPin, LOW);
		usleep(2); // 2 microseconds delay
		digitalWrite(triggerPin, HIGH);
		usleep(5); // 5 microseconds delay
		digitalWrite(triggerPin, LOW);

		/*Echo holdoff delay 750 us*/
		usleep(750);

		/*Switch the pinMode to input*/
		pinMode(triggerPin, INPUT);

		/*Get the time it takes for signal to leave sensor and come back.*/

		// 1. define a varable to get the current time t1. Refer to "High_Resolution_Clock_Reference.pdf" for more information
		auto t1 = high_resolution_clock::now();
		while (digitalRead(sig_pin))
		{ /*read signal pin, the stay is the loop when the signal pin is high*/
			// 2. defind a varable to get the current time t2.
			auto t2 = high_resolution_clock::now();
			// 3. calculate the time duration: t2 - t1
			auto pulse_width = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
			// 4. if the duration is larger than the Pulse Maxium 18.5ms, break the loop.
			if (pulse_width >= MAX_ECHO_TIME)
			{
				break;
			}
		}

		/*Calculate the distance by using the time duration that you just obtained.*/ // Speed of sound is 340m/s
		double distance = (pulse_width / 1000000.0) * 340.0 / 2.0;					  // in meters
		distance = distance * 100.0;												  // convert to cm
		/*Print the distance.*/
		cout << "Distance: " << distance << " cm" << endl;
		/*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
		usleep(250); // 60 ms delay
	}
}
