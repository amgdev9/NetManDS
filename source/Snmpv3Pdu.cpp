/**
 * @file Snmpv3Pdu.cpp
 * @brief SNMPv3 PDU handler
 */

// Own includes
#include "Snmpv3Pdu.h"

/**
 * @brief Constructor for a Snmpv3 PDU
 */
Snmpv3Pdu::Snmpv3Pdu() {

}

void Snmpv3Pdu::sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

}

u8 Snmpv3Pdu::recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip, u32 expectedPduType = SNMPV1_GETRESPONSE) {

}

void Snmpv3Pdu::sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

}

void Snmpv3Pdu::recvTrap(std::shared_ptr<UdpSocket> sock) {

}

/**
 * @brief Destructor for a SNMPv3 PDU
 */
Snmpv3Pdu::~Snmpv3Pdu() {

}
