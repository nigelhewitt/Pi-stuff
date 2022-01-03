//=================================================================================================
//
//	gpioTest.cpp		development workshop for GPOIO stuff
//						based in lots of web searches
//						by Nigel V. Hewitt 2022
//
//=================================================================================================

/*
	Written under VS2022 on my PC but run on a Pi-4 over the Wifi

	Turn on SSH 'cos that's how it works just on the Wifi (reboot)

	We need some of these:
	sudo apt install openssh-server g++ gdb gdbserver make ninja-build rsync zip

	We need Linux development with C++ in VS2022
	Somewhere I must have told it to use pi/password
	Project: C++ - Linux - Console -> Console Application
	usual name/location
	Use the usual compile icon, make it a setup project and run it.
	Either put a sleep(10) or a debug trap at the end because the Linux console
	closes immediately when the program ends

	The work is done in /home/pi/projects
*/


#include <cstdio>
#include <unistd.h>
#include "gpio.h"

GPIO *gpio{};		// GPIO worker class

// a rather quick and dirty tool to read buttons
void printButton(int pin)
{
	static bool previous[32]{};

	bool r = gpio->get(pin);
	if(r==previous[pin]) return;
	previous[pin] = r;
	if(r)										// pin is high
		printf("Button %d released\r", pin);
	else										// pin is low
		printf("Button %d pressed\r", pin);
	fflush(stdout);
}

// main - this is where we make most of our planets
int main()
{
    printf("GPIOtest\n");

	// create the GPIO class
	gpio = new GPIO;
	if(!gpio->ok()) return 99;

	// Set GPIO pins 7 to 11 to output mode
	// wired via 1K5 resistors to LEDs to 0V
	for(int g=7; g<=11; ++g)
		gpio->setOutput(g);

	// set GPIO pins 12 and 25 to input mode and turn on the pull-up
	// wired to NO push buttons to 0V
	gpio->setInput(12);
	gpio->pull(12, GPIOMEM::PULL::UP);
	gpio->setInput(24);
	gpio->pull(24, GPIOMEM::PULL::UP);

	for(int rep=0; rep<10; ++rep){
		// loop flashing lights
		for(int g=7; g<=11; ++g){
			gpio->setBit(g);
			usleep(500000);
			printButton(12);
			printButton(24);
		}
		for(int g=7; g<=11; ++g){
			gpio->clearBit(g);
			usleep(500000);
			printButton(12);
			printButton(24);
		}
	}
	delete gpio;
	return 0;
}