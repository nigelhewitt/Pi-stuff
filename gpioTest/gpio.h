//=================================================================================================
//
// gpio.h		GPIO class for a Pi-4B
//				by Nigel V. Hewitt 2022
//				based on the Pi4's CPU data sheet
//
//				the user needs to be a member of the gpio group
//
//=================================================================================================
#pragma once

#include <cstdio>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// gpiomem buffer
typedef __uint32_t DWORD;

struct alignas(4) GPIOMEM {
public:
	enum SEL { IN=0, OUT=1, ALT5=2, ALT4=3, ALT0=4, ALT1=5, ALT2=6, ALT3=7 };
	DWORD GPFSEL[6], dummy1;				// 0x00 function select  - 10 sets of 3 bits per register as above (GPIO0 to GPIO57)
	DWORD GPSET[2],  dummy2;				// 0x1c pin output set   - 32 bits per register
	DWORD GPCLR[2],  dummy3;				// 0x28 pin output clear - 32 buts per register
	DWORD GPLEV[2],  dummy4;				// 0x34 pin level		 - 32 bits per register
	DWORD GPEDS[2],  dummy5;				// 0x40 pin event detect status
	DWORD GPREN[2],  dummy6;				// 0x4c pin rising edge detect enable
	DWORD GPFEN[2],  dummy7;				// 0x58 pin falling edge detect enable
	DWORD GPHEN[2],  dummy8;				// 0x64 pin high detect enable
	DWORD GPLEN[2],  dummy9;				// 0x70 pin low detect enable
	DWORD GPAREN[2], dummy10;				// 0x7c pin async rising edge detect
	DWORD GPAFEN[2];						// 0x88 pin async falling edge detect
	DWORD dummy11[21];						// pad from 0x90 to 0xe0 inclusive
	enum PULL { NONE=0, UP=1, DOWN=2 };
	DWORD GPIO_PUD_REG[4];					// 0xe4 pull up/pull down register - 16 sets of 2 bits as above
};

class GPIO {
public:
	GPIO(){
		// open /dev/gpiomem
		int mem_fd;
		if((mem_fd = open("/dev/gpiomem", O_RDWR|O_SYNC) ) < 0){
			printf("can't open /dev/gpiomem \n");
			return;
		}

		// mmap GPIO
		void* gpio_map = mmap(
			nullptr,				// Any address in our space will do
			BLOCK_SIZE,				// Map length
			PROT_READ|PROT_WRITE,	// Enable reading & writing to mapped memory
			MAP_SHARED,				// Shared with other processes
			mem_fd,					// File to map
			GPIO_BASE				// Offset to GPIO peripheral
		);

		close(mem_fd);				// No need to keep mem_fd open after mmap

		if(gpio_map == MAP_FAILED){
			printf("mmap error %d\n", (int)gpio_map);	// errno also set!
			return;
		}

		gpio = (volatile GPIOMEM*)gpio_map;
	}
	~GPIO(){
		munmap((void*)gpio, BLOCK_SIZE);
	}
	bool ok(){ return gpio!=nullptr; }

	void setSel(int pin, GPIOMEM::SEL a){				// set the GPFSEL register mode
		gpio->GPFSEL[pin/10] &= ~(7<<((pin%10)*3));
		if(a)
			gpio->GPFSEL[pin/10] |= (a<<((pin%10)*3));
	}
	void setInput(int pin){								// set input mode
		setSel(pin, GPIOMEM::IN);
	}
	void setOutput(int pin){							// set output mode
		setSel(pin, GPIOMEM::OUT);
	}
	void setBit(int pin){								// set a pin
		gpio->GPSET[pin/32] = (1<<(pin%32));
	}
	void clearBit(int pin){								// clear a pin
		gpio->GPCLR[pin/32] = (1<<(pin%32));
	}
	bool get(int pin){									// return a pin state
		return (gpio->GPLEV[pin/32] & (1<<(pin%32)))!=0;
	}
	void pull(int pin, GPIOMEM::PULL pull){				// set pull up/down mode
		gpio->GPIO_PUD_REG[pin/16] &= ~(3<<(pin%16)*2);
		if(pull)
			gpio->GPIO_PUD_REG[pin/16] |= (pull<<((pin%16)*2));
	}

private:
	volatile GPIOMEM* gpio{};
	// constants
	int    BCM2708_PERI_BASE{0x20000000};
	off_t  GPIO_BASE{BCM2708_PERI_BASE + 0x200000};	// GPIO controller
	size_t BLOCK_SIZE{4*1024};
};
