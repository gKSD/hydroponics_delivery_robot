#include "stdafx.h"
#include "route_processor.hpp"
#include <iostream>
#include <math.h>
#include "travelling_salesman.hpp"


Route_processor::Route_processor(size_t left_item_pos, int last_number, char* last_letter)
{
	
    if (left_item_pos < 0 || left_item_pos > 8)
    {
        std::cout << "Error: invalid item_left_pos for stting base position" << std::endl;
        throw int();
    }
	_last_number = last_number;
	_last_letter = last_letter;
	_left_item_pos = left_item_pos;
}

route_vector Route_processor::run(std::vector<char*>& garden_beds)
{

    route_vector vec;
	//char ** mas =  garden_beds.data();
	char str[3];
	str[0] = 'a';
	str[1] = _left_item_pos + '0';
	str[2] = '\0';
	garden_beds.push_back(str);
	int n = garden_beds.size();//Длина матрицы для задачи комивояжера.
	int **matrix;//первый элемент матрицы всегда база('a' + left_item_pos)
	matrix = new int*[n+1];
	for (int i = 1; i <= n; i++) 
	{
		matrix[i] = new int[n+1];
	}
	Initialize_matrix(garden_beds,matrix);

	Travelling_salesman *tr2 = new Travelling_salesman(n, matrix);
	tr2->Run();
	Prepare_flags();
	int *route = tr2->Output();
	for (int i = 0; i < n; i++) {
	    if( i == (n - 1)) {
		    _flag_is_last = true;
		}
		vec.push_back(Get_route(garden_beds[n - route[i]], garden_beds[n - route[i + 1]]));
		
	}
   

/*	char a[3] = "c4";
	char b[3] = "a1";
	Get_route(a, b);*/
    return vec;
}


void Route_processor::Prepare_flags() 
{
	_flag_is_right = false;
	_flag_is_left = false;
	_flag_is_base = true;
	_flag_is_forward = true;
	_flag_is_back = false;
	_flag_is_last = false;
}

void Route_processor::To_lower_case(char* garden_bed) 
{
	char *cursor = garden_bed;
	while (cursor && *cursor) {
		*cursor = tolower(*cursor);
		cursor++;
	}
}

route_item * Route_processor::Get_route(char* garden_bed_1, char* garden_bed_2)

{
	std::cout << " route " << garden_bed_1 << "   to  " << garden_bed_2 << std::endl;
//	To_lower_case(garden_bed_1);
//	To_lower_case(garden_bed_2);
	char *current_position = garden_bed_1;
	route_item *previous_item = NULL;
	route_item *item;
	route_item *first_item = NULL;
	if (_flag_is_last) 
	{
		_flag_is_last = false;
		if (strcmp(current_position, garden_bed_2) == 0) {
			if (_flag_is_left) {
				_flag_is_right = true;
				_flag_is_left = false;
				item = new route_item();
				item->state = r_turn_around;
				first_item = item;
				previous_item = item;
			}

			item = new route_item();
			item->state = r_right;
			if (previous_item != NULL) 
			{
				previous_item->next = item;
				item->prev = previous_item;
			}
			else
			{
				first_item = item;
			}
			previous_item = item;
			item = new route_item();
			item->state = r_base;
			item->prev = previous_item;
			previous_item->next = item;
		}
		else {
			if (current_position[1] < garden_bed_2[1]) //развернулись если надо
			{
				if (_flag_is_left) {
					_flag_is_right = true;
					_flag_is_left = false;
					item = new route_item();
					item->state = r_turn_around;
					first_item = item;
					previous_item = item;
				}
				//едем до нужного поворота
				while (current_position[1] != garden_bed_2[1]) {
					current_position[1]++;

					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
				}
				item = new route_item();//Доехали до финальног поворота остается поворот и финиш
				item->state = r_right;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
				//едем вниз до конца
				while (current_position[0] != garden_bed_2[0]) {
					current_position[0]--;

					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
				}
				item = new route_item();//Доехали до финального поворота остается  финиш на базу
				item->state = r_base;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
			}
			else if (current_position[1] > garden_bed_2[1]) //развернулись, если надо
			{
				if (_flag_is_right) {
					_flag_is_right = false;
					_flag_is_left = true;
					item = new route_item();
					item->state = r_turn_around;
					first_item = item;
					previous_item = item;
				}
				while (current_position[1] != garden_bed_2[1]) {
					current_position[1]--;

					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
				}
				item = new route_item();//Доехали до финальног поворота остается поворот и финиш
				item->state = r_left;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
				//едем вниз до конца
				while (current_position[0] != garden_bed_2[0]) {
					current_position[0]--;

					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
				}
				item = new route_item();//Доехали до финального поворота остается  финиш на базу
				item->state = r_base;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
			}
		}

	} 
     else if (_flag_is_base)
	{	
		_flag_is_base = false;
		if (strcmp(current_position,garden_bed_2) == 0) {
			_flag_is_left = true;
			first_item = new route_item();
			first_item->state = r_left;
			item = new route_item();
			item->state = r_final;
			item->prev = first_item;
			first_item->next = item;
		}
		else
		{
			while (current_position[0] != garden_bed_2[0])
			{
				current_position[0]++;

				item = new route_item();
				item->state = r_crossroad;

				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				else
				{
					first_item = item;
				}
				previous_item = item;
			}

			while (current_position[1] != garden_bed_2[1]) {
				if (current_position[1] - garden_bed_2[1] == 1) {
					item = new route_item();
					if (_flag_is_left)
						item->state = r_crossroad;
					else
					{
						item->state = r_left;
						_flag_is_left = true;
						_flag_is_right = false;
					}
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					item = new route_item();
					item->state = r_final;
					previous_item->next = item;
					item->prev = previous_item;
					previous_item = item;
					current_position[1]--;
				}
				else if (current_position[1] - garden_bed_2[1] == -1) {
					item = new route_item();
					if (_flag_is_right)
						item->state = r_crossroad;
					else
					{
						item->state = r_right;
						_flag_is_left = false;
						_flag_is_right = true;
					}
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					item = new route_item();
					item->state = r_final;
					previous_item->next = item;
					item->prev = previous_item;
					previous_item = item;
					current_position[1]++;
				}
				else if (current_position[1] < garden_bed_2[1])
				{
					item = new route_item();
					if (_flag_is_right)
						item->state = r_crossroad;
					else
					{
						item->state = r_right;
						_flag_is_left = false;
						_flag_is_right = true;
					}
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					current_position[1]++;
				}
				else
				{
					item = new route_item();
					if (_flag_is_left)
						item->state = r_crossroad;
					else
					{
						item->state = r_left;
						_flag_is_left = true;
						_flag_is_right = false;
					}
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					current_position[1]--;
				}
			}
		}
    }
	else {
		if (current_position[1] - garden_bed_2[1] > 0) {//Цель находится слева - в ту сторону и начинаем двигаться.
			if (_flag_is_right) {
				_flag_is_right = false;
				_flag_is_left = true;
				item = new route_item();
				item->state = r_turn_around;
				first_item = item;
				previous_item = item;
			}
			current_position[1]--;
			while (current_position[1] != garden_bed_2[1] ) //едем пока не доедем до поворота
			{
				item = new route_item();
				item->state = r_crossroad;
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				else 
				{
					first_item = item;
				}
				previous_item = item;
				current_position[1]--;
			}
			if (current_position[0] < garden_bed_2[0]) //мы находимся ниже цели, надо ехать вверх
			{
				item = new route_item();
				item->state = r_right;
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				previous_item = item;
				while (current_position[0] != garden_bed_2[0] - 1) //едем пока не доедем до поворота
				{
					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					current_position[0]++;
				}
				item = new route_item();//Доехали до финальног поворота остается поворот и финиш
				item->state = r_left;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
				item = new route_item();
				item->state = r_final;
				previous_item->next = item;
				item->prev = previous_item;
			}
			else if (current_position[0] > garden_bed_2[0]) //мы находимся выше, надо ехать вниз
			{
				item = new route_item();
				item->state = r_left;
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				previous_item = item;
				while (current_position[0] != garden_bed_2[0] + 1) //едем пока не доедем до поворота
				{
					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					current_position[0]--;
				}
				item = new route_item();//Доехали до финального поворота остается поворот и финишь
				item->state = r_right;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
				item = new route_item();
				item->state = r_final;
				previous_item->next = item;
				item->prev = previous_item;
			}
		}
		else if (current_position[1] - garden_bed_2[1] < 0) {//Цель находится справа, начинаем двигаться в ту сторону
			if (_flag_is_left) {
				_flag_is_right = true;
				_flag_is_left = false;
				item = new route_item();
				item->state = r_turn_around;
				first_item = item;
				previous_item = item;
			}
			current_position[1]++;
			while (current_position[1] != garden_bed_2[1]) //едем пока не доедем до поворота
			{
				item = new route_item();
				item->state = r_crossroad;
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				else
				{
					first_item = item;
				}
				previous_item = item;
				current_position[1]++;
			}
			if (current_position[0] < garden_bed_2[0]) //мы находимся ниже цели, надо ехать вверх
			{
				item = new route_item();
				item->state = r_left;
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				previous_item = item;
				while (current_position[0] != garden_bed_2[0] - 1) //едем пока не доедем до поворота
				{
					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					current_position[0]++;
				}
				item = new route_item();//Доехали до финального поворота остается поворот и финиш
				item->state = r_right;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
				item = new route_item();
				item->state = r_final;
				previous_item->next = item;
				item->prev = previous_item;
			}
			else if (current_position[0] > garden_bed_2[0]) //мы находимся выше, надо ехать вниз
			{
				item = new route_item();
				item->state = r_right;
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				previous_item = item;
				while (current_position[0] != garden_bed_2[0] + 1) //едем пока не доедем до поворота
				{
					item = new route_item();
					item->state = r_crossroad;
					if (previous_item != NULL) {
						previous_item->next = item;
						item->prev = previous_item;
					}
					else
					{
						first_item = item;
					}
					previous_item = item;
					current_position[0]--;
				}
				item = new route_item();//Доехали до финального поворота остается поворот и финишь
				item->state = r_left;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
				item = new route_item();
				item->state = r_final;
				previous_item->next = item;
				item->prev = previous_item;
			}
		}
		else if (current_position[1] - garden_bed_2[1] == 0) {//если необходимо сделать прямоугольный поврот 
			bool flag_went_from_right = false;
			bool flag_went_from_left = false;//Флаги нужны что бы знать куда возвращаться при прямоуголном развороте
			if ( current_position[1] == '0' && _flag_is_left) {//если мы уперлись в левый угол, то разворот
				_flag_is_right = true;
				_flag_is_left = false;
				item = new route_item();
				item->state = r_turn_around;
				first_item = item;
				previous_item = item;
				//current_position[0] < *_last_letter) {
			}
			else if ( current_position[1] == _last_number && _flag_is_right){//если мы уперлись в правый угол, то ращворот. 
				_flag_is_right = false;
				_flag_is_left = true;
				item = new route_item();
				item->state = r_turn_around;
				first_item = item;
				previous_item = item;
			}
			if (current_position[0] < garden_bed_2[0]) {//Если надо ехать вверх
				item = new route_item();
				if (_flag_is_left){//если машина смотрит в влево то поворачиваем вправо
					item->state = r_right;
					flag_went_from_left = true;
					_flag_is_forward = true;
					_flag_is_left = false;
					_flag_is_right = false;
					_flag_is_back = false;
				}
				else//иначе машина смотрит вправо => поворачиваем налево
				{
					item->state = r_left;
					flag_went_from_right = true;
					_flag_is_left = false;
					_flag_is_right = false;
					_flag_is_forward = true;
					_flag_is_back = false;
				}
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				else {
					first_item = item;
				}
				previous_item = item;
				current_position[0]++;
			}
			else if (current_position[0] > garden_bed_2[0]) { //если надо ехать вниз
				item = new route_item();
				if (_flag_is_left){//если машина смотрит в влево то поворачиваем влево
					flag_went_from_left = true;
					item->state = r_right;
					_flag_is_forward = false;
					_flag_is_left = false;
					_flag_is_right = false;
					_flag_is_back = true;
				}
				else//иначе машина смотрит вправо => поворачиваем направо
				{
					flag_went_from_right = true;
					item->state = r_right;
					_flag_is_left = false;
					_flag_is_right = false;
					_flag_is_forward = false;
					_flag_is_back = true;
				}
				if (previous_item != NULL) {
					previous_item->next = item;
					item->prev = previous_item;
				}
				else {
					first_item = item;
				}
				previous_item = item;
				current_position[0]--;
			}
			while (current_position[0] != garden_bed_2[0]) {//мы повернули, поднимаемся вверх или вниз, пока не доедем до поворота
				item = new route_item();
				item->state = r_crossroad;
				previous_item->next = item;
				item->prev = previous_item;
				previous_item = item;
				if (_flag_is_back)
					current_position[0]--;
				else
					current_position[0]++;
			}//Доехали по вертикали(A..Z) до нужного поворота, остается поворот и финиш.
			item = new route_item();
			if (_flag_is_forward) {
				if (flag_went_from_left) {
					_flag_is_right = true;
					_flag_is_left = false;
					item->state = r_right;
				}
				else {
					_flag_is_right = false;
					_flag_is_left = true;
					item->state = r_left;
				}
			}
			else {
				if (flag_went_from_left) {
					_flag_is_right = false;
					_flag_is_left = true;
					item->state = r_left;
				}
				else {
					_flag_is_right = true;
					_flag_is_left = false;
					item->state = r_right;
				}

			}
			previous_item->next = item;
			item->prev = previous_item;
			previous_item = item;
			item = new route_item();
			item->state = r_final;
			previous_item->next = item;
			item->prev = previous_item;

		}
	}


	return first_item;
}

void  Route_processor::Initialize_matrix(std::vector<char*>& garden_beds, int **matrix) {
	int n = garden_beds.size();
	for (int i = 1; i <= n; i++) {
		for (int j = i; j <= n; j++) {
			if (i == j) 
			{
				matrix[i][j] = 0;
			}
			else 
			{
				matrix[i][j] = Count_minimal_length(garden_beds[n - i], garden_beds[n - j]);
				matrix[j][i] = matrix[i][j];
				//std::cout << garden_beds[n - i] << " " << garden_beds[n - j] << " " << matrix[i][j];
			}
		}
		
	}
}


int Route_processor::Count_minimal_length(char* garden_bed_1, char* garden_bed_2)
{

		int len = (abs((int)(tolower(garden_bed_1[0])) - (int)(tolower(garden_bed_2[0]))) + abs((int)(tolower(garden_bed_1[1])) - (int)(tolower(garden_bed_2[1]))) * 2);
		if (tolower(garden_bed_1[1]) == tolower(garden_bed_2[1]))
			len += 2;
		return len;
}




