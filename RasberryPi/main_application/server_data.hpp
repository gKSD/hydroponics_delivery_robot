#ifndef _SERVER_DATA_HPP_
#define _SERVER_DATA_HPP_

#include <stdio.h>
#include <string.h>

typedef struct task
{
    char _address[3];
    int _ml;

    task()
    {
        memset(_address, 0, sizeof(_address));
        _ml = -1;
    }
    void reset_task ()
    {
        memset(_address, 0, sizeof(_address));
        _ml = -1;
    }
    void set_address (char *address)
    {
        strcpy (_address, address);
    }
    void set_ml (int ml)
    {
        _ml = ml;
    }
} task;

#endif
