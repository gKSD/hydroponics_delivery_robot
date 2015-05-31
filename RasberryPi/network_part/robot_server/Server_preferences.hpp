#ifndef PREFERENCES___HPP
#define PREFERENCES___HPP

#include <string.h>
#include <stdio.h>
#include <string>

class Server_preferences
{
public:
	static bool base_authorized;
	static char base_pos[3];
	static int field_width;
	static int field_height;
	static int max_robot_volume;
	static std::string base_ip;
};

#endif