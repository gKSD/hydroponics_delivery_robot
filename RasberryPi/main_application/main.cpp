#include <iostream>
#include <vector>


#include "route_processor.hpp"
#include "common.hpp"
using namespace std;

int main (int argc, char **argv)
{
    vector<char *> garden_beds = {(char *)"C1", (char *)"B2",  (char *)"A4"};
    try
    {
        Route_processor proc(-1);
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
