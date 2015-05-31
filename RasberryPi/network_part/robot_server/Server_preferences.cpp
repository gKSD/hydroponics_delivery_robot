#include "Server_preferences.hpp"
char Server_preferences::base_pos[3] = {0};
bool Server_preferences::base_authorized = false;
int Server_preferences::field_width = 0;
int Server_preferences::field_height = 0;
int Server_preferences::max_robot_volume = 500;
std::string Server_preferences::base_ip = "";