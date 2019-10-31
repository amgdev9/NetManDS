/**
 * @file BerPdu.h
 * @brief 
 */

#ifndef BERPDU_H_
#define BERPDU_H_

// Includes C/C++
#include <vector>

// Own includes
#include "asn1/BerField.h"
#include "UdpSocket.h"

// Defines
#define BER_MAX_PDU_SIZE		(64 << 10)

namespace NetMan {

class BerPdu {
	private:
		std::vector<BerField*> fields;
	protected:
		void addField(BerField *field);
		void send(UdpSocket *sock, const char *ip, u16 port);
		u8 *receive(UdpSocket *sock, const char *ip, u16 port);
	public:
		BerPdu();
		virtual ~BerPdu();
};

}

#endif
