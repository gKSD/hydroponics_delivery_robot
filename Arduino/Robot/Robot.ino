  // Подключим библиотеку для работы с I2C-расширителем портов
#include <Wire.h>
// Подключим библиотеку Strela
#include <Strela.h>
#include <Ultrasonic.h>


// echo будет общим для всех 4 дальномеров, запускается один дальномер (по trig), результат считывается с общего входа
//#define ECHO P2
#define ECHO0 P3 //forward
#define ECHO1 P4 //right
#define ECHO2 P5 //back
#define ECHO3 P1 //left
#define TRIG0 P6 //forward
#define TRIG1 P8 //right
#define TRIG2 P9 //back
#define TRIG3 P7 //left

//датчики линиии
#define LEFT_SENSOR P12
#define RIGHT_SENSOR P11
#define BLACK_THRESHOLD 25  // величина отклонения показаний датчиков на черном поле относительно белого поля

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
//********************************************************************************************************************************************************************************************************


Ultrasonic ultrasonic0(TRIG0, P3); // (Trig PIN,Echo PIN)
Ultrasonic ultrasonic1(TRIG1, P4); // (Trig PIN,Echo PIN)
Ultrasonic ultrasonic2(TRIG2, P5); // (Trig PIN,Echo PIN)
Ultrasonic ultrasonic3(TRIG3, P1);

const char US_FORWARD = 'f', US_BACK = 'b', US_RIGHT = 'r', US_LEFT = 'l';

int state = STATE_BASE; // !!! Обязательно в начальный момент времени робот должен быть установлен на базу, датчики линии при этом должны находиться над зазором между базой и линий дороги
int first_left_state = 0;
int first_right_state = 0;

int currentSpeed = 200;
//int crossroads_counter = 0;

int straight_crossroads_counter = 0;
int left_crossroads_counter = 0;
int right_crossroads_counter = 0;

int required_straight_crossroads_amount = 0;
int required_left_crossroads_amount = 0;
int required_right_crossroads_amount = 0;

int targetState; // текущее состояние в зависимости от положение колес
int ustate = -1;

#define MAX_ROAD_OBJECT 256
char passed_road_objects [MAX_ROAD_OBJECT]; //массив для хранения перекрестков ('+') и поворотов ('<', '>'), для того, чтобы можно было вернуться домой
int passed_road_objects_len = 0;

bool timeout = false;
int interrupts_counter = 0;
int interrupts_counter_max = 30;

void setup() 
{
    sei();//разрешаем прерывания глобально
    // Настройка последовательного протокола UART для взаимодействия с Rasberry Pi
    Serial1.begin(9600);
    while (!Serial1)
      ; // wait for serial port to connect. Needed for Leonardo only
    
    Serial.begin(9600); // Инициализируем сериал порт для вывода результата 
    delay(10000);
    get_sensors_start_state ();
}

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

// ***************************************************************** Таймер (для таймаута) *****************************************************************************
#define TIMER_RESOLUTION 65535 // максимальное число, которое может быть записано в регистр таймера TCNT1
#define PRESCALER 1024
#define TIMER_SET_VALUE 4
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
  if (interrupts_counter < interrupts_counter_max) reset_timer_counter(TIMER_SET_VALUE);
  else
  {
    ustate = USTATE_RETURN_TO_BASE;
    timeout = true;
  }
}

void process_commands()
/*
  функция обработки длительных команд, например, движение вперед, которе выполняется не за один "такт", при этом необходимо постоянно опрашивать последовательный канал
*/
{
   switch(ustate)
   {
     case USTATE_RUN_FORWARD:
         if (!run_forward_with_stabilization(false))
           ustate = USTATE_ERROR_SAVING_ROAD;
         break;
      case USTATE_RUN_FORWARD_TILL_CROSSROAD:
          if (!run_forward_with_stabilization(true))
              ustate = USTATE_ERROR_SAVING_ROAD;
          break;
      case USTATE_RETURN_TO_BASE:
          //return_to_base();
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
          //здесь запускаем таймаут на ожидание ответа от управдяющего устройства. По таймауту - возвращение на базу
          timeout = false;
          interrupts_counter = 0;
          interrupts_counter_max = 30;
          set_timeout(TIMER_SET_VALUE, wait_answer_timeout); // таймаут на 120 сек
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
        ustate = USTATE_RUN_FORWARD_TILL_CROSSROAD;
        // отправка подтверждения о получении команды
        Serial.print(UART_RUN_FORWARD_STARTED); //1
        params[i] = '\0';
        required_straight_crossroads_amount = atoi(params);
        Serial.print("Crossroads amount: "); Serial.println(required_straight_crossroads_amount);
        straight_crossroads_counter = 0;
        if (!run_forward_with_stabilization(true))
            ustate = USTATE_ERROR_SAVING_ROAD;
    }
    else if (command == UART_RUN_FORWARD ) //Формат кадра: <код команды> *
    {
        ustate = USTATE_RUN_FORWARD;
        // отправка подтверждения о получении команды  
        Serial.print(UART_RUN_FORWARD_STARTED); //1
        if(!run_forward_with_stabilization(false)) 
            ustate = USTATE_ERROR_SAVING_ROAD;
    }
    else if (command == UART_TURN_LEFT )
    {
        if (passed_road_objects_len < MAX_ROAD_OBJECT - 1)
        {
           passed_road_objects[passed_road_objects_len] = '<'; // сохраняем, что проехали перекресток
           passed_road_objects_len++;
           turnInPlace(true);
           ustate = -1;
        }
        else
           ustate = USTATE_ERROR_SAVING_ROAD;
    }
    else if (command == UART_TURN_RIGHT )
    {
      if (passed_road_objects_len < MAX_ROAD_OBJECT - 1)
        {
           passed_road_objects[passed_road_objects_len] = '>'; // сохраняем, что проехали перекресток
           passed_road_objects_len++;
           turnInPlace(false);
           ustate = -1;
        }
        else
           ustate = USTATE_ERROR_SAVING_ROAD;
    }
    else if (command == UART_TURN_AROUND)
    {
       turnAround();
       //проедем вперед для выравнивания после разворота
       if(!run_forward_with_stabilization(false))
       ustate = USTATE_ERROR_SAVING_ROAD;
       delay (300);
       runStop();
       ustate = -1;
    }
    else if (command == UART_POUR_LIQUID_ML )
    {
        ustate = -1;
    }
    else if (command == UART_POUR_ALL_LIQUID )
    {
        ustate = -1;
    }
    else if (command == UART_RETURN_TO_BASE )
    {
        ustate = USTATE_RETURN_TO_BASE;
    }
    else if (command == UART_GET_CORRENT_POSITION )
    {
        ustate = -1;
    }
    else if (command == UART_GET_BACK_SONAR_VALUE )
    {
        ustate = -1;
    }
    else if (command == UART_GET_FORWARD_SONAR_VALUE )
    {
        ustate = -1;
    }
    else if (command == UART_GET_ALL_SONARS_VALUE )
    {
        ustate = -1;
    }
    else if (command == UART_GET_LEFT_SENSOR_VALUE )
    {
        ustate = -1;
    }
    else if (command == UART_GET_RIGHT_SENSOR_VALUE )
    {
        ustate = -1;
    }
    else if (command == UART_GET_ALL_SENSOR_VALUE )
    {
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
        ustate = -1;
    }
    else if (command == UART_TURN_CAMERA_LEFT )
    {
        ustate = -1;
    }
    else if (command == UART_TURN_CAMERA_FORWARD )
    {
        ustate = -1;
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
         if(!run_forward_with_stabilization(false))
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

void runForward()
{
    uDigitalWrite(L1, LOW);
    uDigitalWrite(L2, HIGH);
    uDigitalWrite(L3, HIGH);
    uDigitalWrite(L4, LOW);
 
    drive (currentSpeed, currentSpeed);
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

void steerRight() 
{
    state = STATE_RIGHT;
    uDigitalWrite(L1, LOW);
    uDigitalWrite(L2, LOW);
    uDigitalWrite(L3, LOW);
    uDigitalWrite(L4, HIGH);
      
    drive (0, currentSpeed + 20);
}

void steerLeft() 
{
    state = STATE_LEFT;
    uDigitalWrite(L1, HIGH);
    uDigitalWrite(L2, LOW);
    uDigitalWrite(L3, LOW);
    uDigitalWrite(L4, LOW);

    drive(currentSpeed + 20, 0);
}

void turnInPlace(int turn_left)
/*
  функция поворота робота (на гусеничной платформе) на одном месте.
  Входной параметр определяет направление поворота: turn_left == 1 => налево, turn_left == 0 => направо
*/
{
  Serial.println("***************************************** turnInPlace *********************************************** ");
    if (turn_left)
        drive (currentSpeed, -currentSpeed);
    else 
        drive (-currentSpeed, currentSpeed);
    delay(150);
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
        run_forward_with_stabilization(false); // проехали для выравнивания после поворота
        delay (200);
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

boolean run_forward_with_stabilization(bool need_crossroads)
/*
  прямое движение вперед, пока не случится таймаут или не придет команда остановится или сменить направление
*/
{
      int distance_forward = interrogate_ultrasonics (US_FORWARD);
      if (distance_forward <= 10)
      {
        // останавливаем движение
        // меняем статус 
        // отправляем сообщение об ошибке - достигнут тупик
        runStop();
        ustate = USTATE_ERROR_DEADLOCK;
        Serial.print(UART_REACH_DEADLOCK_ERROR); //1
        return true;
      }

      check_state(true); // for forward ride
      
      switch (targetState) {
          case STATE_FORWARD:
              runForward(); //движение прямо
              break;
  
          case STATE_RIGHT:
              steerRight(); //поворот направо
              break;
  
          case STATE_LEFT:
              steerLeft(); //поворот налево
              break;
              
          case STATE_CROSSROAD:
            if (need_crossroads)
            {
              if (straight_crossroads_counter == required_straight_crossroads_amount)
              {
                  runStop();
                  ustate = USTATE_PASSED_CROSSROADS;
                  //выполнили команду, проехали определенное количество перекрестков
              }
              else
              {
                  straight_crossroads_counter++;
              }
            }
            if (passed_road_objects_len < MAX_ROAD_OBJECT - 1)
            {
                passed_road_objects[passed_road_objects_len] = '+'; // сохраняем, что проехали перекресток
                passed_road_objects_len++;
            }
            else
            {
              return false;
            }
            pass_crossroad();
            break;

          case STATE_STOP:
              break;
      }
      return true;
}

void pass_crossroad()
{
    runForward(); //начинаем движение вперед
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

