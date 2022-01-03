# Pi-stuff
A C++ class to contain Pi-4 GPIO tools.
I was messing about with a Pi-4B and wanted to do GPIO direct from C++ without having to
include somebody else's library. I was surprised just how little there was on the web as it
all looked reasonably straight forward so I coded up this just to do inputs and outputs.

All the works is in the C++ class in gpio.h in about 100 lines.
The main part was supplying a structure to map the gpiomem into address space of my
program rather than just using arbitrary offsets. The structure members names are based
on the register names used in the CPU datasheet.

It was written using VS2022 on the PC and using the Linux tools provided to talk SSH to
the Pi so I was able to just have the Pi-4 with its nest of wires coming off the GPIO
ports connected to the PC over my WiFi and edit on the PC but compile and test on the
Pi using the usual GCC.
