#include "route_processor.hpp"
#include <iostream>
Route_processor::Route_processor(size_t left_item_pos)
{
    if (left_item_pos < -1 || left_item_pos > 9)
    {
        std::cout << "Error: invalid item_left_pos for stting base position" << std::endl;
        throw int();
    }
}

route_vector Route_processor::run(std::vector<char*>& garden_beds)
{
    route_vector vec;
    route_item *item = new route_item;
    item->state = 'q'; //test!!!!!!!!!!!!!!
    vec.push_back(item);
    item = new route_item;
    item->state = 'e';
    vec.push_back(item);
    return vec;
}
