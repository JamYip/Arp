#include "physical_address.h"

#include <ctype.h>

PhysicalAddress PhysicalAddress::Parse(const string& addr)
{
    if (!IsVaild(addr))
    {
        throw;
    }
    else
    {
        return PhysicalAddress(addr);
    }
}

PhysicalAddress::PhysicalAddress(const string& addr) : addr_(addr)
{

}

PhysicalAddress::~PhysicalAddress()
{

}

void PhysicalAddress::Serialize(char* mac_hex)
{
        sscanf(addr_.c_str(), "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
        mac_hex, mac_hex + 1, mac_hex + 2, mac_hex + 3,
        mac_hex + 4, mac_hex + 5);
}

bool PhysicalAddress::IsVaild(const string& addr)
{
    // mac addr: xx:xx:xx:xx:xx:xx
    if (addr.length() != 17)
    {
        return false;
    }
    for (int i = 0; i < addr.length(); i++)
    {
        if (((i + 1) % 3 == 0) && (addr.at(i) != ':'))
        {
            return false;
        }
        if (((i + 1) % 3 != 0) && (!isxdigit(addr.at(i))))
        {
            return false;
        }
    }
    return true;
}