/**
 * @file Snmpv3Pdu.cpp
 * @brief SNMPv3 PDU handler
 */

// Own includes
#include "Snmpv3Pdu.h"
#include "Snmpv2Pdu.h"
#include "asn1/BerInteger.h"

namespace NetMan {

// Static data
u32 Snmpv3Pdu::requestID = 0;

/**
 * @brief Constructor for a Snmpv3 PDU
 * @param contextName Name of the SNMP context
 * @param userName User name
 */
Snmpv3Pdu::Snmpv3Pdu(std::string &contextName, std::string &userName) {
    this->reqID = 0;
	this->contextName = contextName;
	this->userName = userName;
	this->varBindList = nullptr;
}

std::shared_ptr<BerSequence> Snmpv3Pdu::generateHeader(u8 flags) {

    try {
		std::shared_ptr<BerSequence> message = std::make_shared<BerSequence>();

		// Header (common to all SNMPv3 PDUs)
        u32 ver = SNMPV3_VERSION;
		std::shared_ptr<BerInteger> msgVersion = std::make_shared<BerInteger>(&ver, sizeof(u32), false);
        std::shared_ptr<BerSequence> msgGlobalData = std::make_shared<BerSequence>();
        std::shared_ptr<BerSequence> msgData = std::make_shared<BerSequence>();

        // Fill msgGlobalData
        this->reqID = ++Snmpv3Pdu::requestID;
        std::shared_ptr<BerInteger> msgID = std::make_shared<BerInteger>(&this->reqID, sizeof(u32), false);
        u32 maxSize = SNMP_MAX_PDU_SIZE;
        std::shared_ptr<BerInteger> msgMaxSize = std::make_shared<BerInteger>(&maxSize, sizeof(u32), false);
        std::shared_ptr<BerOctetString> msgFlags = std::make_shared<BerOctetString>(std::string(1, flags));
        u32 securityModel = SNMPV3_USM_MODEL;
        std::shared_ptr<BerInteger> msgSecurityModel = std::make_shared<BerInteger>(&securityModel, sizeof(u32), false);
        msgGlobalData->addChild(msgID);
        msgGlobalData->addChild(msgMaxSize);
        msgGlobalData->addChild(msgFlags);
        msgGlobalData->addChild(msgSecurityModel);

        // Fill msgSecurityParameters
        std::shared_ptr<BerPdu> securityParams = std::make_shared<BerPdu>();
        std::shared_ptr<BerSequence> securityParamsSeq = std::make_shared<BerSequence>();
        std::shared_ptr<BerOctetString> contextEngineID = std::make_shared<BerOctetString>(secParams.msgAuthoritativeEngineID);
        std::shared_ptr<BerInteger> engineBoots = std::make_shared<BerInteger>(&secParams.msgAuthoritativeEngineBoots, sizeof(u32), false);
        std::shared_ptr<BerInteger> engineTime = std::make_shared<BerInteger>(&secParams.msgAuthoritativeEngineTime, sizeof(u32), false);
        std::shared_ptr<BerOctetString> userName = std::make_shared<BerOctetString>(secParams.msgUserName);
        std::shared_ptr<BerOctetString> authParams = std::make_shared<BerOctetString>(secParams.msgAuthenticationParameters);
        std::shared_ptr<BerOctetString> privParams = std::make_shared<BerOctetString>(secParams.msgPrivacyParameters);

        securityParamsSeq->addChild(contextEngineID);   // Equals msgAuthoritativeEngineID
        securityParamsSeq->addChild(engineBoots);
        securityParamsSeq->addChild(engineTime);
        securityParamsSeq->addChild(userName);
        securityParamsSeq->addChild(authParams);
        securityParamsSeq->addChild(privParams);

        securityParams->addField(securityParamsSeq);
        u32 securityParamsSize;
        std::unique_ptr<u8> serializedParams = securityParams->serialize(&securityParamsSize);
        std::shared_ptr<BerOctetString> msgSecurityParameters = std::make_shared<BerOctetString>(std::string((char*)serializedParams.get(), securityParamsSize));

        // Fill msgData
        std::shared_ptr<BerOctetString> contextNameField = std::make_shared<BerOctetString>(contextName);
        msgData->addChild(contextEngineID);
        msgData->addChild(contextNameField);
        // From here the SNMPv2 PDU is added without its header (encrypted or not)

        // Add each structure to the header
        message->addChild(msgVersion);
		message->addChild(msgGlobalData);
        message->addChild(msgSecurityParameters);
        message->addChild(msgData);

		return message;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

void Snmpv3Pdu::checkHeader(u8 **ptr, bool copyParams) {

    try {

		// Read response header
		BerSequence::decode(ptr);

		// Check version
		std::shared_ptr<BerInteger> version = BerInteger::decode(ptr, false);
		if(version->getValueU32() != SNMPV3_VERSION) {
			throw std::runtime_error("Not a SNMPv3 PDU");
		}
#ifdef SNMP_DEBUG
	version->print();
#endif

		// Check msgID
		BerSequence::decode(ptr);
		std::shared_ptr<BerInteger> msgID = BerInteger::decode(ptr, false);
		if(msgID->getValueU32() != this->reqID) {
			throw std::runtime_error("msgID does not match");
		}
#ifdef SNMP_DEBUG
	msgID->print();
#endif

		// Skip maxSize and flags
		BerInteger::decode(ptr, false);
		BerOctetString::decode(ptr);

		// Check msgSecurityModel
		std::shared_ptr<BerInteger> msgSecurityModel = BerInteger::decode(ptr, false);
		if(msgSecurityModel->getValueU32() != SNMPV3_USM_MODEL) {
			throw std::runtime_error("msgSecurityModel does not match");
		}
#ifdef SNMP_DEBUG
	msgSecurityModel->print();
#endif

		// Get securityParameters
		std::shared_ptr<BerOctetString> securityParams = BerOctetString::decode(ptr);

		// Check securityParameters
		u8 *paramsPtr = (u8*)securityParams->getValue().c_str();
		BerSequence::decode(&paramsPtr);
		if(copyParams) {
			secParams.msgAuthoritativeEngineID = BerOctetString::decode(&paramsPtr)->getValue();
			secParams.msgAuthoritativeEngineBoots = BerInteger::decode(&paramsPtr, false)->getValueU32();
			secParams.msgAuthoritativeEngineTime = BerInteger::decode(&paramsPtr, false)->getValueU32();
			// Skip userName, msgAuthenticationParameters and msgPrivacyParameters
		} else {
			std::shared_ptr<BerOctetString> engineID = BerOctetString::decode(&paramsPtr);
			if(engineID->getValue().compare(secParams.msgAuthoritativeEngineID) != 0) {
				throw std::runtime_error("EngineID does not match");
			}
#ifdef SNMP_DEBUG
	engineID->print();
#endif

			std::shared_ptr<BerInteger> engineBoots = BerInteger::decode(&paramsPtr, false);
#ifdef SNMP_DEBUG
	engineBoots->print();
#endif

			std::shared_ptr<BerInteger> engineTime = BerInteger::decode(&paramsPtr, false);
#ifdef SNMP_DEBUG
	engineTime->print();
#endif

			std::shared_ptr<BerOctetString> userName = BerOctetString::decode(&paramsPtr);
			if(userName->getValue().compare(secParams.msgUserName) != 0) {
				throw std::runtime_error("msgUserName does not match");
			}
#ifdef SNMP_DEBUG
	userName->print();
#endif

			std::shared_ptr<BerOctetString> authParams = BerOctetString::decode(&paramsPtr);
			if(authParams->getValue().compare(secParams.msgAuthenticationParameters) != 0) {
				throw std::runtime_error("authParams does not match");
			}
#ifdef SNMP_DEBUG
	authParams->print();
#endif

			std::shared_ptr<BerOctetString> privParams = BerOctetString::decode(&paramsPtr);
			if(privParams->getValue().compare(secParams.msgPrivacyParameters) != 0) {
				throw std::runtime_error("privParams does not match");
			}
#ifdef SNMP_DEBUG
	privParams->print();
#endif
		}

        // Skip msgData sequence
		BerSequence::decode(ptr);

		// Check contextEngineID
		std::shared_ptr<BerOctetString> contextID = BerOctetString::decode(ptr);
		if(!copyParams) {
			if(contextID->getValue().compare(secParams.msgAuthoritativeEngineID) != 0) {
				throw std::runtime_error("contextID does not match");
			}
#ifdef SNMP_DEBUG
	contextID->print();
#endif
		}

		// Check contextName
		std::shared_ptr<BerOctetString> contextNameStr = BerOctetString::decode(ptr);
		if(!copyParams) {
			if(contextNameStr->getValue().compare(contextName) != 0) {
				throw std::runtime_error("contextName does not match");
			}
#ifdef SNMP_DEBUG
	contextNameStr->print();
#endif
		}
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

void Snmpv3Pdu::clear() {
	BerPdu::clear();		// Varbindlist can't be here
	this->varBindList.reset();
}

void Snmpv3Pdu::addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value) {
	try {
		if(this->varBindList == nullptr) this->varBindList = std::make_shared<BerSequence>();
		Snmpv1Pdu::addVarBind(this->varBindList, oid, value);
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

void Snmpv3Pdu::sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip, u32 nonRepeaters, u32 maxRepetitions) {

    try {
		
		if(this->varBindList == nullptr) {
			throw std::runtime_error("Empty VarBindList");
		}

        // Generate a header
        secParams.msgAuthoritativeEngineID = "";
        secParams.msgAuthoritativeEngineBoots = 0;
        secParams.msgAuthoritativeEngineTime = 0;
        secParams.msgUserName = "";
        secParams.msgAuthenticationParameters = "";
        secParams.msgPrivacyParameters = "";
		std::shared_ptr<BerSequence> message = this->generateHeader(SNMPV3_FLAG_REPORTABLE);

        // Attach a serialized SNMPv2 PDU
        std::string community = "public";
        std::shared_ptr<Snmpv2Pdu> snmpv2Pdu = std::make_shared<Snmpv2Pdu>(community);
        snmpv2Pdu->emptyVarBindList();
        std::shared_ptr<BerSequence> pdu = snmpv2Pdu->generateRequest(SNMPV2_GETREQUEST);
        std::static_pointer_cast<BerSequence>(message->getChild(3))->addChild(pdu);

        // Send the PDU
		this->fields.push_back(message);
		BerPdu::send(sock, ip, SNMP_PORT);

        // Clear data
	    this->fields.clear();

        // Receive report
		this->recvResponse(sock, ip, SNMP_PORT, SNMPV2_REPORT);

        // Send getrequest
		secParams.msgUserName = this->userName;
		message = this->generateHeader(SNMPV3_FLAG_REPORTABLE);
		snmpv2Pdu = std::make_shared<Snmpv2Pdu>(community);
		snmpv2Pdu->varBindList = this->varBindList;
		if(type == SNMPV2_GETBULKREQUEST) {
			pdu = snmpv2Pdu->generateBulkRequest(nonRepeaters, maxRepetitions);
		} else {
			pdu = snmpv2Pdu->generateRequest(type);
		}
		std::static_pointer_cast<BerSequence>(message->getChild(3))->addChild(pdu);

		// Send the PDU
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

u8 Snmpv3Pdu::recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port, u32 expectedPduType) {
    
    try {
		// Create recv buffer
		std::unique_ptr<u8> data(new u8[SNMP_MAX_PDU_SIZE]);

		// Receive packet data
		sock->recvPacket(data.get(), SNMP_MAX_PDU_SIZE, ip, port);
		u8 *ptr = data.get();

		// Read response header
		this->checkHeader(&ptr, expectedPduType == SNMPV2_REPORT);	// Copy security parameters if we expect a report PDU

		// Decode SNMP PDU
		u8 pduType;
		std::shared_ptr<BerSequence> vbList = Snmpv2Pdu::recvResponse(&ptr, port, this->reqID, &pduType, expectedPduType);
		if(pduType != SNMPV2_REPORT) {
			this->varBindList = vbList;
		}
		return pduType;
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

void Snmpv3Pdu::recvTrap(std::shared_ptr<UdpSocket> sock) {

}

void Snmpv3Pdu::sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip) {
	this->sendRequest(SNMPV2_GETBULKREQUEST, sock, ip, nonRepeaters, maxRepetitions);
}

std::shared_ptr<BerField> Snmpv3Pdu::getVarBind(u16 i) {
    try {
		return static_cast<BerSequence*>(this->varBindList->getChild(i).get())->getChild(1);
	} catch (const std::out_of_range &e){
		throw;
	}
}

/**
 * @brief Destructor for a SNMPv3 PDU
 */
Snmpv3Pdu::~Snmpv3Pdu() { }

}
