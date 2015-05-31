#ifndef _TASK_PROCESSOR_HPP_
#define _TASK_PROCESSOR_HPP_

#include <vector>

#include "common.hpp"
#include "server_data.hpp"
#include "route_processor.hpp"
#include "serial.hpp"
#include "../../Identifier_recognition/detector/detect_identifier.hpp"

class Task_processor
{
    public:
        Task_processor(size_t left_base_pos, int last_number, char * last_letter);
        Task_processor(std::vector<task> tasks, size_t left_base_pos, int last_number, char * last_letter);
        Task_processor(task task, size_t left_base_pos, int last_number, char * last_letter);
        ~Task_processor();

        int determine_min_route();
        route_vector& get_min_route();
        
        void set_left_base_pos(size_t left_base_pos);
        
        void set_tasks (std::vector<task> tasks);
        void set_tasks (task task);

        int run_task (); // команда, запускающая движение

        int pour_liquid(int ml);

        int drive_by_command(char *command);

    private:
        void extract_garden_bed_addresses();
        int process_garden_bed();
        void clear_route_vector ();
        int get_ml_by_id(char *id);

    private:
        route_vector _route_vector;
        std::vector<task> _tasks;
        Route_processor _route_processor;
        size_t _left_base_pos;
        std::vector <char *> _garden_bed_addresses;

        Serial _serial;

        Identifier_detector _detector;

    public:
};

#endif
