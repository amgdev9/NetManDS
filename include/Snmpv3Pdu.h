/**
 * @file Snmpv3Pdu.h
 * @brief SNMPv3 PDU handler
 */

#ifndef SNMPV3PDU_H_
#define SNMPV3PDU_H_

// Own includes
#include "Snmpv2Pdu.h"

namespace NetMan {

/**
 * @class Snmpv3Pdu
 */
class Snmpv3Pdu: public Snmpv2Pdu {
	public:
		Snmpv3Pdu();
        void sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip) override;
	    u8 recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip, u32 expectedPduType = SNMPV1_GETRESPONSE) override;
        void sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip) override;
		void recvTrap(std::shared_ptr<UdpSocket> sock) override;
		~Snmpv3Pdu();
};

}

#endif
