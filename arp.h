#include <string>
using namespace std;
class Arp
{
public:
    Arp(const string& net_interface);
    virtual ~Arp();
    bool SendRequest(const string& sender_mac, const string& sender_ip,
                const string& target_mac, const string& target_ip);
    //bool SendReply();

private:
    std::string net_interface_;
    static void SerializeMacStr(const string& mac_str, char* mac_hex);
    string GetLocalMac();
};