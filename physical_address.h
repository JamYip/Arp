#include <string>
using namespace std;

class PhysicalAddress
{
public:
    static PhysicalAddress Parse(const string& addr);
    virtual ~PhysicalAddress();
    void Serialize(char* mac_hex);

private:
    PhysicalAddress(const string& addr);
    static bool IsVaild(const string& addr);

private:
    string addr_;
};