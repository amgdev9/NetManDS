/**
 * @file SnmpAgentScanner.h
 * @brief SNMP Agent Discovery
 */

// Includes C/C++
#include <memory>
#include <unordered_map>

// Own includes
#include "snmp/Snmpv2Pdu.h"
#include "asn1/BerInteger.h"

// Defines
#define SNMPAGENT_NOID  7
#define SNMPAGENT_REQID 100

namespace NetMan {

/**
 * @struct SnmpAgentEntry
 */
typedef struct {
    std::string sysDescr;
    std::string sysObjectID;
    u32 sysUpTime;
    std::string sysContact;
    std::string sysName;
    std::string sysLocation;
    u32 sysServices;
} SnmpAgentEntry;

/**
 * @class SnmpAgentScanner
 */
class SnmpAgentScanner {
    private:
        std::shared_ptr<UdpSocket> sock;
        std::shared_ptr<BerOid> oid[SNMPAGENT_NOID];
        std::shared_ptr<BerNull> nullVal;
        std::unordered_map<in_addr_t, SnmpAgentEntry> agents;
        std::string &getStringFromVarBind(std::shared_ptr<Snmpv1Pdu> pdu, u8 i);
        std::shared_ptr<std::string> getOidFromVarBind(std::shared_ptr<Snmpv1Pdu> pdu, u8 i);
        std::shared_ptr<BerInteger> getIntegerFromVarBind(std::shared_ptr<Snmpv1Pdu> pdu, u8 i);
    public:
        SnmpAgentScanner();
        void scanAgents(in_addr_t baseIP, u16 nhosts, u16 port, u8 version, u8 maxRequests, u8 timeout);
        void print();
        virtual ~SnmpAgentScanner();
};

}
