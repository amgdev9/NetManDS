/**
 * @file BerPdu.cpp
 * @brief BER PDU handler
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "asn1/BerPdu.h"

namespace NetMan {

/**
 * @brief Constructor for a BerPdu
 */
BerPdu::BerPdu() {
	fields = std::vector<std::shared_ptr<BerField>>();
}

/**
 * @brief Clear a BerPdu for further usage
 */
void BerPdu::clear() {
	fields.clear();
}

/**
 * @brief Destructor for a BerPdu
 */
BerPdu::~BerPdu() { }

/**
 * @brief Send a BerPdu
 * @param sock Socket to be used for sending
 * @param ip Destination IP address
 * @param port Destination port
 */
void BerPdu::send(std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port) {

	// Variables
	u32 pdu_size = 0;

	// Calculate total PDU size
	for(u32 i = 0; i < fields.size(); i++) {
		pdu_size += fields[i]->getTotalSize();
	}

	// Create the PDU buffer
	try {
		std::unique_ptr<u8> pdu_base(new u8[pdu_size]);
		u8 *pdu_buffer = pdu_base.get();

		// Fill in the PDU buffer
		for(u32 i = 0; i < fields.size(); i++) {
			fields[i]->parseTag(&pdu_buffer);
			fields[i]->parseLength(&pdu_buffer);
			fields[i]->parseData(&pdu_buffer);
		}

		// Send the PDU packet
		sock->sendPacket(pdu_base.get(), pdu_size, ip, port);
	} catch (const std::runtime_error &e) {
		throw;
	}
}

}
