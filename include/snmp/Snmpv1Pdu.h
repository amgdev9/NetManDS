/**
 * @file Snmpv1Pdu.h
 * @brief SNMPv1 PDU handler
 */

#ifndef SNMPV1PDU_H_
#define SNMPV1PDU_H_

// Includes C/C++
#include <memory>

// Own includes
#include "asn1/BerPdu.h"
#include "asn1/BerSequence.h"
#include "asn1/BerOid.h"
#include "asn1/BerNull.h"
#include "asn1/BerOctetString.h"
#include "Snmp.h"

// Defines
#define SNMPV1_VERSION			0
#define SNMPV1_ERROR_NOERROR	0
#define SNMPV1_ERROR_TOOBIG		1
#define SNMPV1_ERROR_NOSUCHNAME	2
#define SNMPV1_ERROR_BADVALUE	3
#define SNMPV1_ERROR_READONLY	4
#define SNMPV1_ERROR_GENERR		5

// Defines PDUs tags
#define SNMPV1_TAGCLASS				(BER_TAG_CONTEXT | BER_TAG_STRUCTURED)
#define SNMPV1_GETREQUEST			0
#define SNMPV1_GETNEXTREQUEST		1
#define SNMPV1_GETRESPONSE			2
#define SNMPV1_SETREQUEST			3
#define SNMPV1_TRAP					4

// Defines Application types
#define SNMPV1_TAGCLASS_NETWORKADDRESS 	BER_TAG_APPLICATION		/* OCTET STRING of SIZE 4 */
#define SNMPV1_TAG_NETWORKADDRESS		0
#define SNMPV1_TAGCLASS_COUNTER		 	BER_TAG_APPLICATION		/* u32 */
#define SNMPV1_TAG_COUNTER				1
#define SNMPV1_TAGCLASS_GAUGE 			BER_TAG_APPLICATION		/* u32 */
#define SNMPV1_TAG_GAUGE				2
#define SNMPV1_TAGCLASS_TIMETICKS	 	BER_TAG_APPLICATION		/* u32 */
#define SNMPV1_TAG_TIMETICKS			3
#define SNMPV1_TAGCLASS_OPAQUE		 	BER_TAG_APPLICATION		/* OCTET STRING */
#define SNMPV1_TAG_OPAQUE				4

namespace NetMan {

/**
 * @class Snmpv1Pdu
 */
class Snmpv1Pdu: public BerPdu {
	protected:
		u32 snmpVersion;
		std::shared_ptr<BerSequence> varBindList;
		std::shared_ptr<BerSequence> generateHeader(u32 ver);
		std::shared_ptr<BerSequence> generateRequest(u32 type);
		static std::shared_ptr<BerSequence> recvResponse(u8 **ptr, bool checkResponseID, u32 reqID, u8 *pduType, u32 expectedPduType = SNMPV1_GETRESPONSE);
		static void addVarBind(std::shared_ptr<BerSequence> vbList, std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value);
		void checkHeader(u8 **ptr);
		static u32 requestID;
		u32 reqID;
		std::string community;
	public:
		Snmpv1Pdu(const std::string &community);
		void clear() override;
		void emptyVarBindList();
		void addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value);
		virtual void sendRequest(u32 type, std::shared_ptr<Socket> sock, const std::string &ip);
		virtual u8 recvResponse(std::shared_ptr<Socket> sock, const std::string &ip, u16 port, u32 expectedPduType = SNMPV1_GETRESPONSE);
		virtual void recvTrap(std::shared_ptr<Socket> sock);
		std::shared_ptr<BerField> getVarBind(u16 i);
		~Snmpv1Pdu();
};

}

#endif
