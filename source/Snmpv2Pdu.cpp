/**
 * @file Snmpv1Pdu.cpp
 * @brief SNMPv1 PDU handler
 */

// Own includes
#include "Snmpv2Pdu.h"

namespace NetMan {

/**
 * @brief Constructor for a SNMPv2 PDU
 * @param community Community to be used
 */
Snmpv2Pdu::Snmpv2Pdu(const std::string &community) : Snmpv1Pdu(community) { }

/**
 * @brief Send a SNMPv2 request
 * @param type  Any of the SNMPV2_*REQUEST defines
 * @param sock  Socket used for transmission
 * @param ip    Destination IP address
 */
void Snmpv2Pdu::sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

}

/**
 * @brief Send a SNMPv2 GetBulk request
 * @param nonRepeaters      Number of scalar objects to fetch
 * @param maxRepetitions    Number of rows to get for the remaining objects
 * @param sock              Socket used for transmission
 * @param ip                Destination IP address
 */
void Snmpv2Pdu::sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

}

/**
 * @brief Receive a SNMPv2 response
 * @param sock  Socket used for reception
 * @param ip    Expected source IP
 */
void Snmpv2Pdu::recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip) {

}

/**
 * @brief Receive a SNMPv2 trap or inform-request (acknowledged trap)
 * @param sock Socket used for reception
 */
void Snmpv2Pdu::recvTrap(std::shared_ptr<UdpSocket> sock) {

}

/**
 * @brief Destructor for a SNMPv2 PDU
 */
Snmpv2Pdu::~Snmpv2Pdu() { }

}
