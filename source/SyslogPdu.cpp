/**
 * @file SyslogPdu.cpp
 * @brief Syslog PDU processing
 */

// Own includes
#include "SyslogPdu.h"

namespace NetMan {

SyslogPdu::SyslogPdu() {

}

SyslogPdu::~SyslogPdu() {

}

void SyslogPdu::recvLog(std::shared_ptr<UdpSocket> sock) {

    try {
		// Create recv buffer
		std::unique_ptr<u8> data(new u8[SYSLOG_MAX_PDU_SIZE]);

		// Receive packet data
		sock->recvPacket(data.get(), SYSLOG_MAX_PDU_SIZE);
		//u8 *ptr = data.get();

	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

}
