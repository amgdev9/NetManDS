/**
 * @file BerEncoder.cpp
 * @brief 
 */

#include "asn1/BerEncoder.h"

namespace NetMan {

BerEncoder::BerEncoder() {
	// TODO Auto-generated constructor stub

}

void BerEncoder::encodeAndSend(BerDocument *doc, UdpSocket *sock, const char *ip, u16 port) {

	std::vector<BerField*> fields = doc->getFields();

	u32 pdu_size = 0;

	for(u32 i = 0; i < fields.size(); i++) {
		pdu_size += fields[i]->getSize();
	}

	u8 *pdu_buffer = new u8[pdu_size];
	pdu_size = 0;		// Stores the writing address

	for(u32 i = 0; i < fields.size(); i++) {
		fields[i]->encode(&pdu_buffer[pdu_size]);
		pdu_size += fields[i]->getSize();
	}

	sock->sendPacket(pdu_buffer, pdu_size, ip, port);

	delete [] pdu_buffer;
}

BerEncoder::~BerEncoder() {
	// TODO Auto-generated destructor stub
}

}
