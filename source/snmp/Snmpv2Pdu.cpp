/**
 * @file Snmpv2Pdu.cpp
 * @brief SNMPv2c PDU handler
 */

// Own includes
#include "asn1/BerInteger.h"
#include "snmp/Snmpv2Pdu.h"

namespace NetMan {

/**
 * @brief Constructor for a SNMPv2 PDU
 * @param community Community to be used
 */
Snmpv2Pdu::Snmpv2Pdu(const std::string &community) : Snmpv1Pdu(community) {
    this->snmpVersion = SNMPV2_VERSION;
}

/**
 * @brief Generate a SNMPv2 bulk request PDU
 * @param nonRepeaters      Number of scalar objects to fetch
 * @param maxRepetitions    Number of rows to get for the remaining objects
 * @return The generated PDU (without its header)
 */
std::shared_ptr<BerSequence> Snmpv2Pdu::generateBulkRequest(u32 nonRepeaters, u32 maxRepetitions) {

	std::shared_ptr<BerSequence> getBulkRequest = std::make_shared<BerSequence>(SNMPV1_TAGCLASS, SNMPV2_GETBULKREQUEST);

	this->reqID = ++Snmpv1Pdu::requestID;
	std::shared_ptr<BerInteger> reqid = std::make_shared<BerInteger>(&this->reqID, sizeof(u32), false);
	std::shared_ptr<BerInteger> nrField = std::make_shared<BerInteger>(&nonRepeaters, sizeof(u32), false);
	std::shared_ptr<BerInteger> mrField = std::make_shared<BerInteger>(&maxRepetitions, sizeof(u32), false);
	getBulkRequest->addChild(reqid);				// RequestID
	getBulkRequest->addChild(nrField);				// Non repeaters
	getBulkRequest->addChild(mrField);	    		// Max repetitions
	getBulkRequest->addChild(this->varBindList);	// Elements to ask for

	return getBulkRequest;
}

/**
 * @brief Send a SNMPv2 GetBulk request
 * @param nonRepeaters      Number of scalar objects to fetch
 * @param maxRepetitions    Number of rows to get for the remaining objects
 * @param sock              Socket used for transmission
 * @param ip                Destination IP address
 */
void Snmpv2Pdu::sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

	try {

        if(this->varBindList == nullptr) {
		    throw std::runtime_error("Empty VarBindList");
	    }

		std::shared_ptr<BerSequence> message = this->generateHeader(this->snmpVersion);

		// GetBulkRequest PDU
		std::shared_ptr<BerSequence> getBulkRequest = this->generateBulkRequest(nonRepeaters, maxRepetitions);
		message->addChild(getBulkRequest);

		this->fields.push_back(message);
		BerPdu::send(sock, ip, SNMP_PORT);

	} catch (const std::bad_alloc &e) {
		this->fields.clear();
		throw;
	} catch (const std::runtime_error &e) {
		this->fields.clear();
		throw;
	}

	// Clear data
	this->fields.clear();
	this->varBindList.reset();
}

/**
 * @brief Receive a SNMPv2 trap or inform-request (acknowledged trap)
 * @param sock Socket used for reception
 */
void Snmpv2Pdu::recvTrap(std::shared_ptr<UdpSocket> sock) {

    try {

        // Receive a trap or inform-request PDU
        u8 pduType = this->recvResponse(sock, "", 0, SNMP_PDU_ANY);

        // Error if not a notification was received
        if(pduType != SNMPV2_TRAP && pduType != SNMPV2_INFORMREQUEST) {
            throw std::runtime_error("This is not a notification PDU");
        }

        // If it was an inform-request, send the acknowledgement
        if(pduType == SNMPV2_INFORMREQUEST) {
            this->sendRequest(SNMPV2_GETRESPONSE, sock, "");    // Use inform-request origin IP-port as destination IP-port
        }
    } catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Destructor for a SNMPv2 PDU
 */
Snmpv2Pdu::~Snmpv2Pdu() { }

}
