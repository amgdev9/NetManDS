/**
 * @file Snmpv3Pdu.cpp
 * @brief SNMPv3 PDU handler
 */

// Own includes
#include "Snmpv3Pdu.h"
#include "asn1/BerInteger.h"

namespace NetMan {

// Static data
u32 Snmpv3Pdu::requestID = 0;

/**
 * @brief Constructor for a Snmpv3 PDU
 */
Snmpv3Pdu::Snmpv3Pdu() {
    this->reqID = 0;
}

std::shared_ptr<BerSequence> Snmpv3Pdu::generateHeader(u8 flags, std::string &engineID, std::string &contextName) {

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
        // TODO Fill + SNMPv3Pdu friend of BerPdu
        securityParams->addField(securityParamsSeq);
        u32 securityParamsSize;
        std::unique_ptr<u8> serializedParams = securityParams->serialize(&securityParamsSize);
        std::shared_ptr<BerOctetString> msgSecurityParameters = std::make_shared<BerOctetString>(std::string((char*)serializedParams.get(), securityParamsSize));

        // Fill msgData
        std::shared_ptr<BerOctetString> contextEngineID = std::make_shared<BerOctetString>(engineID);
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

void Snmpv3Pdu::clear() {

}

void Snmpv3Pdu::addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value) {

}

void Snmpv3Pdu::sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

}

u8 Snmpv3Pdu::recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port, u32 expectedPduType) {
    return 0;
}

void Snmpv3Pdu::recvTrap(std::shared_ptr<UdpSocket> sock) {

}

void Snmpv3Pdu::sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip) {

}

std::shared_ptr<BerField> Snmpv3Pdu::getVarBind(u16 i) {
    return nullptr;
}

/**
 * @brief Destructor for a SNMPv3 PDU
 */
Snmpv3Pdu::~Snmpv3Pdu() {

}

}
