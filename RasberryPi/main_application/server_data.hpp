#ifndef _SERVER_DATA_HPP_
#define _SERVER_DATA_HPP_

#include <stdio.h>
#include <string.h>

typedef struct task
{
    char _address[3];

    task()
    {
        memset(_address, 0, sizeof(_address));
    }
    void reset_task ()
    {
        memset(_address, 0, sizeof(_address));
    }
    void set_address (char *address)
    {
        strcpy (_address, address);
    } 
} task;

#endif
