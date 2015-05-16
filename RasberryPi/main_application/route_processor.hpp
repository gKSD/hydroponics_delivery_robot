#ifndef _ROUTE_PROCESSOR_HPP_
#define _ROUTE_PROCESSOR_HPP_

#include "common.hpp"
#include <vector>

class Route_processor
// класс, определяющий минимальный путь между входными вершинами
{
    private:
        size_t _left_item_pos;

    public:
        Route_processor(size_t left_item_pos); // -1 - если база располагается за нулевым элементом
        route_vector run(std::vector<char*>& garden_beds); // основная функция, выполняющая построение минимального пути между заданными вершинами
};

#endif
