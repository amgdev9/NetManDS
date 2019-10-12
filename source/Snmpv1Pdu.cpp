/**
 * @file Snmpv1Pdu.cpp
 * @brief 
 */

#include "Snmpv1Pdu.h"
#include "asn1/BerInteger.h"
#include "asn1/BerOctetString.h"

namespace NetMan {

Snmpv1Pdu::Snmpv1Pdu() {

	this->varBindList = new BerSequence();
}

void Snmpv1Pdu::addVarBind(const char *oid, BerField *value) {

	BerSequence *varBind = new BerSequence();
	varBind->addChild(new BerOid(oid));
	varBind->addChild(value);

	this->varBindList->addChild(varBind);
}

void Snmpv1Pdu::generateHeader() {

	BerSequence *message = new BerSequence();
	this->addField(message);

	// Header (common to all SNMP PDUs)
	message->addChild(new BerInteger(SNMPV1_VERSION));
	message->addChild(new BerOctetString("public"));		// TODO Configuration
	// From here it can be PDUs or authentication data
}

void Snmpv1Pdu::sendRequest(u32 type, UdpSocket *sock, const char *ip) {

	this->generateHeader();

	// GetRequest PDU
	BerSequence *getRequest = new BerSequence(SNMPV1_TAGCLASS_REQUEST, type);
	message->addChild(getRequest);

	getRequest->addChild(new BerInteger(0));						// TODO RequestID
	getRequest->addChild(new BerInteger(SNMPV1_ERROR_NOERROR));		// Error status
	getRequest->addChild(new BerInteger(0));						// Error details
	getRequest->addChild(this->varBindList);						// Elements to ask for

	BerPdu::send(sock, ip, SNMPV1_PORT);
}

}
