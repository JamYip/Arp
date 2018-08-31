#include "arp.h"

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

#include "physical_address.h"

using namespace std;

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
        Operation = htons(operation);
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
        PhysicalAddress::Parse(sender_mac).Serialize(SenderMacAddr);
        PhysicalAddress::Parse(target_mac).Serialize(TargetMacAddr);
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
        Type = htons(ETH_P_ARP);
        Crc = 0;
        PhysicalAddress::Parse(dest_addr).Serialize(DestinationAddr);
        PhysicalAddress::Parse(src_addr).Serialize(SourceAddr);
        memcpy(Data, data, sizeof(Data));
    }
};

Arp::Arp(const string& net_interface) : net_interface_(net_interface)
{

}

Arp::~Arp()
{

}

bool Arp::SendRequest(const string& sender_mac,
                const string& sender_ip,
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
    device.sll_ifindex = if_nametoindex (net_interface_.c_str());
    device.sll_family = AF_PACKET;
    device.sll_halen = 6;

    // send packet
    ArpMsg arp_msg(2, sender_mac, sender_ip, target_mac, target_ip);
    ArpEthernetFrame frame(target_mac, GetLocalMac(), &arp_msg);
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

string Arp::GetLocalMac()
{
    string file_path = "/sys/class/net/" + net_interface_ + "/address";
    ifstream mac_file(file_path);
    string mac_addr;
    mac_file >> mac_addr;
    return mac_addr;
}