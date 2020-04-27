/**
 * @file Snmpv1Pdu.cpp
 * @brief SNMPv1 PDU handler
 */

// Includes C/C++
#include <stdio.h>
#include <string.h>
#include <stdexcept>

// Own includes
#include "snmp/Snmpv1Pdu.h"
#include "asn1/BerInteger.h"
#include "asn1/BerOctetString.h"
#include "Utils.h"

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
	this->snmpVersion = SNMPV1_VERSION;
}

/**
 * @brief Clear the PDU for further usage
 */
void Snmpv1Pdu::clear() {
	BerPdu::clear();		// Varbindlist can't be here
	this->varBindList.reset();
}

/**
 * @brief Set the VarBindList to empty state
 */
void Snmpv1Pdu::emptyVarBindList() {
	this->varBindList = std::make_shared<BerSequence>();
}

/**
 * @brief Add a field to a VarBindList
 * @param vbList VarBindList to use
 * @param oid Object identifier of the field
 * @param value Value for that field (optional if requesting)
 */
void Snmpv1Pdu::addVarBind(std::shared_ptr<BerSequence> vbList, std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value) {
	
	std::shared_ptr<BerSequence> varBind = nullptr;
	try {
		varBind = std::make_shared<BerSequence>();
	} catch (const std::bad_alloc &e) {
		throw;
	}
	varBind->addChild(oid);
	varBind->addChild(value);

	vbList->addChild(varBind);
}

/**
 * @brief Add a field to the PDU
 * @param oid Object identifier of the field
 * @param value Value for that field (optional if requesting)
 */
void Snmpv1Pdu::addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value) {

	try {
		if(this->varBindList == nullptr) this->varBindList = std::make_shared<BerSequence>();
		Snmpv1Pdu::addVarBind(this->varBindList, oid, value);
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Generate a PDU header
 * @param ver	SNMP version to use
 * @return The root sequence of the PDU
 */
std::shared_ptr<BerSequence> Snmpv1Pdu::generateHeader(u32 ver) {

	try {
		std::shared_ptr<BerSequence> message = std::make_shared<BerSequence>();

		// Header (common to all SNMP PDUs)
		std::shared_ptr<BerInteger> version = std::make_shared<BerInteger>(&ver, sizeof(u32), false);
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
 * @brief Generate a SNMPv1 PDU
 * @param type PDU type
 * @return The generated PDU (without its header)
 */
std::shared_ptr<BerSequence> Snmpv1Pdu::generateRequest(u32 type) {

	std::shared_ptr<BerSequence> getRequest = std::make_shared<BerSequence>(SNMPV1_TAGCLASS, type);

	this->reqID = ++Snmpv1Pdu::requestID;
	u32 errorInteger = SNMPV1_ERROR_NOERROR;
	u32 errorDetailsInteger = 0;
	std::shared_ptr<BerInteger> reqid = std::make_shared<BerInteger>(&this->reqID, sizeof(u32), false);
	std::shared_ptr<BerInteger> error = std::make_shared<BerInteger>(&errorInteger, sizeof(u32), false);
	std::shared_ptr<BerInteger> errorDetails = std::make_shared<BerInteger>(&errorDetailsInteger, sizeof(u32), false);
	getRequest->addChild(reqid);				// RequestID
	getRequest->addChild(error);				// Error status
	getRequest->addChild(errorDetails);			// Error details
	getRequest->addChild(this->varBindList);	// Elements to ask for

	return getRequest;
}

/**
 * @brief Send a GET REQUEST
 * @param type Type of SNMP PDU
 * @param socket Socket used when sending the PDU
 * @param ip Destination IP. If zero, last socket's remote host IP-port will be used
 * @param port  Destination port
 */
void Snmpv1Pdu::sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, in_addr_t ip, u16 port) {

	try {

		if(this->varBindList == nullptr) {
			throw std::runtime_error("Empty VarBindList");
		}

		std::shared_ptr<BerSequence> message = this->generateHeader(this->snmpVersion);

		// GetRequest PDU
		std::shared_ptr<BerSequence> getRequest = this->generateRequest(type);
		message->addChild(getRequest);

		this->fields.push_back(message);
		BerPdu::send(sock, ip, port);

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
		std::shared_ptr<BerInteger> version = BerInteger::decode(ptr, false);
		if(version->getValueU32() != this->snmpVersion) {
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
 * @brief Receive a response from a buffer
 * @param ptr Buffer data
 * @param checkResponseID Check response ID?
 * @param reqID Expected request ID
 * @param pduType Type of response PDU obtained (=expectedPduType, or obtained PDU if SNMP_PDU_ANY)
 * @param expectedPduType Expected PDU type
 */
std::shared_ptr<BerSequence> Snmpv1Pdu::recvResponse(u8 **ptr, bool checkResponseID, u32 reqID, u8 *pduType, u32 expectedPduType) {

	try {

		// Skip get-response sequence
		BerSequence::decode(ptr, SNMPV1_TAGCLASS, expectedPduType, pduType);

		// Check responseID
		std::shared_ptr<BerInteger> responseID = BerInteger::decode(ptr, false);
		if(checkResponseID && responseID->getValueU32() != reqID && responseID->getValueU32() != 0) {
			throw std::runtime_error("RequestID does not match");
		}
		if(!checkResponseID) {		// Save the request ID for the possible ACK
			Snmpv1Pdu::requestID = responseID->getValueU32() - 1;
		}
#ifdef SNMP_DEBUG
		responseID->print();
#endif

		// Check errors
		std::shared_ptr<BerInteger> errorStatus = BerInteger::decode(ptr, false);
#ifdef SNMP_DEBUG
		errorStatus->print();
#endif
		std::shared_ptr<BerInteger> errorDetails = BerInteger::decode(ptr, false);
#ifdef SNMP_DEBUG
		errorDetails->print();
#endif
		if(errorStatus->getValueU32() != SNMPV1_ERROR_NOERROR) {
			throw std::runtime_error(std::string("Error in SNMP response: ") + 
									 std::to_string(errorStatus->getValueU32()) +
									 std::string("; Details: ") +
									 std::to_string(errorDetails->getValueU32()));
		}

		// Skip varbindlist sequence
		u32 varbindLength = BerSequence::decode(ptr);

		// Create a VarBindList
		std::shared_ptr<BerSequence> vbList = std::make_shared<BerSequence>();

		// Loop each varbind
		u32 read = 0;
		while(read < varbindLength) {

			u8 *tmp = *ptr;

			// Skip varbind sequence
			BerSequence::decode(ptr);

			// Decode OID
			std::shared_ptr<BerOid> oid = BerOid::decode(ptr);
#ifdef SNMP_DEBUG
			oid->print();
#endif

			// Decode value
			std::shared_ptr<BerField> value = BerField::decode(ptr);
			read += (*ptr - tmp);
#ifdef SNMP_DEBUG
			value->print();
#endif

			// Add pair OID,value to the varbindlist
			Snmpv1Pdu::addVarBind(vbList, oid, value);
		}

		// Return the loaded VarBindList
		return vbList;

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
 * @param port Expected port
 * @param expectedPduType Expected PDU type
 * @return Type of response PDU obtained (=expectedPduType, or obtained PDU if SNMP_PDU_ANY)
 */
u8 Snmpv1Pdu::recvResponse(std::shared_ptr<UdpSocket> sock, in_addr_t ip, u16 port, u32 expectedPduType) {

	try {
		// Create recv buffer
		std::unique_ptr<u8> data(new u8[SNMP_MAX_PDU_SIZE]);

		// Receive packet data
		sock->recvPacket(data.get(), SNMP_MAX_PDU_SIZE, ip, port);
		u8 *ptr = data.get();

		// Read response header
		this->checkHeader(&ptr);

		// Read PDU fields
		u8 pduType;
		this->varBindList = Snmpv1Pdu::recvResponse(&ptr, port != 0, this->reqID, &pduType, expectedPduType);
		return pduType;

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
		this->fields.push_back(BerInteger::decode(&ptr, false));
		this->fields.push_back(BerInteger::decode(&ptr, false));

		// Add timestamp to the list
		this->fields.push_back(BerInteger::decode(&ptr, false));

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
			std::shared_ptr<BerField> value = BerField::decode(&ptr);
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
		return std::static_pointer_cast<BerSequence>(this->varBindList->getChild(i))->getChild(1);
	} catch (const std::out_of_range &e){
		throw;
	}
}

/**
 * @brief Get a VarBind OID from the PDU
 * @param i VarBind index
 * @return Desired VarBind OID
 */
std::shared_ptr<BerOid> Snmpv1Pdu::getVarBindOid(u16 i) {
    try {
		return std::static_pointer_cast<BerOid>(std::static_pointer_cast<BerSequence>(this->varBindList->getChild(i))->getChild(0));
	} catch (const std::out_of_range &e){
		throw;
	}
}

/**
 * @brief Serialize a SNMPv1 trap into a JSON
 * @return The serialized trap
 */
std::shared_ptr<json_t> Snmpv1Pdu::serializeTrap() {

    auto root = std::shared_ptr<json_t>(json_object(), [=](json_t* data) { json_decref(data); });
    
    json_t *data = json_array();
    json_object_set_new(root.get(), "data", data);

    Utils::addJsonField(data, "Enterprise ID");
    Utils::addJsonField(data, fields[0]->print());

    Utils::addJsonField(data, "Agent address");
    auto agentAddress = std::static_pointer_cast<BerOctetString>(fields[1])->getValue();
    if(agentAddress.length() == 4) {
        Utils::addJsonField(data, std::to_string(agentAddress[0]) + "." + std::to_string(agentAddress[1]) + "." +
                               std::to_string(agentAddress[2]) + "." + std::to_string(agentAddress[3]));
    }

    Utils::addJsonField(data, "Generic trap: " + fields[2]->print());
    Utils::addJsonField(data, "Specific trap: " + fields[3]->print());
    
    u32 timestamp = std::static_pointer_cast<BerInteger>(fields[4])->getValueU32() / 100;
    u8 hour = timestamp / 3600;
    u8 minute = (timestamp % 3600) / 60;
    u8 second = (timestamp % 3600) % 60;
    Utils::addJsonField(data, "Timestamp: " + std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second));

    if(varBindList != nullptr) {
        for(u32 i = 0; i < varBindList->getNChildren(); i++) {
            auto child = std::static_pointer_cast<BerSequence>(this->varBindList->getChild(i));
            Utils::addJsonField(data, "OID: " + child->getChild(0)->print());
            Utils::addJsonField(data, "Value: " + child->getChild(1)->print());
        }
    }

    return root;
}

}
