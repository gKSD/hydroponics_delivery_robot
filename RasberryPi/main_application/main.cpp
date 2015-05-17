#include <iostream>
#include <vector>
#include <string>

#include "route_processor.hpp"
#include "common.hpp"
#include "serial.hpp"
#include "error_codes.hpp"

using namespace std;

int main (int argc, char **argv)
{
    Serial serial;
    serial.set_device("1111");
    serial.set_device("asdasd");
    try
    {
        serial.serial_open("/dev/ttyAMA0", 9600);
        cout << "111" << endl;
        serial.uart_setup();
        cout << "000" << endl;
        bool test = true;
        while (test)
        {
            string ss;
            cout << "Enter string: ";
            cin >> ss;
            cout <<"Your string is: " << ss << endl;
            serial.serial_puts((char *)ss.c_str());
            if (ss.compare("end") == 0)
            {
                break;
            }
        }
        serial.serial_close();
        cout << "222" << endl;
    }
    catch(int e)
    {
        if (e == SERIAL_OPEN_ERROR)
        {
            cout << "ERROR: serial open" << endl;
            return -1;
        }
    }

    vector<char *> garden_beds = {(char *)"C1", (char *)"B2",  (char *)"A4"};
    try
    {
        Route_processor proc(1);
        route_vector vec = proc.run(garden_beds);
        for (int i = 0; i < vec.size(); i++)
        {
            cout << "item => "<< vec[i]->state << endl;
        }

        for (int i = i; i < vec.size(); i++)
        {
            delete vec[i];
        }
    }
    catch(int)
    {
        cout << "error detected" << endl;
    }
    return 1;
}
