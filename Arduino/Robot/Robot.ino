  // Подключим библиотеку для работы с I2C-расширителем портов
#include <Wire.h>
// Подключим библиотеку Strela
#include <Strela.h>
#include <Ultrasonic.h>

//#include "uart_commands.h"

// echo будет общим для всех 4 дальномеров, запускается один дальномер (по trig), результат считывается с общего входа
//#define ECHO P2
#define ECHO0 P3 //forward
#define ECHO1 P4 //right
#define ECHO2 P5 //back
//#define ECHO3 P1 //left, используется для клапана
#define TRIG0 P6 //forward
//#define TRIG1 P8 //right, используется для датчика уровня воды
#define TRIG2 P9 //back
#define TRIG3 P7 //left

#define VALVE P1 // цифровой пин  с клапаном
#define WATER_LEVEL_SENSOR P8 // аналоговый пин с датчиком уровня воды

//датчики линиии
#define LEFT_SENSOR P12
#define RIGHT_SENSOR P11
#define BLACK_THRESHOLD 25  // величина отклонения показаний датчиков на черном поле относительно белого поля


//моторы для поворота камеры
#define MOTOR_PIN1  P10
#define MOTOR_PIN2  P2

//состояния при движении робота
#define STATE_FORWARD          0
#define STATE_RIGHT            1
#define STATE_LEFT             2
#define STATE_STOP             3
#define STATE_BASE             4
#define STATE_CROSSROAD        5
#define STATE_BACK             6

//UART states
#define USTATE_RUN_FORWARD                  0
#define USTATE_RUN_FORWARD_TILL_CROSSROAD   1
#define USTATE_RETURN_TO_BASE               2
#define USTATE_PASSED_CROSSROADS            3
#define USTATE_ERROR_SAVING_ROAD            4
#define USTATE_ERROR_DEADLOCK               5
#define USTATE_RUN_BACK                     6
#define USTATE_RUN_BACK_TILL_CROSSROAD      7
#define USTATE_RUN_BACK_BY_DISTANCE         8

//********************************************************************************************************************************************************************************************************
// команды последовательного протокола при взаиможействии RasberryPi и Arduino
//команды запроса от RaspberryPi Arduino
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
const char UART_STOP = 'p'; // команда остановиться (например, для полива)
const char UART_TURN_CAMERA_RIGHT = 'H';
const char UART_TURN_CAMERA_LEFT = 'I';
const char UART_TURN_CAMERA_FORWARD = 'J';
const char UART_TURN_AROUND = 'h';

const char UART_RUN_BACK_TILL_CROSSROAD = 'S';
const char UART_RUN_BACK = 'T';
const char UART_RUN_BACK_BY_DISTANCE = 'U';

const char UART_SET_DIRECTION = 'X';
const char UART_GET_CURRENT_DIRECTION = 'Y';

//команды ответа Arduino RasberryPi
const char UART_REACH_DEADLOCK_ERROR = 'q'; // достигли тупика
const char UART_TURN_COMPLETED = 'r';
const char UART_TURN_ERROR = 't'; // ошибка поворота
const char UART_REACHED_CROSSROAD = 'u'; // достигли требуемого перекрестка
const char UART_POUR_LIQUID_COMPLETED = 'v';
const char UART_POUR_LIQUID_ERROR = 'w';
const char UART_RETURN_TO_BASE_COMPLETED = 'x';
const char UART_STOP_COMPLETED = 'z';

const char UART_GET_CORRENT_POSITION_ANSWER = 'A';
const char UART_GET_ALL_SONARS_VALUE_ANSWER = 'D';
const char UART_GET_ALL_SENSOR_VALUE_ANSWER = 'G';

const char UART_TURN_CAMERA_COMPLETED = 'K';
const char UART_TURN_CAMERA_ERROR = 'O';

const char UART_RUN_FORWARD_STARTED = 'Q';
const char UART_RUN_BACK_STARTED = 'Z';
const char UART_RUN_BACK_BY_DISTANCE_COMPLETED = 'L';

const char UART_WAIT_ANSWER_TIMEOUT = 'V';
const char UART_RETURN_TO_BASE_STARTED = 'W';


const char UART_BAD_COMMAND_ERROR = 'R';
const char UART_COMMAND_LAST_SYMBOL = '*';
const char UART_RESET_SYSTEM = '!';
//********************************************************************************************************************************************************************************************************


Ultrasonic ultrasonic0(TRIG0, P3); // (Trig PIN,Echo PIN)
//Ultrasonic ultrasonic1(TRIG1, P4); // (Trig PIN,Echo PIN)
Ultrasonic ultrasonic2(TRIG2, P5); // (Trig PIN,Echo PIN)
//Ultrasonic ultrasonic3(TRIG3, P1);

const char US_FORWARD = 'f', US_BACK = 'b', US_RIGHT = 'r', US_LEFT = 'l';

int state = STATE_BASE; // !!! Обязательно в начальный момент времени робот должен быть установлен на базу, датчики линии при этом должны находиться над зазором между базой и линий дороги
int first_left_state = 0;
int first_right_state = 0;

int currentSpeed = 150;

int straight_crossroads_counter = 0;
int required_straight_crossroads_amount = 0;

int required_back_distance = 0;


const char r_back = 'b';
const char r_forward = 'f';
const char r_crossroad = '+';
const char r_left = '<';
const char r_right = '>';
const char r_turn_around = '&';
const char r_deadlock = 'd';


int targetState; // текущее состояние в зависимости от положение колес
int ustate = -1;
char current_global_direction = r_forward; // b - back, f = forward

bool is_crossroad_global = false;

#define MAX_ROAD_OBJECT 256
char passed_road_objects [MAX_ROAD_OBJECT]; //массив для хранения пройденных элементов
int passed_road_objects_len = 0;

bool timeout = false;
int interrupts_counter = 0;
int interrupts_counter_max = 30;

#define MIN_VOLUME_ML 10
int ten_ml_units = 33; // units, 33 ед. соответствуют 10 мл  (по умолчанию)
int total_volume_ml = 150; // ml, 150 мл - default
int required_volume_to_pour_ml = 0; //ml
int volume_left_ml = 0; //ml

bool is_camera_turned = false;
bool camera_turn_left = false;

void setup() 
{
    pinMode(MOTOR_PIN1, OUTPUT);
    pinMode(MOTOR_PIN2, OUTPUT);
  
    //pinMode(WATER_LEVEL_SENSOR, OUTPUT);
    pinMode(VALVE, OUTPUT); // настраиваем выход пин для управления клапаном (цифровой выход)
    sei();//разрешаем прерывания глобально
    // Настройка последовательного протокола UART для взаимодействия с Rasberry Pi
    Serial1.begin(9600);
    while (!Serial1)
      ; // wait for serial port to connect. Needed for Leonardo only
    
    Serial.begin(9600); // Инициализируем сериал порт для вывода результата 
    delay(10000);
    get_sensors_start_state ();
}
bool test= true;
void loop()
/*
  основной цикл, в котором проверяюся флаги:
  1. is_home - является ли остановка домашней станцией
  2. is_stop - является ли остановка требуемой остановкой для наполнения жидкостью
  3. (else) проверка текущего состояния робота при езде, положения колес, выбор дальнейшего поведения
*/
{
  
    //if (state == STATE_BASE)
    //{
    //  run_base_state_process();
    //}
    check_commands();
    process_commands();
}

boolean add_road_passed_object (char c)
{
    if (passed_road_objects_len < MAX_ROAD_OBJECT - 1)
    {
        passed_road_objects[passed_road_objects_len] = c; // сохраняем, что проехали перекресток
        passed_road_objects_len++;
        return true;
    }
    return false;
}

// ***************************************************************** Таймер (для таймаута) *****************************************************************************
#define TIMER_RESOLUTION 65535 // максимальное число, которое может быть записано в регистр таймера TCNT1
#define PRESCALER 1024
#define TIMER_SET_VALUE_FOR_ANSWER 4
#define TIMER_SET_VALUE_FOR_STABILIZATION_AFTER_TURN 1
//работа с таймером Timer1 (16битный таймер)
typedef void (*user_function)(void);
int set_timeout (int timeout, void (*user_function) (void));
int reset_timer_counter (int timeout);

void (*timer1_callback)();
ISR(TIMER1_OVF_vect) // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
    TIMSK1 &= ~(1 << TOIE1); // запрещаем прерывания по переполнению
    TIFR1 &= ~(1 << TOV1); // снимаем флаг - по переполнению
    timer1_callback();
}
int  reset_timer_counter (int timeout)
{
    if (timeout*F_CPU/PRESCALER > TIMER_RESOLUTION) return -1;
    TCNT1 = TIMER_RESOLUTION - F_CPU/PRESCALER * timeout;

    TCCR1A = 0x0; //очищаем управляющее слова, используемый режим - обычный счетный режим
    TCCR1B = (1 << CS12)|(1 << CS10); //выбираем делитель частоты, 
    
    sei();//разрешаем прерывания глобально
    TIMSK1 = (1 << TOIE1); // разрешить прерывания по переполнению
    TIFR1 &= (1 << TOV1); //устанваливаем флаг - по переполнению
    return 1;
}

int set_timeout (int timeout, void (*user_function) (void)) { //timeout in seconds + callback function
    
    if (timeout*F_CPU/PRESCALER > TIMER_RESOLUTION) return -1;
    timer1_callback = user_function;
    
    unsigned char sreg;
    
    sreg = SREG;

    TCNT1 = TIMER_RESOLUTION - F_CPU/PRESCALER * timeout;

    TCCR1A = 0x0; //очищаем управляющее слова, используемый режим - обычный счетный режим
    TCCR1B = (1 << CS12)|(1 << CS10); //выбираем делитель частоты, 
    
    sei();//разрешаем прерывания глобально
    TIMSK1 = (1 << TOIE1); // разрешить прерывания по переполнению
    TIFR1 &= (1 << TOV1); //устанваливаем флаг - по переполнению
    
    SREG = sreg;
    return 1;
}
// ****************************************************************************************************************************************  ****************************
void wait_answer_timeout()
{
  interrupts_counter++;
  if (interrupts_counter < interrupts_counter_max) reset_timer_counter(TIMER_SET_VALUE_FOR_ANSWER);
  else
  {
      Serial.print(UART_WAIT_ANSWER_TIMEOUT); //1
      Serial.print(UART_RETURN_TO_BASE_STARTED); //1
      ustate = USTATE_RETURN_TO_BASE;
      timeout = true;
  }
}

void stabilization_timeout()
{
  timeout = true;
}

void process_commands()
/*
  функция обработки длительных команд, например, движение вперед, которе выполняется не за один "такт", при этом необходимо постоянно опрашивать последовательный канал
*/
{
   switch(ustate)
   {
     case USTATE_RUN_FORWARD:
         if (!run_forward_with_stabilization(false, true))
           ustate = USTATE_ERROR_SAVING_ROAD;
         break;
      case USTATE_RUN_FORWARD_TILL_CROSSROAD:
          if (!run_forward_with_stabilization(true, true))
              ustate = USTATE_ERROR_SAVING_ROAD;
          break;
      case USTATE_RUN_BACK_TILL_CROSSROAD:
          if (!run_forward_with_stabilization(true, false))
              ustate = USTATE_ERROR_SAVING_ROAD;
          break;
      case USTATE_RUN_BACK:
           if (!run_forward_with_stabilization(false, false))
             ustate = USTATE_ERROR_SAVING_ROAD;
          break;
      case USTATE_RUN_BACK_BY_DISTANCE:
          stepBackByDistance(required_back_distance);
          required_back_distance = 0;
          Serial.print (UART_RUN_BACK_BY_DISTANCE_COMPLETED); //1
          ustate = -1;
          break;
      case USTATE_RETURN_TO_BASE:
          return_to_base();
          break;
      case USTATE_PASSED_CROSSROADS:
          // отправляем подтверждение, что достигли требуемого перекрестка, ожидаем указаний от расберри
          Serial.print(UART_REACHED_CROSSROAD); //1
          ustate = -1;
          break;
      case USTATE_ERROR_SAVING_ROAD:
          Serial.print(UART_RESET_SYSTEM); //1
          ustate = -1;
          break;
      case USTATE_ERROR_DEADLOCK:
          runStop();
          Serial.print(UART_REACH_DEADLOCK_ERROR); //1
          if (!add_road_passed_object(r_deadlock))
          {
              ustate = USTATE_ERROR_SAVING_ROAD;
              return;
          }
          //здесь запускаем таймаут на ожидание ответа от управдяющего устройства. По таймауту - возвращение на базу
          timeout = false;
          interrupts_counter = 0;
          interrupts_counter_max = 30;
          set_timeout(TIMER_SET_VALUE_FOR_ANSWER, wait_answer_timeout); // таймаут на 120 сек = 4 (TIMER_SET_VALUE_FOR_ANSWER) * 30 (interrupts_counter_max)
          break;
   }
}

void check_commands ()
/*
 * проверяет команды, поступившие от управляющего устройства (RasberryPi)
 */
{
    if (!Serial.available()) return; //1 
    char command = Serial.read(); //1
    char params[100];
    int i = 0;
    while (Serial.available() && i < 100) //1
    {
        params[i] = Serial.read(); //1
        if (params[i] == UART_COMMAND_LAST_SYMBOL)
          break;
        i++;
    }
 
    if (params[i] != UART_COMMAND_LAST_SYMBOL)
    {
        Serial.print(UART_BAD_COMMAND_ERROR); //1
        return;
    }
    
    Serial.print ("Uart command:  ");
    Serial.println (command);
    
    if (command ==  UART_RUN_FORWARD_TILL_CROSSROAD ) //Формат кадра: <код команды> <сколько перекрестков>*
    {
        if (!add_road_passed_object(r_forward))
        {
            ustate = USTATE_ERROR_SAVING_ROAD;
            return;
        }
        
        ustate = USTATE_RUN_FORWARD_TILL_CROSSROAD;
        // отправка подтверждения о получении команды
        Serial.print(UART_RUN_FORWARD_STARTED); //1
        params[i] = '\0';
        required_straight_crossroads_amount = atoi(params);
        Serial.print("Crossroads amount: "); Serial.println(required_straight_crossroads_amount);
        straight_crossroads_counter = 0;
        current_global_direction = r_forward;
    }
    else if (command == UART_RUN_FORWARD ) //Формат кадра: <код команды> *
    {
        if (!add_road_passed_object(r_forward))
        {
            ustate = USTATE_ERROR_SAVING_ROAD;
            return;
        }
        ustate = USTATE_RUN_FORWARD;
        // отправка подтверждения о получении команды  
        Serial.print(UART_RUN_FORWARD_STARTED); //1
        current_global_direction = r_forward;
    }
    if (command ==  UART_RUN_BACK_TILL_CROSSROAD ) //Формат кадра: <код команды> <сколько перекрестков>*
    {
        if (!add_road_passed_object(r_back))
        {
            ustate = USTATE_ERROR_SAVING_ROAD;
            return;
        }
        ustate = USTATE_RUN_BACK_TILL_CROSSROAD;
        // отправка подтверждения о получении команды
        Serial.print(UART_RUN_BACK_STARTED); //1
        params[i] = '\0';
        required_straight_crossroads_amount = atoi(params);
        Serial.print("Crossroads amount: "); Serial.println(required_straight_crossroads_amount);
        straight_crossroads_counter = 0;
        current_global_direction = r_back;
    }
    else if (command == UART_RUN_BACK ) //Формат кадра: <код команды> *
    {
        if (!add_road_passed_object(r_back))
        {
            ustate = USTATE_ERROR_SAVING_ROAD;
            return;
        }
        ustate = USTATE_RUN_BACK;
        // отправка подтверждения о получении команды  
        Serial.print(UART_RUN_BACK_STARTED); //1
        current_global_direction = r_back;
    }
    else if (command == UART_RUN_BACK_BY_DISTANCE)
    {
        if (!add_road_passed_object(r_back))
        {
            ustate = USTATE_ERROR_SAVING_ROAD;
            return;
        }
        ustate = USTATE_RUN_BACK_BY_DISTANCE;
        // необходимо сохранить расстояние на которое надо смещаться
        params[i] = '\0';
        required_back_distance = atoi(params);
        Serial.print("Run back by distance: "); Serial.println(required_back_distance);
        current_global_direction = r_back;
    }
    else if (command == UART_TURN_LEFT )
    {
        if (!is_crossroad_global)
        {
          //поворачивать и разворачиваться можно только на перекрестке
           Serial.print(UART_TURN_ERROR); //1
           return;
        }
        if (add_road_passed_object(r_left))
        {
             if (current_global_direction == r_forward) turnInPlace(true, true);
             else if (current_global_direction == r_back) turnInPlace(true, false);
             ustate = -1;
             Serial.print(UART_TURN_COMPLETED); //1
        }
        else
           ustate = USTATE_ERROR_SAVING_ROAD;
    }
    else if (command == UART_TURN_RIGHT )
    {
        if (!is_crossroad_global)
        {
             //поворачивать и разворачиваться можно только на перекрестке
             Serial.print(UART_TURN_ERROR); //1
             return;
        }
        if (add_road_passed_object(r_right))
        {
           if (current_global_direction == r_forward) turnInPlace(false, true);
           else if (current_global_direction == r_back) turnInPlace(false, false);
           ustate = -1;
           Serial.print(UART_TURN_COMPLETED); //1
        }
        else
           ustate = USTATE_ERROR_SAVING_ROAD;
    }
    else if (command == UART_TURN_AROUND)
    {
       if (!is_crossroad_global)
       {
           //поворачивать и разворачиваться можно только на перекрестке
           Serial.print(UART_TURN_ERROR); //1
           return;
       }
       if (!add_road_passed_object(r_turn_around))
       {
           ustate = USTATE_ERROR_SAVING_ROAD;
           return;
       }
       turnAroundAtCrossroad(); // в теории это поворот на перекрестке, другие запрещены
       //проедем вперед для выравнивания после разворота
       if(!run_forward_with_stabilization(false, true)) // в случае если при езде вперед встретился перекресток
       {
         ustate = USTATE_ERROR_SAVING_ROAD;
         return;
       }
       delay (300);
       runStop();
       Serial.print(UART_TURN_COMPLETED); //1
       ustate = -1;
    }
    else if (command == UART_SET_DIRECTION)
    {
        if (*params == r_back || *params == r_forward)
            current_global_direction = *params;
        else
            current_global_direction = r_forward;
        Serial.print(UART_SET_DIRECTION); //1
        Serial.print(current_global_direction); //1 передает установленное направление
        ustate = -1;
    }
    else if (command == UART_GET_CURRENT_DIRECTION)
    {
        Serial.print(UART_SET_DIRECTION); //1
        Serial.print(current_global_direction); //1
        ustate = -1;
    }
    else if (command == UART_POUR_LIQUID_ML )
    {
        params[i] = '\0';
        required_volume_to_pour_ml = atoi(params);
        Serial.print("Liquid volume to pour (ml): "); Serial.println(required_volume_to_pour_ml);
        if (required_volume_to_pour_ml > volume_left_ml)
        {
            Serial.print(UART_POUR_LIQUID_ERROR); //1
            return;
        }
        pour_liquid_ml(required_volume_to_pour_ml);
        Serial.print(UART_POUR_LIQUID_COMPLETED); //1
        ustate = -1;
    }
    else if (command == UART_POUR_ALL_LIQUID )
    {
        if (volume_left_ml > MIN_VOLUME_ML)
            pour_liquid_ml(volume_left_ml);
        Serial.print(UART_POUR_LIQUID_COMPLETED); //1
        ustate = -1;
    }
    else if (command == UART_RETURN_TO_BASE )
    {
        Serial.print(UART_RETURN_TO_BASE_STARTED); //1
        ustate = USTATE_RETURN_TO_BASE;
    }
    else if (command == UART_GET_CORRENT_POSITION )
    {
        Serial.print(UART_GET_CORRENT_POSITION_ANSWER); //1
        for (int i = 0; i < passed_road_objects_len; i++)
          Serial.print(passed_road_objects[i]); //1
        Serial.print(UART_COMMAND_LAST_SYMBOL); //1
        ustate = -1;
    }
    else if (command == UART_GET_ALL_SONARS_VALUE )
    {
        int distance_forward = interrogate_ultrasonics (US_FORWARD);
        int distance_back = interrogate_ultrasonics (US_BACK);
        Serial.print(UART_GET_ALL_SONARS_VALUE_ANSWER); //1
        Serial.print("b:"); //back //1
        Serial.print(distance_back); //1
        Serial.print("f:"); //back //1
        Serial.print(distance_forward); //1
        Serial.print(UART_COMMAND_LAST_SYMBOL); //1
        ustate = -1;
    }
    else if (command == UART_GET_ALL_SENSOR_VALUE )
    {
        int cur_left = uAnalogRead(LEFT_SENSOR); // считываем текущее значение с левого датчика
        int cur_right = uAnalogRead(RIGHT_SENSOR); // считываем текущее значение с правого датчика
        Serial.print(UART_GET_ALL_SENSOR_VALUE_ANSWER); //1
        Serial.print("l:"); //back //1
        Serial.print(cur_left); //1
        Serial.print("r:"); //back //1
        Serial.print(cur_right); //1
        Serial.print(UART_COMMAND_LAST_SYMBOL); //1
        ustate = -1;
    }
    else if (command == UART_STOP )
    {
        runStop();
        Serial.print(UART_STOP_COMPLETED);
        ustate = -1;
    }
    else if (command == UART_TURN_CAMERA_RIGHT )
    {
        turn_camera(false);
        Serial.print(UART_TURN_CAMERA_COMPLETED); //1
        ustate = -1;
    }
    else if (command == UART_TURN_CAMERA_LEFT )
    {
        turn_camera(true);
        Serial.print(UART_TURN_CAMERA_COMPLETED); //1
        ustate = -1;
    }
    else if (command == UART_TURN_CAMERA_FORWARD )
    {
        turn_camera_forward();
        Serial.print(UART_TURN_CAMERA_COMPLETED); //1
        ustate = -1;
    }
}

void turn_camera(bool left)
{
    if (left)
    {
        uDigitalWrite(MOTOR_PIN1, HIGH);
        uDigitalWrite(MOTOR_PIN2, LOW);
        camera_turn_left = true;
    }
    else
    {
        uDigitalWrite(MOTOR_PIN1, LOW);
        uDigitalWrite(MOTOR_PIN2, HIGH);
        camera_turn_left = false;
    }
    delay (200);
    is_camera_turned = true;
    uDigitalWrite(MOTOR_PIN1, LOW);
    uDigitalWrite(MOTOR_PIN2, LOW);
}
void turn_camera_forward()
{
    if (!is_camera_turned) return;
    if (camera_turn_left)
    {
        uDigitalWrite(MOTOR_PIN1, LOW);
        uDigitalWrite(MOTOR_PIN2, HIGH);
    }
    else
    {
        uDigitalWrite(MOTOR_PIN1, HIGH);
        uDigitalWrite(MOTOR_PIN2, LOW);
    }
    delay (100);
    is_camera_turned = false;
    uDigitalWrite(MOTOR_PIN1, LOW);
    uDigitalWrite(MOTOR_PIN2, LOW);
}


void return_to_base()
{
    bool first = true;
    char cur_direction = '\0';
    int cur_direction_pos = passed_road_objects_len + 1;
    while (passed_road_objects_len > 0)
    {
        int cur_pos = passed_road_objects_len - 1;
        if (cur_direction_pos > cur_pos)
        {
            for (int i = passed_road_objects_len - 1; i >= 0; i--)
            {
                if (passed_road_objects[i] == r_back || passed_road_objects[i] == r_forward)
                {
                    cur_direction_pos = i;
                    cur_direction = passed_road_objects[i];
                    current_global_direction = cur_direction == r_back? r_forward: r_back;
                    break;
                }
            }
        }

        straight_crossroads_counter = 0;
        required_straight_crossroads_amount = 0;
        while (ustate == USTATE_RETURN_TO_BASE && run_forward_with_stabilization (true, true)) 
        {
            check_commands();
            if (ustate != USTATE_RETURN_TO_BASE)
            {
                process_commands();
                break;
            }
        }
        
        if (ustate == USTATE_PASSED_CROSSROADS) ustate = USTATE_RETURN_TO_BASE;
        else return;

        switch (passed_road_objects[cur_pos])
        {
            case r_crossroad:
            {
                if (first) first = false;
                straight_crossroads_counter = 0;
                required_straight_crossroads_amount = 1;
                bool dir = current_global_direction == r_forward? true: false;
                while (ustate == USTATE_RETURN_TO_BASE && run_forward_with_stabilization (true, dir))
                {
                    check_commands();
                    if (ustate != USTATE_RETURN_TO_BASE)
                    {
                        process_commands();
                        break;
                    }
                }
                
                if (ustate == USTATE_PASSED_CROSSROADS) ustate = USTATE_RETURN_TO_BASE;
                else return;
                break;
            }
            case r_left:
            {
                if (first) first = false;
                if (current_global_direction == r_back)
                    turnInPlace(false, false);
                else if (current_global_direction == r_forward)
                    turnInPlace(false, true);
                break;
            }
            case r_right:
            {
                if (first) first = false;
                if (current_global_direction == r_back)
                    turnInPlace(true, false);
                else if (current_global_direction == r_forward)
                    turnInPlace(true, true);
                break;
            }
            case r_back:
            case r_forward:
            case r_turn_around:
            case r_deadlock:
            {
                if (first) first = false;
                //ничего не делаем
                break;
            }
        }
        passed_road_objects_len--;
    }
}

void run_base_state_process()
{
    Serial.println("----------------------------------------- run base -----------------------------------------");
    delay (5000); // дополнительное время на то, чтобы получить правиьные данные с датчиков
    int distance_forward = interrogate_ultrasonics (US_FORWARD);
    int distance_back = interrogate_ultrasonics (US_BACK);
    
    Serial.print ("distance_forward => ");
    Serial.println (distance_forward);
    Serial.print ("distance_back => ");
    Serial.println (distance_back);
    
    if (distance_forward < distance_back)
    // необходимо развернуть робота для наполнения раствором
    {
         turnAround();
         //проедем вперед для выравнивания после разворота
         if(!run_forward_with_stabilization(false, true))
             ustate = USTATE_ERROR_SAVING_ROAD;
         delay (300);
         runStop();
    }

    // проедем назад до тех пор расстояние до базы не станет равным 6
    stepBackByDistance(10);
    
    straight_crossroads_counter = 0; // сбрасываем счетчик перкрестков (just in case)
    state = STATE_STOP;
}
// ********************************************************************************** Ultrasonics **********************************************************************************
int interrogate_ultrasonics (const char us)
/*
 * Функция опроса всех ультразвуковых дальномеров
 */
{
  int distance = 0;
  //{US_FORWARD, US_BACK, US_RIGHT, US_LEFT};
  switch (us)
  {
    case US_FORWARD:
      distance = ultrasonic0.Ranging(CM);
      Serial.print ("Forward: ");
      Serial.print (distance);
      Serial.println(" cm  ");
      break;
    case US_BACK:
      distance = ultrasonic2.Ranging(CM);
      Serial.print ("Back: ");
      Serial.print (distance);
      Serial.println(" cm  ");
      break;
  }
  delay(100); // in case next ultrasonic call  
  return distance;
}
// ********************************************************************************** Test drives **********************************************************************************

void test_drives()
{
  drive (-180, 180);
}

void runForward(bool forward)
{
    uDigitalWrite(L1, LOW);
    uDigitalWrite(L2, HIGH);
    uDigitalWrite(L3, HIGH);
    uDigitalWrite(L4, LOW);
 
     if (forward) 
        drive (currentSpeed, currentSpeed);
    else 
        drive(-currentSpeed, -currentSpeed);
}

void runStop()
{
  drive(0,0);
}

void turnAround ()
{
    drive(currentSpeed, -currentSpeed);
    delay (200);
  
    int cur_left, cur_right; // текущее значение на датчиках, левом и правом соответственно
    int ldelta, rdelta; //дельта между исходным (белым!) значением полотона и текущим,  левая и правая соответственно
    boolean left, right; // флаг того, находится ли левое и правое колесо робота на черной полосе (1) или на белой (0)
    bool was_black_line = false;
    
    while(true)
    {
      //Serial.print ("cycle      ");
      cur_left = uAnalogRead(LEFT_SENSOR); // считываем текущее значение с левого датчика
      cur_right = uAnalogRead(RIGHT_SENSOR); // считываем текущее значение с правого датчика
  
      ldelta = cur_left - first_left_state;
      rdelta = cur_right - first_right_state;
      
      right = !(rdelta < -BLACK_THRESHOLD); // если текущее отклонения значения правого датчика от исходного более, чем на 75 и в отрицательную сторону, значит правое колесо попало на черную линию
      left = !(ldelta < -BLACK_THRESHOLD); // если текущее отклонения значения левого датчика от исходного более, чем на 75 и в отрицательную сторону, значит левое колесо попало на черную линию

      if (left == 0 && !was_black_line)
      // колесо в сторону разворота попало на черную линию
      {
        was_black_line = true;
        delay(300);
      }
      
      if (left && was_black_line)
      {
        runStop();
        return;
      }
    }
}

void turnAroundAtCrossroad ()
//разворот на перекрестке отличается от того, который просто на прямой линии происходит
{
    drive(currentSpeed, -currentSpeed);
    delay (200);
  
    int cur_left, cur_right; // текущее значение на датчиках, левом и правом соответственно
    int ldelta, rdelta; //дельта между исходным (белым!) значением полотона и текущим,  левая и правая соответственно
    boolean left, right; // флаг того, находится ли левое и правое колесо робота на черной полосе (1) или на белой (0)
    bool is_black_line = false;
    int black_line_counter = 0;
    
    while(true)
    {
      //Serial.print ("cycle      ");
      cur_left = uAnalogRead(LEFT_SENSOR); // считываем текущее значение с левого датчика
      cur_right = uAnalogRead(RIGHT_SENSOR); // считываем текущее значение с правого датчика
  
      ldelta = cur_left - first_left_state;
      rdelta = cur_right - first_right_state;
      
      right = !(rdelta < -BLACK_THRESHOLD); // если текущее отклонения значения правого датчика от исходного более, чем на 75 и в отрицательную сторону, значит правое колесо попало на черную линию
      left = !(ldelta < -BLACK_THRESHOLD); // если текущее отклонения значения левого датчика от исходного более, чем на 75 и в отрицательную сторону, значит левое колесо попало на черную линию

      if (left == 0 && !is_black_line && black_line_counter <= 2)
      // колесо в сторону разворота попало на черную линию 1 раз
      {
        is_black_line = true;
        black_line_counter++;
        delay(300);
      }
      
      if (left == 1 && is_black_line && black_line_counter <= 2)
      // колесо в сторону разворота попало на белую линию между двумя чрными
      {
        is_black_line = false;
        delay(300);
      }
      
      if (left == 0 && !is_black_line && black_line_counter <= 2)
      // колесо в сторону разворота попало на черную линию 1 раз
      {
        is_black_line = true;
        black_line_counter++;
        delay(300);
      }
      
      if (left && is_black_line && black_line_counter == 2)
      // проехали левым колесом 2 белых промежутка и 2 черные линии
      {
        runStop();
        return;
      }
    }
}

void steerRight(bool forward)
{
    state = STATE_RIGHT;
    uDigitalWrite(L1, LOW);
    uDigitalWrite(L2, LOW);
    uDigitalWrite(L3, LOW);
    uDigitalWrite(L4, HIGH);
    
    if (forward)
      drive (0, currentSpeed + 20);
    else
      drive (0, -currentSpeed - 20);
}

void steerLeft(bool forward)
{
    state = STATE_LEFT;
    uDigitalWrite(L1, HIGH);
    uDigitalWrite(L2, LOW);
    uDigitalWrite(L3, LOW);
    uDigitalWrite(L4, LOW);

    if (forward)
      drive(currentSpeed + 20, 0);
    else
      drive (-currentSpeed - 20, 0);
}

void turnInPlace(int turn_left, bool is_forward)
/*
  функция поворота робота (на гусеничной платформе) на одном месте.
  Входной параметр определяет направление поворота: turn_left == 1 => налево, turn_left == 0 => направо
*/
{
  Serial.println("***************************************** turnInPlace *********************************************** ");
    if (turn_left)
    {
        if (is_forward) drive (currentSpeed, -currentSpeed);
        else drive (-currentSpeed, currentSpeed);
    }
    else
    {
        if (is_forward) drive (-currentSpeed, currentSpeed);
        else  drive (currentSpeed, -currentSpeed);
    }
    delay(200);
    boolean is_black = false;
    while(true)
    {
      int cur_left = uAnalogRead(LEFT_SENSOR); // считываем текущее значение с левого датчика
      int cur_right = uAnalogRead(RIGHT_SENSOR); // считываем текущее значение с правого датчика
 
      int ldelta = cur_left - first_left_state;
      int rdelta = cur_right - first_right_state;
      
      boolean right = !(rdelta < -BLACK_THRESHOLD); // если текущее отклонения значения правого датчика от исходного более, чем на 75 и в отрицательную сторону, значит правое колесо попало на черную линию
      boolean left = !(ldelta < -BLACK_THRESHOLD); // если текущее отклонения значения левого датчика от исходного более, чем на 75 и в отрицательную сторону, значит левое колесо попало на черную линию
    
      if (!is_black && ((turn_left && !left) || (!turn_left && !right))) is_black = true;
      if (is_black && ((right && !turn_left) || (left && turn_left)))
      {
        timeout = false;
        set_timeout(TIMER_SET_VALUE_FOR_STABILIZATION_AFTER_TURN, stabilization_timeout); // таймаут на 1 сек (TIMER_SET_VALUE_FOR_STABILIZATION_AFTER_TURN)
        while (!timeout)
        {
            if (is_forward ) run_forward_with_stabilization(false, true); // проехали для выравнивания после поворота
            else run_forward_with_stabilization(false, false); // проехали для выравнивания после поворота
        }
        runStop(); // останавливаем робота
        return;
      }
    }
}

void stepBackByDistance(int distance) 
/*
  движение назад на определенное расстояние
*/
{
    //включение 4х светоиодов для отображения направления движения
    uDigitalWrite(L1, HIGH);
    uDigitalWrite(L2, HIGH);
    uDigitalWrite(L3, HIGH);
    uDigitalWrite(L4, HIGH);    
    drive(-currentSpeed, -currentSpeed);
    
    int distance_back = 1000;
    
    while (true)
    {
      distance_back = interrogate_ultrasonics (US_BACK);
      if (distance_back <= distance)
      {
          runStop();
          break;
       }
     }
    uDigitalWrite(L1, LOW);
    uDigitalWrite(L2, LOW);
    uDigitalWrite(L3, LOW);
    uDigitalWrite(L4, LOW);
}

// ********************************************************************************** Sensors **********************************************************************************
void get_sensors_start_state ()
/*
  получение начальных знаечний для датчиков линии - на белом фоне
*/
{
  
      first_left_state = uAnalogRead(LEFT_SENSOR);; // начальная инициализация значений датчиков (ТРЕБОВАНИЕ - изначально поставить робота на белое)
      first_right_state = uAnalogRead(RIGHT_SENSOR); // начальная инициализация значений датчиков (ТРЕБОВАНИЕ - изначально поставить робота на белое)
      Serial.print("first_left_state => ");
      Serial.println(first_left_state);
      Serial.print("first_right_state => ");
      Serial.println(first_right_state);
}

boolean run_forward_with_stabilization(bool need_crossroads, bool is_forward)
/*
  прямое движение вперед, пока не случится таймаут или не придет команда остановится или сменить направление
*/
{
      is_crossroad_global = false;
      int distance_forward = interrogate_ultrasonics (is_forward? US_FORWARD: US_BACK);
      if (distance_forward <= 10)
      {
        // останавливаем движение
        // меняем статус 
        // отправляем сообщение об ошибке - достигнут тупик
        runStop();
        ustate = USTATE_ERROR_DEADLOCK;
        //Serial.print(UART_REACH_DEADLOCK_ERROR); //1
        return true;
      }

      //check_state(true); // for forward ride
      check_state(is_forward);
      
      switch (targetState) {
          case STATE_FORWARD:
              //runForward(true); //движение прямо
              runForward(is_forward);
              break;
  
          case STATE_RIGHT:
              //steerRight(true); //поворот направо
              steerRight(is_forward);
              break;
  
          case STATE_LEFT:
              //steerLeft(true); //поворот налево
              steerLeft(is_forward);
              break;
              
          case STATE_CROSSROAD:
          {
            is_crossroad_global = true;
            boolean is_last = false;
            if (!add_road_passed_object(r_crossroad))
                return false;

            if (need_crossroads)
            {
                if (straight_crossroads_counter == required_straight_crossroads_amount)
                {
                    is_last = true;
                    runStop();
                    ustate = USTATE_PASSED_CROSSROADS;
                    //выполнили команду, проехали определенное количество перекрестков
                }
                else
                {
                    straight_crossroads_counter++;
                }
            }

            //pass_crossroad(true);
            if (!need_crossroads || (need_crossroads && ( is_forward || (!is_last && !is_forward))))
                pass_crossroad(is_forward); // при движении назад для корректных поворотов необходимо останавливаться до перекрестка, а при движении вперед проехать его
            break;
          }
          case STATE_STOP:
              break;
      }
      return true;
}

void pass_crossroad(bool forward)
{
    runForward(forward); //начинаем движение вперед или назад в соотвествии с флагом
    delay(500);
    while(true)
    {
      int cur_left = uAnalogRead(LEFT_SENSOR); // считываем текущее значение с левого датчика
      int cur_right = uAnalogRead(RIGHT_SENSOR); // считываем текущее значение с правого датчика
 
      int ldelta = cur_left - first_left_state;
      int rdelta = cur_right - first_right_state;
      
      boolean right = !(rdelta < -BLACK_THRESHOLD); // если текущее отклонения значения правого датчика от исходного более, чем на 75 и в отрицательную сторону, значит правое колесо попало на черную линию
      boolean left = !(ldelta < -BLACK_THRESHOLD); // если текущее отклонения значения левого датчика от исходного более, чем на 75 и в отрицательную сторону, значит левое колесо попало на черную линию

      //робот трогается с остановки (черная поперечная полоса), значит как только хотя бы одно колесо попало на белое,
      //то перекресток пройден
      if (right || left)
      {
        runStop(); // останавливаем робота
        return;
      }
    }
}

void check_state(bool is_forward)
/*
  Функция проверки текущего состояния робота при движении.
  Выполняется анализ текущего значения датчиков и принятие решение:
  1. продолжение движения прямо
  2. поворот налево
  3. поворот направо
  4. остановка
*/
{
      boolean left, right = 0; // флаг того, находится ли левое и правое колесо робота на черной полосе (1) или на белой (0)
      int cur_left, cur_right; // текущее значение на датчиках, левом и правом соответственно
      int ldelta, rdelta;  //дельта между исходным (белым!) значением полотона и текущим,  левая и правая соответственно
  
      cur_left = uAnalogRead(LEFT_SENSOR); // считываем текущее значение с левого датчика
      cur_right = uAnalogRead(RIGHT_SENSOR); // считываем текущее значение с правого датчика
      
      //Serial.print ("first left:   "); Serial.print(first_left_state); Serial.print ("  "); Serial.print ("Left: "); Serial.print (cur_left); Serial.print ("  ");
      //Serial.print ("first right:   "); Serial.print(first_right_state); Serial.print ("  "); Serial.print ("Right: "); Serial.print (cur_right);

      ldelta = cur_left - first_left_state;
      rdelta = cur_right - first_right_state;

      right = rdelta < -BLACK_THRESHOLD; // если текущее отклонения значения правого датчика от исходного более, чем на 75 и в отрицательную сторону, значит правое колесо попало на черную линию
      left = ldelta < -BLACK_THRESHOLD; // если текущее отклонения значения левого датчика от исходного более, чем на 75 и в отрицательную сторону, значит левое колесо попало на черную линию
      
      //Serial.print("    ldelta: "); Serial.print(ldelta);
      //Serial.print("    rdelta: "); Serial.print(rdelta);
      //Serial.print("    ");
      if (left == right) {
        if (left && right)
        {
          targetState = STATE_CROSSROAD;
          // наехали на перекресток
          //Serial.println ("CROSSROAD");
        }
        else
        {
          targetState = is_forward? STATE_FORWARD: STATE_BACK;
          //          Serial.println ("FORWARD");
        }
      } else if (left) {
          targetState = is_forward? STATE_LEFT: STATE_RIGHT;
          //          Serial.println ("LEFT");
      } else {
          targetState = is_forward? STATE_RIGHT: STATE_LEFT;
          //          Serial.println ("RIGHT");
      }
}

void pour_liquid_ml(int volume_ml)
{  
    int volume_limit_ml = volume_left_ml - volume_ml;
    int volume_limit_units = volume_limit_ml * ten_ml_units/MIN_VOLUME_ML; // (MIN_VOLUME_ML = 10 ml)
    open_valve();
    while (get_water_sensor_value() > volume_limit_units) ;
    close_valve();
    volume_left_ml = get_water_sensor_value() * MIN_VOLUME_ML / ten_ml_units;
}

void open_valve()
{
    uDigitalWrite(VALVE, HIGH); // закрывается транзистор высоким уровнем, клапан открыт
}

void close_valve()
{
    uDigitalWrite(VALVE, LOW); //транзистор, управляющий клапаном, открывается низким уровнем, клапан закрыт
}

int get_water_sensor_value ()
{
    return uAnalogRead(WATER_LEVEL_SENSOR);
}
