#ifndef _ROUTE_PROCESSOR_HPP_
#define _ROUTE_PROCESSOR_HPP_

#include "common.hpp"
#include <vector>

class Route_processor
// класс, определяющий минимальный путь между входными вершинами
{
    private:
		int _last_number;
		char * _last_letter;
        size_t _left_item_pos;
		bool _flag_is_base = true;
		bool _flag_is_left = false;
		bool _flag_is_right = false;
		bool _flag_is_forward = true;
		bool _flag_is_back = false;
		bool _first_time = true;
		bool _flag_is_last = false;
    public:
		Route_processor(size_t left_item_pos, int last_number, char* last_letter); // -1 - если база располагается за нулевым элементом
        route_vector run(std::vector<char*>& garden_beds); // основная функция, выполняющая построение минимального пути между заданными вершинами
    private:
		void 	Prepare_flags();
		void To_lower_case(char* garden_bed_1);
		int Count_minimal_length(char* garden_bed_1, char* garden_bed_2); //функция определяющая минимальное расстояние между двумя грядками
		void Initialize_matrix(std::vector<char*>& garden_beds, int **matrix);
		route_item * Get_route(char* garden_bed_1, char* garden_bed_2);
};

#endif
