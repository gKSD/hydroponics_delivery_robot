#ifndef __SINGLETON_HPP
#define __SINGLETON_HPP

#include <queue>
#include <string>


class Singleton
{
protected:
    static Singleton* _self;
    Singleton() {}
    virtual ~Singleton() {}

public:
    static Singleton* Instance()
    {
        if(!_self)
        {
            _self = new Singleton();
        }
        return _self;
    }

    static bool DeleteInstance()
    {
        if(_self)
        {
            delete _self;
            _self = 0;
            return true;
        }
        return false;
    }

public:
    std::queue<std::string> messages_for_django;
    std::queue<std::string> messages_for_base;
    std::queue<std::string> messages_for_aplication;
};

//Singleton* Singleton ::_self = 0;

#endif