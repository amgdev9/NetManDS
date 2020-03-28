/**
 * @file SnmpAgentScanner.cpp
 * @brief SNMP Agent Discovery
 */

// Includes C/C++
#include <arpa/inet.h>
#include <typeinfo>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "snmp/SnmpAgentScanner.h"

namespace NetMan {

SnmpAgentScanner::SnmpAgentScanner() {
    sock = std::make_shared<UdpSocket>(0, 0);

    for(int i = 0; i < SNMPAGENT_NOID; i++) {
        oid[i] = std::make_shared<BerOid>("1.3.6.1.2.1.1." + std::to_string(i + 1) + ".0");
    }

    nullVal = std::make_shared<BerNull>();
    agents = std::unordered_map<in_addr_t, SnmpAgentEntry>();
}

void SnmpAgentScanner::scanAgents(in_addr_t baseIP, u16 nhosts, u16 port, u8 version, u8 maxRequests, u8 timeout) {
    
    // Initialize agents map
    this->agents.clear();

    // Create the SNMP PDU
    std::shared_ptr<Snmpv1Pdu> pdu = nullptr;
    const static std::string community = "public";
    if(version == SNMPV1_VERSION) {
        pdu = std::make_shared<Snmpv1Pdu>(community);
    } else {
        pdu = std::make_shared<Snmpv2Pdu>(community);
    }

    // Check every host
    u16 checkedHosts = 0;
    while(checkedHosts < nhosts) {
        u16 hostsToCheck = std::min<int>(maxRequests, nhosts - checkedHosts);

        // Send a PDU to every host in the block
        for(u16 i = 0; i < hostsToCheck; i++) {
            
            try {
                for(u8 j = 0; j < 7; j++) {
                    pdu->addVarBind(oid[j], nullVal);
                }

                try {
                    Snmpv1Pdu::setGlobalRequestID(SNMPAGENT_REQID);
                    pdu->sendRequest(SNMPV2_GETREQUEST, sock, ntohl(htonl(baseIP) + checkedHosts + i), port);
                } catch (const std::bad_alloc &e) {
                    throw;
                } catch (const std::runtime_error &e) {
                    // ARP can fail here, it doesn't matter
                }

                pdu->clear();
            } catch (const std::bad_alloc &e) {
                throw;
            }
        }

        // Update counters
        checkedHosts += hostsToCheck;

        // Wait timeout (seconds)
        for(u32 i = 0; i < timeout * 60; i++) {
            gspWaitForVBlank();
        }

        // Receive responses
        Snmpv1Pdu::setGlobalRequestID(SNMPAGENT_REQID);
        for(u16 i = 0; i < hostsToCheck; i++) {
            try {
                pdu->recvResponse(sock, INADDR_ANY, port);

                // Save the agent
                SnmpAgentEntry agent;
                agent.sysDescr = this->getStringFromVarBind(pdu, 0);
                agent.sysObjectID = *this->getOidFromVarBind(pdu, 1).get();
                agent.sysUpTime = this->getIntegerFromVarBind(pdu, 2)->getValueU32();
                agent.sysContact = this->getStringFromVarBind(pdu, 3);
                agent.sysName = this->getStringFromVarBind(pdu, 4);
                agent.sysLocation = this->getStringFromVarBind(pdu, 5);
                agent.sysServices = this->getIntegerFromVarBind(pdu, 6)->getValueS32();
                this->agents[sock->getLastOrigin()] = agent;
            } catch (const std::bad_alloc &e) {
                throw;
            } catch (const std::runtime_error &e) {
                // No PDU has been received, exit the loop
                i = hostsToCheck;
            }
        }
    }
}

std::string &SnmpAgentScanner::getStringFromVarBind(std::shared_ptr<Snmpv1Pdu> pdu, u8 i) {
    std::shared_ptr<BerField> varBind = pdu->getVarBind(i);
    if(typeid(*varBind.get()) == typeid(BerOctetString)) {
        return std::static_pointer_cast<BerOctetString>(varBind)->getValue();
    }

    throw std::runtime_error("Error retrieving OCTET STRING");
}

std::shared_ptr<std::string> SnmpAgentScanner::getOidFromVarBind(std::shared_ptr<Snmpv1Pdu> pdu, u8 i) {
    std::shared_ptr<BerField> varBind = pdu->getVarBind(i);
    if(typeid(*varBind.get()) == typeid(BerOid)) {
        return std::static_pointer_cast<BerOid>(varBind)->get();
    }

    throw std::runtime_error("Error retrieving OID");
}

std::shared_ptr<BerInteger> SnmpAgentScanner::getIntegerFromVarBind(std::shared_ptr<Snmpv1Pdu> pdu, u8 i) {
    std::shared_ptr<BerField> varBind = pdu->getVarBind(i);
    if(typeid(*varBind.get()) == typeid(BerInteger)) {
        return std::static_pointer_cast<BerInteger>(varBind);
    }

    throw std::runtime_error("Error retrieving INTEGER");
}

void SnmpAgentScanner::print() {
    FILE *f = fopen("log.txt", "a+");
    fprintf(f, "SNMP Agent Discovery: %d\n", this->agents.size());
    in_addr addr;
    for (auto agent : this->agents) {
        SnmpAgentEntry &entry = agent.second;
        addr.s_addr = agent.first;
	    fprintf(f, "\nAgent at %s\n", inet_ntoa(addr));
        fprintf(f, "\tsysDescr: %s\n", entry.sysDescr.c_str());
        fprintf(f, "\tsysObjectID: %s\n", entry.sysObjectID.c_str());
        fprintf(f, "\tsysUpTime: %ld\n", entry.sysUpTime);
        fprintf(f, "\tsysContact: %s\n", entry.sysContact.c_str());
        fprintf(f, "\tsysName: %s\n", entry.sysName.c_str());
        fprintf(f, "\tsysLocation: %s\n", entry.sysLocation.c_str());
        fprintf(f, "\tsysServices: %ld\n", entry.sysServices);
    }
    fclose(f);
}

SnmpAgentScanner::~SnmpAgentScanner() { }

}
