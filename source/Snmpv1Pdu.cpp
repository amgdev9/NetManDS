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

void Snmpv1Pdu::addVarBind(BerOid *oid, BerField *value) {

	BerSequence *varBind = new BerSequence();
	varBind->addChild(oid);
	varBind->addChild(value);

	this->varBindList->addChild(varBind);
}

BerSequence *Snmpv1Pdu::generateHeader() {

	BerSequence *message = new BerSequence();

	// Header (common to all SNMP PDUs)
	message->addChild(new BerInteger(SNMPV1_VERSION));
	message->addChild(new BerOctetString("public"));		// TODO Configuration
	// From here it can be PDUs or authentication data

	return message;
}

void Snmpv1Pdu::sendRequest(u32 type, UdpSocket *sock, const char *ip) {

	BerSequence *message = this->generateHeader();
	this->addField(message);

	// GetRequest PDU
	BerSequence *getRequest = new BerSequence(SNMPV1_TAGCLASS_REQUEST, type);
	message->addChild(getRequest);

	getRequest->addChild(new BerInteger(0));						// TODO RequestID
	getRequest->addChild(new BerInteger(SNMPV1_ERROR_NOERROR));		// Error status
	getRequest->addChild(new BerInteger(0));						// Error details
	getRequest->addChild(this->varBindList);						// Elements to ask for

	BerPdu::send(sock, ip, SNMPV1_PORT);

	delete getRequest;
}

void Snmpv1Pdu::recvResponse(UdpSocket *sock, const char *ip) {

	// Delete previous data
	delete varBindList;

	// Receive data
	u8 *data = BerPdu::receive(sock, ip, SNMPV1_PORT);
	u8 **ptr = &data;

	// Skip main sequence
	BerSequence::decode(ptr);

	// Get header fields
	BerInteger *responseID = BerInteger::decode(ptr);
	BerInteger *errorStatus = BerInteger::decode(ptr);
	BerInteger *errorDetails = BerInteger::decode(ptr);

	// Skip varbindlist sequence
	BerSequence::decode(ptr);
	varBindList = new BerSequence();

	// Loop each varbind
	while(*ptr != data) {

		// Skip varbind sequence
		BerSequence::decode(ptr);

		// Decode OID and value
		BerOid *oid = BerOid::decode(ptr);
		BerField *value = BerField::decode(ptr);

		// Add pair OID-value to the varbindlist
		this->addVarBind(oid, value);
	}

	// Delete temp data
	delete [] data;
}

Snmpv1Pdu::~Snmpv1Pdu() {
	delete varBindList;
}

}
