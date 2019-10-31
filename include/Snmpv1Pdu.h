/**
 * @file Snmpv1Pdu.h
 * @brief 
 */

#ifndef SNMPV1PDU_H_
#define SNMPV1PDU_H_

#include "asn1/BerPdu.h"
#include "asn1/BerSequence.h"
#include "asn1/BerOid.h"
#include "asn1/BerNull.h"

// Defines
#define SNMPV1_PORT				161
#define SNMPV1_VERSION			0
#define SNMPV1_ERROR_NOERROR	0
#define SNMPV1_ERROR_TOOBIG		1
#define SNMPV1_ERROR_NOSUCHNAME	2
#define SNMPV1_ERROR_BADVALUE	3
#define SNMPV1_ERROR_READONLY	4
#define SNMPV1_ERROR_GENERR		5

// Defines PDUs tags
#define SNMPV1_TAGCLASS_REQUEST	BER_TAG_CONTEXT
#define SNMPV1_GETREQUEST		0
#define SNMPV1_GETNEXTREQUEST	1
#define SNMPV1_GETRESPONSE		2
#define SNMPV1_SETREQUEST		3

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

class Snmpv1Pdu: public BerPdu {
	private:
		BerSequence *varBindList;
		BerSequence *generateHeader();
	public:
		Snmpv1Pdu();
		void addVarBind(BerOid *oid, BerField *value = new BerNull());
		void sendRequest(u32 type, UdpSocket *sock, const char *ip);
		void recvResponse(UdpSocket *sock, const char *ip);
		~Snmpv1Pdu();
};

}

#endif
