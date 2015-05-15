#ifndef UART_COMMANDS_H_
#define UART_COMMANDS_H_

#include <Arduino.h> //needed for Serial.println

//команды запроса от RaspberryPi Arduino
const char UART_RUN_FORWARD_TILL_CROSSROAD = 'a';
const char UART_RUN_FORWARD = 'b';
const char UART_TURN_LEFT = 'c';
const char UART_TURN_RIGHT = 'd';
const char UART_POUR_LIQUID_ML = 'e';
const char UART_POUR_ALL_LIQUID = 'f';
const char UART_RETURN_TO_BASE = 'g';
const char UART_GET_CORRENT_POSITION = 'i';
const char UART_GET_BACK_SONAR_VALUE = 'g';
const char UART_GET_FORWARD_SONAR_VALUE = 'k';
const char UART_GET_ALL_SONARS_VALUE = 'l';
const char UART_GET_LEFT_SENSOR_VALUE = 'm';
const char UART_GET_RIGHT_SENSOR_VALUE = 'n';
const char UART_GET_ALL_SENSOR_VALUE = 'o';
const char UART_STOP = 'p'; // команда остановиться (например, для полива)
const char UART_TURN_CAMERA_RIGHT = 'H';
const char UART_TURN_CAMERA_LEFT = 'I';
const char UART_TURN_CAMERA_FORWARD = 'J';
const char UART_TURN_AROUND = 'h';

const char UART_RUN_BACK_TILL_CROSSROAD = 'S';
const char UART_RUN_BACK = 'T';
const char UART_RUN_BACK_BY_DISTANCE = 'U';

//команды ответа Arduino RasberryPi
const char UART_REACH_DEADLOCK_ERROR = 'q'; // достигли тупика
const char UART_TURN_LEFT_COMPLETED = 'r';
const char UART_TURN_RIGHT_COMPLETED = 's';
const char UART_TURN_ERROR = 't'; // ошибка поворота
const char UART_REACHED_CROSSROAD = 'u'; // достигли требуемого перекрестка
const char UART_POUR_LIQUID_COMPLETED = 'v';
const char UART_POUR_LIQUID_ERROR = 'w';
const char UART_RETURN_TO_BASE_COMPLETED = 'x';
const char UART_PING_ALIVE = 'y';
const char UART_STOP_COMPLETED = 'z';

const char UART_GET_CORRENT_POSITION_ANSWER = 'A';
const char UART_GET_BACK_SONAR_VALUE_ANSWER = 'B';
const char UART_GET_FORWARD_SONAR_VALUE_ANSWER = 'C';
const char UART_GET_ALL_SONARS_VALUE_ANSWER = 'D';
const char UART_GET_LEFT_SENSOR_VALUE_ANSWER = 'E';
const char UART_GET_RIGHT_SENSOR_VALUE_ANSWER = 'F';
const char UART_GET_ALL_SENSOR_VALUE_ANSWER = 'G';

const char UART_TURN_CAMERA_RIGHT_COMPLETED = 'K';
const char UART_TURN_CAMERA_LEFT_COMPLETED = 'L';
const char UART_TURN_CAMERA_FORWARD_COMPLETED= 'M';
const char UART_TURN_CAMERA_RIGHT_ERROR = 'N';
const char UART_TURN_CAMERA_LEFT_ERROR = 'O';
const char UART_TURN_CAMERA_FORWARD_ERROR = 'P';

const char UART_RUN_FORWARD_STARTED = 'Q';




const char UART_BAD_COMMAND_ERROR = 'R';
const char UART_COMMAND_LAST_SYMBOL = '*';
const char UART_RESET_SYSTEM = '!';

#endif // UART_COMMANDS_H_