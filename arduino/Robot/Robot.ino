// Подключим библиотеку для работы с I2C-расширителем портов
#include <Wire.h>
// Подключим библиотеку Strela
#include <Strela.h>

// echo будет общим для всех 4 дальномеров, запускается один дальномер (по trig), результат считывается с общего входа
#define ECHO P8
#define TRIG0 P7

unsigned int distance_sm=0; // Переменная для хранения расстояния в сантиметрах

void setup() 
{
    // настройка пинов Trig - на вывод, Echo - на ввод (для ультразвуковго дальномера)
    pinMode(TRIG0, OUTPUT);
    pinMode(ECHO, INPUT);
    
    // Настройка последовательного протокола UART для взаимодействия с Rasberry Pi
    Serial1.begin(9600);
    while (!Serial1)
      ; // wait for serial port to connect. Needed for Leonardo only
    
    Serial.begin(9600); // Инициализируем сериал порт для вывода результата 
}

void loop() 
{
    Serial1.println ("Hello Rasberry");
    //Serial1.print ("a");
    //if (Serial1.available() > 0) 
    //  Serial.println(Serial1.read());
    delay (1000);
}


void test_trigger () // тестовая функция - проверка работы дальномера
{
    distance_sm = get_trig_distance (TRIG0, ECHO, 10);
    Serial.print(distance_sm); // Выводим на порт
    Serial.print(" ");
    delay(500); 
}

unsigned int get_trig_distance (int trig, int echo, int echo_delay)
/*
 * Функция для опроса триггера ультразвукового дальномера
 */
{
    uDigitalWrite(trig, HIGH); // Подаем сигнал на выход микроконтроллера
    delayMicroseconds(echo_delay); // Удерживаем 10 микросекунд
    uDigitalWrite(trig, LOW); // Затем убираем
    unsigned int time_us = pulseIn(echo, HIGH); // Переменная для хранения временного интервала. Замеряем длину импульса
    return time_us/58; // Пересчитываем в сантиметры, из даташита: s=t/58
}
