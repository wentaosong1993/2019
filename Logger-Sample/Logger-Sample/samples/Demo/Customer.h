#pragma once
#include <string>
#include <iostream>
#include <plog/Log.h>

struct Customer
{
    int id;
    std::string name;
    ~Customer(){std::cout << "destructor" << std::endl;LOGD << "destructor"; }
};


inline std::ostream& operator<<(std::ostream& os, const Customer& obj)
{
    os << "Customer (id: " << obj.id << ", name: " << obj.name << ")";
    return os;
}
