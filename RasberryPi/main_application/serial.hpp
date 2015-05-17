#ifndef _SERIAL_HPP_
#define _SERIAL_HPP_

//#include <termios.h>
#include <cstddef>
class Serial
{
    private:
        //struct termious _options;
        int _fd; // хранит указатель на файловый дексриптор открытого соединения
        char *_device;
        int _baud;

    public:
        Serial(): _fd(-1), _device(NULL) {}
        void serial_open(const char *device, int baud);
        void serial_open();
        int uart_setup();
        void set_device(const char *device);
        void serial_close();
        void serial_putchar(unsigned char c);
        void serial_puts(char *s);
        int serial_data_available();
        int serial_getchar();
        void serial_flush();
        ~Serial();
};

#endif
