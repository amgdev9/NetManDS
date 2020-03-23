/**
 * @file Snmpv2Pdu.h
 * @brief SNMPv2c PDU handler
 */

#ifndef SNMPV2PDU_H_
#define SNMPV2PDU_H_

// Own includes
#include "Snmpv1Pdu.h"

// Defines
#define SNMPV2_VERSION			        1
#define SNMPV2_ERROR_NOERROR	        0
#define SNMPV2_ERROR_TOOBIG		        1
#define SNMPV2_ERROR_NOSUCHNAME	        2
#define SNMPV2_ERROR_BADVALUE	        3
#define SNMPV2_ERROR_READONLY	        4
#define SNMPV2_ERROR_GENERR		        5
#define SNMPV2_ERROR_NOACCESS           6
#define SNMPV2_ERROR_WRONGTYPE          7
#define SNMPV2_ERROR_WRONGLENGTH        8
#define SNMPV2_ERROR_WRONGENCODING      9
#define SNMPV2_ERROR_WRONGVALUE         10
#define SNMPV2_ERROR_NOCREATION         11
#define SNMPV2_ERROR_INCONSISTENTVALUE  12
#define SNMPV2_ERROR_RESUNAVAILABLE     13
#define SNMPV2_ERROR_COMMITFAILED       14
#define SNMPV2_ERROR_UNDOFAILED         15
#define SNMPV2_ERROR_AUTH               16
#define SNMPV2_ERROR_NOTWRITABLE        17
#define SNMPV2_ERROR_INCONSISTENTNAME   18

// Defines PDUs tags
#define SNMPV2_GETREQUEST			0
#define SNMPV2_GETNEXTREQUEST		1
#define SNMPV2_GETRESPONSE			2
#define SNMPV2_SETREQUEST			3
/* Type 4 is obsolete */
#define SNMPV2_GETBULKREQUEST       5
#define SNMPV2_INFORMREQUEST        6
#define SNMPV2_TRAP					7
#define SNMPV2_REPORT               8

// Defines Application types
#define SNMPV2_TAGCLASS_COUNTER64       BER_TAG_APPLICATION     /* u64 */
#define SNMPV2_TAG_COUNTER64            6
#define SNMPV2_TAGCLASS_VALUE_EXCEPTION BER_TAG_CONTEXT         /* NULL */
#define SNMPV2_TAG_NOSUCHOBJECT         0
#define SNMPV2_TAG_NOSUCHINSTANCE       1
#define SNMPV2_TAG_ENDOFMIBVIEW         2

namespace NetMan {

/**
 * @class Snmpv2Pdu
 */
class Snmpv2Pdu: public Snmpv1Pdu {
	private:
		std::shared_ptr<BerSequence> generateBulkRequest(u32 nonRepeaters, u32 maxRepetitions);
	public:
		Snmpv2Pdu(const std::string &community);
        virtual void sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, const std::string &ip);
		virtual void recvTrap(std::shared_ptr<UdpSocket> sock) override;
		~Snmpv2Pdu();
		friend class Snmpv3Pdu;
};

}

#endif
