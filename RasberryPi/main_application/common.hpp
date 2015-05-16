#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <vector>
#include <cstddef>

//элементы, определяющие действия на перекрестках
static const char r_back = 'b';
static const char r_forward = 'f';
static const char r_crossroad = '+';
static const char r_left = '<';
static const char r_right = '>';
static const char r_turn_around = '&';
static const char r_deadlock = 'd';


static const char r_final = '*'; //признак участка с грядкой

typedef struct route_item
{
    char state; // определяет: 1. проехать перекресток 2. повернуть направо 3. повернуть налево 4. признак участка с грядкой
    route_item *next;
    route_item *prev;
    route_item()
    {
        next = NULL;
        prev = NULL;
        state = '\0';
    }
    void reset ()
    {
        next = NULL;
        prev = NULL;
        state = '\0';

    }
} route_item;

typedef std::vector <route_item *> route_vector; 

#endif
