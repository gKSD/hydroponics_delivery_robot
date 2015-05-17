#include "route_processor.hpp"
#include <iostream>
Route_processor::Route_processor(size_t left_item_pos)
{
    if (left_item_pos < 0 || left_item_pos > 8)
    {
        std::cout << "Error: invalid item_left_pos for stting base position" << std::endl;
        throw int();
    }
}

route_vector Route_processor::run(std::vector<char*>& garden_beds)
{
    route_vector vec;
    route_item *item = new route_item;
    item->state = '+';
    vec.push_back(item);
    vec.push_back(item);
item = new route_item;
    item->state = '+';
    vec.push_back(item);
item = new route_item;
    item->state = '>';
    vec.push_back(item);
item = new route_item;
item = new route_item;
    item->state = '+';
    vec.push_back(item);
item = new route_item;
    item->state = '*';
    vec.push_back(item);

    return vec;
}
