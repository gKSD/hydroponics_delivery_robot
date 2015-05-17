#include "serial.hpp"

#include <iostream>
#include <string.h>
#include "error_codes.hpp"

//wiringPi help => https://projects.drogon.net/raspberry-pi/wiringpi/serial-library/

int Serial::uart_setup()
{
    return 1;
}

void Serial::serial_open()
{
    if (_device == NULL || _baud == -1)
    {
        throw SERIAL_PARAMS_NOT_SET;
    }

    _fd = 15;
    if(_fd == -1)
    {
        throw SERIAL_OPEN_ERROR;
    }
}
void Serial::serial_open(const char *device, int baud)
{
    std::cout << ">>> Serial open" << std::endl;
    set_device(device);
    _baud = baud;
    serial_open();
}

void Serial::set_device(const char *device)
{
    std::cout << ">>> Serial set device" << std::endl;
    if (_device != NULL)
        delete [] _device;
    int dev_len = strlen(device);
    _device = new char [dev_len];
    strcpy(_device, device);
    std::cout<<"new device: " << _device << std::endl;
}

//****

void Serial::serial_close()
//Closes the device identified by the file descriptor given.
{
    std::cout << ">>> Serial close" << std::endl;
}

void Serial::serial_putchar(unsigned char c)
//Sends the single byte to the serial device identified by the given file descriptor.
{
    std::cout << ">>> Serial putchar: "<< c << std::endl;
}

void Serial::serial_puts (char *s)
{
    std::cout << ">>> Serial puts: " << s << std::endl;
}

// Possible:  void  serialPrintf (int fd, char *message, …) ;
// Emulates the system printf function to the serial device.

int Serial::serial_data_available()
// Returns the number of characters available for reading, or -1 for any error condition, in which case errno will be set appropriately.
{
    return 3;
}

char Serial::serial_getchar()
// Returns the next character available on the serial device. This call will block for up to 10 seconds if no data is available (when it will return -1)
{
    char *c = new char;
    std::cout << ">>> Serial getchar, enter char: ";
    std::cin >> c;
    return *c;
}

void Serial::serial_flush()
// This discards all data received, or waiting to be send down the given device.
{
}

Serial::~Serial()
{
    //if (_device != NULL ) delete [] _device;
}
