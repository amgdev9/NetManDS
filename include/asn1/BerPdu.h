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

namespace NetMan {

class BerPdu {
	private:
		std::vector<BerField*> fields;
	public:
		BerPdu();
		void addField(BerField *field);
		void send(UdpSocket *sock, const char *ip, u16 port);
		void receive(UdpSocket *sock, const char *ip, u16 port);
		virtual ~BerPdu();
};

}

#endif
