#include <iostream>
#include <string>
#include <unistd.h>

#include "arp.h"

using namespace std;

void SplitArg(const string& src_arg, string& arg1, string& arg2);

int main(int argc, char* argv[])
{
    string net_if;
    string target_ip;
    string target_mac;
    string sender_ip;
    string sender_mac;
    int opt = -1;
    while((opt = getopt(argc, argv, "i:t:s:")) != -1)
    {
        switch(opt)
        {
        case 'i':
            net_if = optarg;
            break;
        case 't':
            SplitArg(string(optarg), target_ip, target_mac);
            break;
        case 's':
            SplitArg(string(optarg), sender_ip, sender_mac);
            break;
        default:
            break;
        }
    }

    Arp arper(net_if);
    arper.SendRequest(sender_mac, sender_ip, target_mac, target_ip);
    return 0;
}

void SplitArg(const string& src_arg, string& arg1, string& arg2)
{
    auto n = src_arg.find('/');
    if (std::string::npos == n)
    {
        throw;
    }
    
    arg1 = src_arg.substr(0, n);
    arg2 = src_arg.substr(n + 1);
}