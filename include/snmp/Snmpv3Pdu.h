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
#define SNMPV3_FLAG_REPORTABLE	(1 << 2)	/**< Use it in messages which need a response */
#define SNMPV3_FLAG_PRIV		(1 << 1)	/**< Encryption algo used? */
#define SNMPV3_FLAG_AUTH		(1 << 0)	/**< Authentication used? */

// Defines error status
#define SNMPV3_SECMODEL_MISMATCH	"1.3.6.1.6.3.15.1.1.1.0"
#define SNMPV3_USERNAME_MISMATCH	"1.3.6.1.6.3.15.1.1.3.0"
#define SNMPV3_ENGINEID_MISMATCH	"1.3.6.1.6.3.15.1.1.4.0"
#define SNMPV3_AUTH_WRONG			"1.3.6.1.6.3.15.1.1.5.0"
#define SNMPV3_PRIV_WRONG			"1.3.6.1.6.3.15.1.1.6.0"

namespace NetMan {

/**
 * @struct Snmpv3SecurityParams
 * @brief Security parameters for SNMPv3 PDUs
 */
typedef struct {
	std::string msgAuthoritativeEngineID;
    u32 msgAuthoritativeEngineBoots;
    u32 msgAuthoritativeEngineTime;
    std::string msgUserName;
    std::string msgAuthenticationParameters;
    std::string msgPrivacyParameters;
} Snmpv3SecurityParams;

/**
 * @class Snmpv3Pdu
 */
class Snmpv3Pdu: public BerPdu {
	private:
		std::string contextName;
		Snmpv3SecurityParams secParams;
		std::shared_ptr<BerSequence> varBindList;
		static u32 requestID;
		u32 reqID;
		std::shared_ptr<BerSequence> generateHeader(u32 type, bool reportable, std::shared_ptr<BerField> scopedPDU);
		std::shared_ptr<BerSequence> generateScopedPdu(std::shared_ptr<BerSequence> pdu);
		std::shared_ptr<BerOctetString> checkHeader(u8 **ptr, bool checkMsgID, Snmpv3SecurityParams &params, std::shared_ptr<UdpSocket> sock, u8 *flags);
		static void sendReportTo(std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port, const std::string &reasonOid, const Snmpv3SecurityParams &params);
	public:
		Snmpv3Pdu(const std::string &engineID, const std::string &contextName, const std::string &userName);
		void clear() override;
		void addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value);
		void sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port, u32 nonRepeaters = 0, u32 maxRepetitions = 0);
		u8 recvResponse(std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port, u32 expectedPduType = SNMPV1_GETRESPONSE);
		void recvTrap(std::shared_ptr<UdpSocket> sock);
		void sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port);
		std::shared_ptr<BerField> getVarBind(u16 i);
		~Snmpv3Pdu();
};

}

#endif
