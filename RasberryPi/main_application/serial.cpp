#include "serial.hpp"

#include <iostream>
#include <string.h>
#include "wiringSerial.h"
#include "wiringPi.h"
#include "error_codes.hpp"

//wiringPi help => https://projects.drogon.net/raspberry-pi/wiringpi/serial-library/

int Serial::uart_setup()
{
/*    tcgetattr (fd, &_options) ;   // Read current options
    _options.c_cflag &= ~CSIZE ;  // Mask out size
    _options.c_cflag |= CS7 ;     // Or in 7-bits
    _options.c_cflag |= PARENB ;  // Enable Parity - even by default
    tcsetattr (fd, &_options) ;   // Set new options
*/  
    if (wiringPiSetup () == -1)
    {
        std::cout << "Unable to start wiringPi"<< std::endl;
        return -1 ;
    }

}

void Serial::serial_open()
{
    if (_device == NULL || _baud == -1)
    {
        throw SERIAL_PARAMS_NOT_SET;
    }

    _fd = serialOpen(_device, _baud);
    if(_fd == -1)
    {
        throw SERIAL_OPEN_ERROR;
    }
}
void Serial::serial_open(const char *device, int baud)
{
    set_device(device);
    _baud = baud;
    serial_open();
}

void Serial::set_device(const char *device)
{
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
    if (_fd >= 0)
        serialClose(_fd);
}

void Serial::serial_putchar(unsigned char c)
//Sends the single byte to the serial device identified by the given file descriptor.
{
    if (_fd >= 0)
        serialPutchar (_fd, c);
}

void Serial::serial_puts (char *s)
{
    if (_fd >= 0)
        serialPuts (_fd, s) ;
}

// Possible:  void  serialPrintf (int fd, char *message, …) ;
// Emulates the system printf function to the serial device.

int Serial::serial_data_available()
// Returns the number of characters available for reading, or -1 for any error condition, in which case errno will be set appropriately.
{
    if (_fd >= 0)
        return serialDataAvail (_fd) ;
    return -1;
}

int Serial::serial_getchar()
// Returns the next character available on the serial device. This call will block for up to 10 seconds if no data is available (when it will return -1)
{
    if (_fd >= 0)
        return serialGetchar (_fd) ;
    return-1;
}

void Serial::serial_flush()
// This discards all data received, or waiting to be send down the given device.
{
    if (_fd >= 0)
        serialFlush (_fd);
}

Serial::~Serial()
{
    //if (_device != NULL ) delete [] _device;
}
