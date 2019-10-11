/**
 * @file BerDocument.cpp
 * @brief 
 */

#include "asn1/BerPdu.h"

namespace NetMan {

BerPdu::BerPdu() {
	fields = std::vector<BerField*>();
}

void BerPdu::addField(BerField *field) {
	fields.push_back(field);
}

BerPdu::~BerPdu() {

	for(u32 i = 0; i < this->fields.size(); i++) {
		delete fields[i];
	}
}

void BerPdu::send(UdpSocket *sock, const char *ip, u16 port) {

	// Variables
	u32 pdu_size = 0;

	// Calculate total PDU size
	for(u32 i = 0; i < fields.size(); i++) {
		pdu_size += fields[i]->getTotalSize();
	}

	// Create the PDU buffer
	u8 *pdu_base = new u8[pdu_size];
	u8 *pdu_buffer = pdu_base;

	// Fill in the PDU buffer
	for(u32 i = 0; i < fields.size(); i++) {
		fields[i]->parseTag(&pdu_buffer);
		fields[i]->parseLength(&pdu_buffer);
		fields[i]->parseData(&pdu_buffer);
	}

	// Send the PDU packet
	sock->sendPacket(pdu_base, pdu_size, ip, port);

	// Delete PDU buffer
	delete [] pdu_base;
}

void BerPdu::receive(UdpSocket *sock, const char *ip, u16 port) {
	// TODO
}

}
