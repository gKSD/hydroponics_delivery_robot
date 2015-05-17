#include <iostream>

#include "task_processor.hpp"
#include "error_codes.hpp"

#include "../../Arduino/Robot/uart_commands.h"

Task_processor::Task_processor(size_t left_base_pos): _route_processor(left_base_pos)
{
    //set_left_base_pos(left_base_pos);
    _serial.serial_open("/dev/ttyAMA0", 9600);
    _serial.uart_setup();
}

Task_processor::Task_processor(std::vector<task> tasks, size_t left_base_pos):  _route_processor(left_base_pos)
{
    set_tasks(tasks);
    //set_left_base_pos(left_base_pos);
    _serial.serial_open("/dev/ttyAMA0", 9600);
    _serial.uart_setup();
}

Task_processor::Task_processor(task task, size_t left_base_pos): _route_processor(left_base_pos)
{
    set_tasks(task);
    //set_left_base_pos(left_base_pos);
    _serial.serial_open("/dev/ttyAMA0", 9600);
    _serial.uart_setup();
}

Task_processor::~Task_processor()
{
    _serial.serial_close();
    clear_route_vector();
}

void Task_processor::clear_route_vector ()
{
    int _route_vector_size = _route_vector.size();
    for (int i = 0; i < _route_vector_size; i++)
    {
        route_item *p, *p1;
        p = _route_vector[i];
        if (p != NULL) p1 = p->next;
        while(p1 != NULL)
        {
            p = p1;
            p1 = p1->next;
            delete p;
        }
    }
    _route_vector.clear();
}

void Task_processor::set_left_base_pos(size_t left_base_pos)
{
    // TODO: сделать установку чреез route_processor
    /*if (left_base_pos < 0 || left_base_pos > 8)
    {
        std::cout << "Error: invalid left_base_pos for stting base position" << std::endl;
        throw SET_BASE_POS_ERROR;
    }*/
}

int Task_processor::determine_min_route()
{
    if (_tasks.size() == 0)
    {
        std::cout << "Error: task vector is empty" << std::endl;
        return -1;
    }

    if (_garden_bed_addresses.size() == 0)
    {
        std::cout << "Error: extract garden beds address before determine route!" <<std::endl;
        return -1;
    }

    //_route_vector.clear();
    clear_route_vector();
    _route_vector = _route_processor.run(_garden_bed_addresses);

    for (int i = 0; i < _route_vector.size(); i++)
    {
        std::cout << "item => "<< _route_vector[i]->state << std::endl;
    }
    for (int i = i; i < _route_vector.size(); i++)
    {
        delete _route_vector[i];
    }

    return 1;
}

route_vector& Task_processor::get_min_route()
{
    return _route_vector;
}

void Task_processor::set_tasks (std::vector<task> tasks)
{
    _tasks.clear();
    _tasks = tasks;
    _garden_bed_addresses.clear();
    //_route_vector.clear();
    clear_route_vector();
    extract_garden_bed_addresses ();
}

void Task_processor::set_tasks (task task)
{
    _tasks.clear();
    _tasks.push_back (task);
    _garden_bed_addresses.clear();
    //_route_vector.clear();
    clear_route_vector();
    extract_garden_bed_addresses ();
}

void Task_processor::extract_garden_bed_addresses ()
{
    if (_garden_bed_addresses.size() > 0) _garden_bed_addresses.clear();
    int len = _tasks.size();
    for (int i = 0; i < len; i++)
    {
        char address[3];
        strcpy (address, _tasks[i]._address);
        _garden_bed_addresses.push_back(address);

        //to delete
        std::cout << "new garden item: " << _garden_bed_addresses[i] << std::endl;
    }
}

int Task_processor::run_task()
{
    if(_tasks.size() == 0)
    {
        std::cout << "Error: task vector is empty" << std::endl;
        return -1;
    }

    if (_route_vector.size() == 0)
    {
        std::cout << "Error: route vector is empty" << std::endl;
        return -1;
    }

    int _route_vector_size = _route_vector.size();

    // устанаваливаем направление движения - прямо,
    // алгоритм коммивояжера возвращает последовательно перещений в прямом направлении
    _serial.serial_putchar(UART_SET_DIRECTION);
    _serial.serial_putchar(r_forward);
    _serial.serial_putchar(UART_COMMAND_LAST_SYMBOL);


    _serial.serial_flush(); // очищаем входной буфер
    _serial.serial_putchar(UART_GET_CURRENT_DIRECTION);
    _serial.serial_putchar(UART_COMMAND_LAST_SYMBOL);
    int data_am = 0;
    while (!(data_am = _serial.serial_data_available())) ; // ожидаем ответ
    char data[100];
    int data_len = 0;

    for (int i = data_am - 1; i >= 0 && data_len < 100; i--, data_len++)
    {
        data[data_len] = _serial.serial_getchar();
    }

    for (int i = 0; i < data_len; i++)
    {
        if (data[i] == UART_SET_DIRECTION && i < data_len - 2)
        {
            if (data[i + 1] != r_forward || data[i + 2] != UART_COMMAND_LAST_SYMBOL)
            {
                std::cout << "Error: setting direction on arduino board" << std::endl;
                return -1;
            }
        }
    }
    std::cout << "111" << std::endl;
    for (int i = 0; i < _route_vector_size; i++)
    {
        route_item *subroute = _route_vector[i];
        while (subroute != NULL)
        {
            char state = subroute->state;
            if (subroute->next == NULL)
            {
                if (state != r_final)
                {
                    std::cout << "Error: route last symbol must be '*'" << std::endl;
                    return -1;
                }
                process_garden_bed();
                break;
            }

            switch (state)
            {
                case r_forward: // в пути, полученном после работы алгоритма коммивояжера движение должно быть только прямо!
                case r_back:    // изменения направления игнорируются
                    break;
                case r_right:
                {
                    _serial.serial_putchar(UART_TURN_RIGHT);
                    _serial.serial_putchar(UART_COMMAND_LAST_SYMBOL);
                    break;
                }
                case r_left:
                {
                    _serial.serial_putchar(UART_TURN_LEFT);
                    _serial.serial_putchar(UART_COMMAND_LAST_SYMBOL);
                    break;
                }
                case r_turn_around:
                {
                    _serial.serial_putchar(UART_TURN_AROUND);
                    _serial.serial_putchar(UART_COMMAND_LAST_SYMBOL);
                    break;
                }
                case r_crossroad:
                {
                    _serial.serial_putchar(UART_RUN_FORWARD_TILL_CROSSROAD);
                    if (subroute->next->state == r_final)
                        _serial.serial_putchar('0');
                    else
                        _serial.serial_putchar('1');
                    _serial.serial_putchar(UART_COMMAND_LAST_SYMBOL);
                    break;
                }
                case r_final:
                {
                    break;
                }
            }
            subroute = subroute->next;
        }
        /*
        const char UART_RUN_FORWARD_TILL_CROSSROAD = 'a';
const char UART_RUN_FORWARD = 'b';
const char UART_TURN_LEFT = 'c';
const char UART_TURN_RIGHT = 'd';
const char UART_POUR_LIQUID_ML = 'e';
const char UART_POUR_ALL_LIQUID = 'f';
const char UART_RETURN_TO_BASE = 'g';
const char UART_GET_CORRENT_POSITION = 'i';
const char UART_GET_ALL_SONARS_VALUE = 'l';
const char UART_GET_ALL_SENSOR_VALUE = 'o';
const char UART_STOP = 'p'; // РєРѕРјР°РЅРґР° РѕСЃС‚Р°РЅРѕРІРёС‚СЊСЃСЏ (РЅР°РїСЂРёРјРµСЂ, РґР»СЏ РїРѕР»РёРІР°)
const char UART_TURN_CAMERA_RIGHT = 'H';
const char UART_TURN_CAMERA_LEFT = 'I';
const char UART_TURN_CAMERA_FORWARD = 'J';
const char UART_TURN_AROUND = 'h';

const char UART_RUN_BACK_TILL_CROSSROAD = 'S';
const char UART_RUN_BACK = 'T';
const char UART_RUN_BACK_BY_DISTANCE = 'U';

const char UART_SET_DIRECTION = 'X';
const char UART_GET_CURRENT_DIRECTION = 'Y';
        */
    }
    std::cout << std::endl;
    return 1;
}

void Task_processor::process_garden_bed()
{}

