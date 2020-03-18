/**
 * @file Snmpv3Pdu.h
 * @brief SNMPv3 PDU handler
 */

#ifndef SNMPV3PDU_H_
#define SNMPV3PDU_H_

// Own includes
#include "asn1/BerPdu.h"
#include "asn1/BerOid.h"
#include "Snmpv1Pdu.h"

// Defines
#define SNMPV3_VERSION			3
#define SNMPV3_USM_MODEL		3

// Defines flags
#define SNMPV3_FLAG_REPOTABLE	(1 << 2)	/**< Use it in messages which need a response */
#define SNMPV3_FLAG_PRIV		(1 << 1)	/**< Encryption algo used? */
#define SNMPV3_FLAG_AUTH		(1 << 0)	/**< Authentication used? */

namespace NetMan {

/**
 * @class Snmpv3Pdu
 */
class Snmpv3Pdu: public BerPdu {
	private:
		static u32 requestID;
		u32 reqID;
		std::shared_ptr<BerSequence> generateHeader(u8 flags, std::string &engineID, std::string &contextName);
	public:
		Snmpv3Pdu();
		void clear() override;
		void addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value);
		void sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip);
		u8 recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port, u32 expectedPduType = SNMPV1_GETRESPONSE);
		void recvTrap(std::shared_ptr<UdpSocket> sock);
		void sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip);
		std::shared_ptr<BerField> getVarBind(u16 i);
		~Snmpv3Pdu();
};

}

#endif
