/**
 * @file Snmpv1Pdu.cpp
 * @brief SNMPv1 PDU handler
 */

// Includes C/C++
#include <stdio.h>
#include <string.h>
#include <stdexcept>

// Own includes
#include "Snmpv1Pdu.h"
#include "asn1/BerInteger.h"
#include "asn1/BerOctetString.h"

namespace NetMan {

// Static data
u32 Snmpv1Pdu::requestID = 0;

/**
 * @brief Constructor for a SNMPv1 PDU
 * @param community SNMPv1 community
 */
Snmpv1Pdu::Snmpv1Pdu(const std::string &community) { 

	this->varBindList = nullptr;
	this->reqID = 0;
	this->community = community;
}

/**
 * @brief Clear the PDU for further usage
 */
void Snmpv1Pdu::clear() {
	BerPdu::clear();		// Varbindlist can't be here
	this->varBindList.reset();
}

/**
 * @brief Add a field to the PDU
 * @param oid Object identifier of the field
 * @param value Value for that field (optional if requesting)
 */
void Snmpv1Pdu::addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value) {

	std::shared_ptr<BerSequence> varBind = nullptr;
	try {
		if(this->varBindList == nullptr) this->varBindList = std::make_shared<BerSequence>();

		varBind = std::make_shared<BerSequence>();
	} catch (const std::bad_alloc &e) {
		throw;
	}
	varBind->addChild(oid);
	varBind->addChild(value);

	this->varBindList->addChild(varBind);
}

/**
 * @brief Generate a PDU header
 * @param ver	SNMP version to use
 * @return The root sequence of the PDU
 */
std::shared_ptr<BerSequence> Snmpv1Pdu::generateHeader(u8 ver) {

	try {
		std::shared_ptr<BerSequence> message = std::make_shared<BerSequence>();

		// Header (common to all SNMP PDUs)
		std::shared_ptr<BerInteger> version = std::make_shared<BerInteger>(ver);
		std::shared_ptr<BerOctetString> community = std::make_shared<BerOctetString>(this->community);
		message->addChild(version);
		message->addChild(community);
		// From here it can be PDUs or authentication data

		return message;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Send a GET REQUEST
 * @param type Type of SNMP PDU
 * @param socket Socket used when sending the PDU
 * @param ip Destination IP
 */
void Snmpv1Pdu::sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

	if(this->varBindList == nullptr) {
		throw std::runtime_error("Empty VarBindList");
	}

	try {
		std::shared_ptr<BerSequence> message = this->generateHeader(SNMPV1_VERSION);

		// GetRequest PDU
		std::shared_ptr<BerSequence> getRequest = std::make_shared<BerSequence>(SNMPV1_TAGCLASS, type);
		message->addChild(getRequest);

		this->reqID = ++Snmpv1Pdu::requestID;
		std::shared_ptr<BerInteger> reqid = std::make_shared<BerInteger>(this->reqID);
		std::shared_ptr<BerInteger> error = std::make_shared<BerInteger>(SNMPV1_ERROR_NOERROR);
		std::shared_ptr<BerInteger> errorDetails = std::make_shared<BerInteger>(0);
		getRequest->addChild(reqid);				// RequestID
		getRequest->addChild(error);				// Error status
		getRequest->addChild(errorDetails);			// Error details
		getRequest->addChild(this->varBindList);	// Elements to ask for

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
 * @brief Check a response header
 * @param ptr Header data
 */
void Snmpv1Pdu::checkHeader(u8 **ptr) {
	try {

		// Read response header
		BerSequence::decode(ptr);

		// Check version
		std::shared_ptr<BerInteger> version = BerInteger::decode(ptr);
		if(version->getValue() != SNMPV1_VERSION) {
			throw std::runtime_error("Not a SNMPv1 PDU");
		}

		// Check community
		std::shared_ptr<BerOctetString> community = BerOctetString::decode(ptr);
		if(community->getValue().compare(this->community) != 0) {
			throw std::runtime_error("Community does not match");
		}

#ifdef SNMP_DEBUG
		version->print();
		community->print();
#endif
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Receive a response from the agent
 * @param sock Socket used for reception
 * @param ip Expected source IP
 */
void Snmpv1Pdu::recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip) {

	try {
		// Create recv buffer
		std::unique_ptr<u8> data(new u8[SNMP_MAX_PDU_SIZE]);

		// Receive packet data
		sock->recvPacket(data.get(), SNMP_MAX_PDU_SIZE, ip, SNMP_PORT);
		u8 *ptr = data.get();

		// Read response header
		this->checkHeader(&ptr);

		// Skip get-response sequence
		BerSequence::decode(&ptr, SNMPV1_TAGCLASS, SNMPV1_GETRESPONSE);

		// Check responseID
		std::shared_ptr<BerInteger> responseID = BerInteger::decode(&ptr);
		if((u32)responseID->getValue() != this->reqID) {
			throw std::runtime_error("RequestID does not match");
		}
#ifdef SNMP_DEBUG
		responseID->print();
#endif

		// Check errors
		std::shared_ptr<BerInteger> errorStatus = BerInteger::decode(&ptr);
#ifdef SNMP_DEBUG
		errorStatus->print();
#endif
		std::shared_ptr<BerInteger> errorDetails = BerInteger::decode(&ptr);
#ifdef SNMP_DEBUG
		errorDetails->print();
#endif
		if(errorStatus->getValue() != SNMPV1_ERROR_NOERROR) {
			throw std::runtime_error(std::string("Error in SNMP response: ") + 
									 std::to_string(errorStatus->getValue()) +
									 std::string("; Details: ") +
									 std::to_string(errorDetails->getValue()));
		}

		// Skip varbindlist sequence
		u32 varbindLength = BerSequence::decode(&ptr);

		// Loop each varbind
		u32 read = 0;
		while(read < varbindLength) {

			u8 *tmp = ptr;

			// Skip varbind sequence
			BerSequence::decode(&ptr);

			// Decode OID
			std::shared_ptr<BerOid> oid = BerOid::decode(&ptr);
#ifdef SNMP_DEBUG
			oid->print();
#endif

			// Decode value
			std::shared_ptr<BerField> value = BerField::decode(&ptr);
			read += (ptr - tmp);
#ifdef SNMP_DEBUG
			value->print();
#endif

			// Add pair OID,value to the varbindlist
			this->addVarBind(oid, value);
		}
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Receive a TRAP pdu
 * @param sock Socket listening to some udp port
 */
void Snmpv1Pdu::recvTrap(std::shared_ptr<UdpSocket> sock) {

	try {
		// Create recv buffer
		std::unique_ptr<u8> data(new u8[SNMP_MAX_PDU_SIZE]);

		// Receive packet data
		sock->recvPacket(data.get(), SNMP_MAX_PDU_SIZE);
		u8 *ptr = data.get();

		// Read response header
		this->checkHeader(&ptr);

		// Skip trap sequence
		BerSequence::decode(&ptr, SNMPV1_TAGCLASS, SNMPV1_TRAP);

		// Add enterprise OID to the list
		this->fields.push_back(BerOid::decode(&ptr));
		
		// Add agent address to the list
		this->fields.push_back(BerOctetString::decode(&ptr));

		// Add generic and specific trap to the list
		this->fields.push_back(BerInteger::decode(&ptr));
		this->fields.push_back(BerInteger::decode(&ptr));

		// Add timestamp to the list
		this->fields.push_back(BerInteger::decode(&ptr));

#ifdef SNMP_DEBUG
		for(u8 i = 0; i < 5; i++) {
			this->fields[i]->print();
		}
#endif

		// Skip varbindlist sequence
		u32 varbindLength = BerSequence::decode(&ptr);

		// Loop each varbind
		u32 read = 0;
		while(read < varbindLength) {

			u8 *tmp = ptr;

			// Skip varbind sequence
			BerSequence::decode(&ptr);

			// Decode OID and value
			std::shared_ptr<BerOid> oid = BerOid::decode(&ptr);
			std::shared_ptr<BerInteger> value = BerInteger::decode(&ptr);
			read += (ptr - tmp);
#ifdef SNMP_DEBUG
			oid->print();
			value->print();
#endif

			// Add pair OID-value to the varbindlist
			this->addVarBind(oid, value);
		}
	} catch (const std::runtime_error &e) {
		this->clear();
		throw;
	} catch (const std::bad_alloc &e) {
		this->clear();
		throw;
	}
}

/**
 * @brief Destructor for a SNMPv1 PDU
 */
Snmpv1Pdu::~Snmpv1Pdu() { }

/**
 * @brief Get a VarBind from the PDU
 * @param i VarBind index
 * @return Desired VarBind
 */
std::shared_ptr<BerField> Snmpv1Pdu::getVarBind(u16 i) {
	try {
		return static_cast<BerSequence*>(this->varBindList->getChild(i).get())->getChild(1);
	} catch (const std::out_of_range &e){
		throw;
	}
}

}
